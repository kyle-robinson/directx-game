#pragma once
#ifndef STENCILOUTLINE_H
#define STENCILOUTLINE_H

#include "Stencil.h"
#include "InputLayout.h"
#include "GraphicsResource.h"

namespace Bind
{
	class StencilOutline : public GraphicsResource
	{
	public:
		StencilOutline( GraphicsContainer& gfx, float outlineScale = 0.1f, XMFLOAT3 outlineColor = XMFLOAT3( 1.0f, 0.6f, 0.1f ) )
			: scale( outlineScale ), color( outlineColor )
		{			
			try
			{				
				HRESULT hr = cb_ps_outline.Initialize( GetDevice( gfx ), GetContext( gfx ) );
				COM_ERROR_IF_FAILED( hr, "Failed to create 'cb_ps_outline' constant buffer!" );

				hr = vertexShader_outline.Initialize( GetDeviceRef( gfx ), L"Resources\\Shaders\\Model_Outline.fx",
					Layout::layoutPosCol, ARRAYSIZE( Layout::layoutPosCol ) );
				hr = pixelShader_outline.Initialize( GetDeviceRef( gfx ), L"Resources\\Shaders\\Model_Outline.fx" );
				COM_ERROR_IF_FAILED( hr, "Failed to create 'Outline' shaders!" );

			}
			catch ( COMException& exception )
			{
				ErrorLogger::Log( exception );
				return;
			}
		}
		void DrawWithOutline( GraphicsContainer& gfx, Cube& cube, ConstantBuffer<CB_VS_matrix>& cb_vs_matrix,
			ID3D11ShaderResourceView* texture )
		{
			// write pixels to the buffer, which will act as the stencil mask
			GetStencil( gfx, "Write" )->Bind( gfx );
			cube.Draw( cb_vs_matrix, texture );

			// scale the cube and draw the stencil outline, ignoring the pixels previously written to the buffer
			cb_ps_outline.data.outlineColor = color;
			if ( !cb_ps_outline.ApplyChanges() ) return;
			GetContext( gfx )->PSSetConstantBuffers( 1u, 1u, cb_ps_outline.GetAddressOf() );
			Shaders::BindShaders( GetContext( gfx ), vertexShader_outline, pixelShader_outline );
			GetStencil( gfx, "Mask" )->Bind( gfx );
			cube.SetScale(
				cube.GetScaleFloat3().x + scale,
				cube.GetScaleFloat3().y + scale,
				cube.GetScaleFloat3().z + scale
			);
			cube.Draw( cb_vs_matrix, texture );

			// rescale the cube and draw using the appropriate shaders and textures
			if ( !GetLightCB( gfx ).ApplyChanges() ) return;
			GetContext( gfx )->PSSetConstantBuffers( 3u, 1u, GetLightCB( gfx ).GetAddressOf() );
			Shaders::BindShaders( GetContext( gfx ), GetLightVS( gfx ), GetLightPS( gfx ) );
			cube.SetScale(
				cube.GetScaleFloat3().x - scale,
				cube.GetScaleFloat3().y - scale,
				cube.GetScaleFloat3().z - scale
			);
			GetStencil( gfx, "Off" )->Bind( gfx );
			cube.Draw( cb_vs_matrix, texture );
		}
		void DrawWithOutline( GraphicsContainer& gfx, RenderableGameObject& object )
		{
			// write pixels to the buffer, which will act as the stencil mask
			GetStencil( gfx, "Write" )->Bind( gfx );
			object.Draw();

			// scale the model and draw the stencil outline, ignoring the pixels previously written to the buffer
			cb_ps_outline.data.outlineColor = color;
			if ( !cb_ps_outline.ApplyChanges() ) return;
			GetContext( gfx )->PSSetConstantBuffers( 1u, 1u, cb_ps_outline.GetAddressOf() );
			Shaders::BindShaders( GetContext( gfx ), vertexShader_outline, pixelShader_outline );
			GetStencil( gfx, "Mask" )->Bind( gfx );
			object.SetScale( object.GetScaleFloat3().x + scale, 1.0f, object.GetScaleFloat3().z + scale );
			object.Draw();

			// rescale the model and draw using the appropriate shaders and textures
			if ( !GetLightCB( gfx ).ApplyChanges() ) return;
			GetContext( gfx )->PSSetConstantBuffers( 3u, 1u, GetLightCB( gfx ).GetAddressOf() );
			Shaders::BindShaders( GetContext( gfx ), GetLightVS( gfx ), GetLightPS( gfx ) );
			object.SetScale( object.GetScaleFloat3().x - scale, 1.0f, object.GetScaleFloat3().z - scale );
			GetStencil( gfx, "Off" )->Bind( gfx );
			object.Draw();
		}
	private:
		float scale;
		XMFLOAT3 color;
		ConstantBuffer<CB_PS_outline> cb_ps_outline;
		VertexShader vertexShader_outline;
		PixelShader pixelShader_outline;
	};
}

#endif