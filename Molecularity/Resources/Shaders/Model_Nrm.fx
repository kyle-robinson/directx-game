#pragma pack_matrix( row_major )

// VERTEX SHADER
cbuffer ObjectBuffer : register( b0 )
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

cbuffer FogBuffer : register( b1 )
{
    float3 fogColor;
    float fogStart;
    
    float fogEnd;
    bool fogEnable;
}

struct VS_INPUT
{
    float3 inPosition : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outWorldPos : WORLD_POSITION;
    float outFog : FOG;
};

VS_OUTPUT VS( VS_INPUT input )
{
    VS_OUTPUT output;
        
    output.outPosition = mul( float4( input.inPosition, 1.0f ), worldMatrix );
    output.outPosition = mul( output.outPosition, viewMatrix );
    output.outPosition = mul( output.outPosition, projectionMatrix );
        
    output.outTexCoord = input.inTexCoord;
    output.outNormal = normalize( mul( float4( input.inNormal, 0.0f ), worldMatrix ) );
    
    output.outWorldPos = (float3)mul( float4( input.inPosition, 1.0f ), worldMatrix );
    float3 cameraPosition = (float3)mul( float4( output.outWorldPos, 1.0f ), viewMatrix );
    output.outFog = saturate( ( fogEnd - cameraPosition.z ) / ( fogEnd - fogStart ) ); // linear fog
    
    return output;
}

// PIXEL SHADER
cbuffer SceneBuffer : register( b2 )
{
    bool useTexture;
    float alphaFactor;
    bool useNormalMap;
}

cbuffer PointLightBuffer : register( b3 )
{
    float3 pointAmbientColor;
    float pointAmbientStrength;
    
    float3 pointDiffuseColor;
    float pointDiffuseStrength;
    
    float3 pointSpecularColor;
    float pointSpecularStrength;
    
    float pointSpecularPower;
    float3 pointPosition;
    
    float pointConstant;
    float pointLinear;
    float pointQuadratic;
    bool pointEnable;
};

cbuffer DirectionalLightBuffer : register( b4 )
{
    float3 directionalPosition;
    float directionalDiffuseStrength;
    
    float3 directionalDiffuseColor;
    float directionalSpecularStrength;
    
    float3 directionalSpecularColor;
    float directionalSpecularPower;
    
    bool directionalEnable;
}

cbuffer SpotLightBuffer : register( b5 )
{
    float spotRange;
    float3 spotPosition;
    
    float spotCone;
    float3 spotDirection;
    
    float spotDiffuseStrength;
    float3 spotDiffuseColor;
    
    bool spotEnable;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WORLD_POSITION;
    float inFog : FOG;
};

Texture2D objTexture : TEXTURE : register( t0 );
Texture2D normalTexture : TEXTURE : register( t1 );
SamplerState samplerState : SAMPLER : register( s0 );

float4 PS( PS_INPUT input ) : SV_TARGET
{    
    float3 cumulativeColor = { 0.0f, 0.0f, 0.0f };
    
    if ( useNormalMap )
    {
        const float3 normalSample = normalTexture.Sample( samplerState, input.inTexCoord );
        input.inNormal.x = normalSample.x * 2.0f - 1.0f;
        input.inNormal.y = -normalSample.y * 2.0f - 1.0f;
        input.inNormal.z = -normalSample.z * 2.0f - 1.0f;
        input.inNormal = mul( input.inNormal, (float3x3)mul( worldMatrix, viewMatrix ) );
    }
    
    // DIRECTIONAL LIGHT
    {
        if ( directionalEnable )
        {
            // Create vector between light and pixel positions and get length
            const float3 toLight = normalize( directionalPosition - input.inWorldPos );
            const float distanceToLight = length( toLight );
        
            // Create a vector for the direction to the light from the camera
            const float3 directionToLight = toLight / distanceToLight;
            const float NDotL = dot( directionToLight, input.inNormal );
        
            // Diffuse component
            const float3 diffuse = directionalDiffuseColor * saturate( NDotL ) * directionalDiffuseStrength;
        
            // Get the angle bewteen object normals and the light position
            const float3 incidence = input.inNormal * dot( toLight, input.inNormal );
        
            // Calculate angle between incidence and camera's vector to the light
            const float3 reflection = incidence * 2.0f - toLight;
        
            // Specular component
            const float3 specular = directionalSpecularColor * directionalSpecularStrength *
                pow( max( 0.0f, dot( normalize( -reflection ), normalize( input.inWorldPos ) ) ), directionalSpecularPower );
        
            cumulativeColor += diffuse + specular;
        }
    }
    
    // POINT LIGHT
    {
        if ( pointEnable )
        {
            // Create vector between light and pixel positions
            const float3 vToL = normalize( pointPosition - input.inWorldPos );
        
            // Attenuation factor according to point light position
            const float attenuation = 1 / ( pointConstant + pointLinear * vToL + pointQuadratic * pow( vToL, 2 ) );
        
            // Ambient component
            const float3 ambient = pointAmbientColor * pointAmbientStrength;

            // Diffuse component
            const float3 diffuseIntensity = max( dot( vToL, input.inNormal ), 0 ) * attenuation;
            const float3 diffuse = diffuseIntensity * pointDiffuseStrength * pointDiffuseColor;

            // Get the angle bewteen object normals and the light position
            const float3 incidence = input.inNormal * dot( vToL, input.inNormal );
        
            // Calculate angle between incidence and camera's vector to the light
            const float3 reflection = incidence * 2.0f - vToL;
        
            // Specular component
            const float3 specular = pointSpecularColor * pointSpecularStrength * attenuation *
                pow( max( 0.0f, dot( normalize( -reflection ), normalize( input.inWorldPos ) ) ), pointSpecularPower );
        
            cumulativeColor += ambient + diffuse + specular;
        }
    }
    
    // SPOT LIGHT
    {
        if ( spotEnable )
        {
            // Create vector between light and pixel positions
            float3 lightToPixelVec = normalize( spotPosition - input.inWorldPos );
        
            // Get the distance between the light and pixel positions
            const float lightDistance = length( lightToPixelVec );
        
            // If the pixel is too far, don't run any lighting calculations
            if ( lightDistance < spotRange )
            {
                float3 finalSpotLightColor = { 0.0f, 0.0f, 0.0f };
            
                // Convert to unit length
                lightToPixelVec /= lightDistance;
            
                // Calculate how much light the pixel gets by the angle in which the light strikes the pixel surface
                const float howMuchLight = dot( lightToPixelVec, normalize( input.inNormal ) );
            
                // Check if light is striking the front side of the pixel
                if ( howMuchLight > 0.0f )
                {
                    finalSpotLightColor += spotDiffuseColor * spotDiffuseStrength;
                
                    // Calculate falloff from center to edge of point light cone
                    finalSpotLightColor *= pow( max( dot( -lightToPixelVec, spotDirection ), 0.0f ), spotCone );
                }
            
                cumulativeColor += finalSpotLightColor;       
            }
        }
    }
    
    // sample from texture
    float4 textureColor = useTexture ? objTexture.Sample( samplerState, input.inTexCoord ) : 1.0f;
    if ( textureColor.a < 0.1f ) discard;
    
    // Output colour
    float4 finalColor = saturate( float4( cumulativeColor, 1.0f ) );
    finalColor *= textureColor;
    
    // FOG FACTOR
    {
        if ( fogEnable )
        {
            float fogValue = input.inFog * finalColor + ( 1.0 - input.inFog );
            finalColor += fogValue * float4( fogColor, 1.0f );
        }
    }
    
    return finalColor;
    //return float4( finalColor, alphaFactor );
}