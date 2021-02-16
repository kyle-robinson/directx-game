#include "Quad.h"
#include "Indices.h"
#include "Vertices.h"

bool Quad::Initialize( ID3D11DeviceContext* context, ID3D11Device* device )
{
    this->context = context;

    try
    {
        HRESULT hr = vb_plane.Initialize( device, Vtx::verticesQuad, ARRAYSIZE( Vtx::verticesQuad ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create quad vertex buffer!" );
        hr = ib_plane.Initialize( device, Idx::indicesQuad, ARRAYSIZE( Idx::indicesQuad ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create quad index buffer!" );

        SetPosition( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
        SetRotation( XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
        SetScale( 1.0f, 1.0f );
        UpdateMatrix();
    }
    catch ( COMException& exception )
    {
        ErrorLogger::Log( exception );
        return false;
    }

    return true;
}

void Quad::Draw( ConstantBuffer<CB_VS_matrix>& cb_vs_matrix,
    ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* textureNormal ) noexcept
{
    UINT offset = 0u;
    context->IASetVertexBuffers( 0u, 1u, vb_plane.GetAddressOf(), vb_plane.StridePtr(), &offset );
    context->IASetIndexBuffer( ib_plane.Get(), DXGI_FORMAT_R16_UINT, 0u );
    context->PSSetShaderResources( 0u, 1u, &texture );
    context->PSSetShaderResources( 1u, 1u, &textureNormal );
    cb_vs_matrix.data.worldMatrix = XMMatrixIdentity() * worldMatrix;
    if ( !cb_vs_matrix.ApplyChanges() ) return;
    context->VSSetConstantBuffers( 0u, 1u, cb_vs_matrix.GetAddressOf() );
    context->DrawIndexed( ib_plane.IndexCount(), 0u, 0u );
}