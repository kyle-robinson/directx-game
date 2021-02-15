#pragma once
#ifndef GRAPHICSCONTAINER_H
#define GRAPHICSCONTAINER_H

#include <map>
#include "Cube.h"
#include "Camera.h"
#include "Shaders.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "ImGuiManager.h"
#include "DirectionalLight.h"

namespace Bind
{
	class Blender;
	class DepthStencil;
	class Rasterizer;
	class RenderTarget;
	class Sampler;
	class Stencil;
	class SwapChain;
	class TextRenderer;
	class Viewport;
}

class Cube;

class GraphicsContainer
{
	friend class GraphicsResource;
public:
	// FUNCTIONS //
	enum ToolType { CONVERT, RESIZE } toolType = CONVERT;
	virtual ~GraphicsContainer( void ) = default;
	UINT GetWidth() const noexcept { return windowWidth; }
	UINT GetHeight() const noexcept { return windowHeight; }
protected:
	bool InitializeGraphics( HWND hWnd, int width, int height );
	void ClearScene();
	void UpdateRenderState();
	void UpdateConstantBuffers();

	void DrawWithOutline( Cube& cube, float scale, XMFLOAT3& color, ID3D11ShaderResourceView* texture );
	void DrawWithOutline( RenderableGameObject& object, float scale, XMFLOAT3& color );

	void RenderSceneText();
	void RenderImGuiWindows();
	void PresentScene();
private:
	bool InitializeDirectX( HWND hWnd );
	bool InitializeShaders();
public:
	// VARIABLES //
	int boxToUse = 0;
	int sizeAmount = 2;
	float sizeToUse = 1.0f;
	bool cubeHover = false;
	bool cubeInRange = false;
	bool holdingCube = false;
	std::string cameraToUse = "Default";
protected:
	float useTexture = 1.0f;
	float alphaFactor = 1.0f;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	std::map<std::string, std::unique_ptr<Camera>> cameras;

	std::shared_ptr<Bind::Blender> blender;
	std::shared_ptr<Bind::TextRenderer> textRenderer;
	std::map<std::string, std::shared_ptr<Bind::Sampler>> samplers;
	std::map<std::string, std::shared_ptr<Bind::Stencil>> stencils;
	std::map<std::string, std::shared_ptr<Bind::Rasterizer>> rasterizers;

	SpotLight spotLight;
	PointLight pointLight;
	DirectionalLight directionalLight;

	VertexShader vertexShader_2D;
	VertexShader vertexShader_light;
	VertexShader vertexShader_outline;
	PixelShader pixelShader_2D;
	PixelShader pixelShader_light;
	PixelShader pixelShader_noLight;
	PixelShader pixelShader_outline;
	PixelShader pixelShader_2D_discard;

	ConstantBuffer<CB_PS_spot> cb_ps_spot;
	ConstantBuffer<CB_PS_point> cb_ps_point;
	ConstantBuffer<CB_PS_scene> cb_ps_scene;
	ConstantBuffer<CB_VS_matrix> cb_vs_matrix;
	ConstantBuffer<CB_PS_outline> cb_ps_outline;
	ConstantBuffer<CB_PS_directional> cb_ps_directional;
private:
	UINT windowWidth;
	UINT windowHeight;
	ImGuiManager imgui;

	std::string samplerToUse = "Anisotropic";
	std::shared_ptr<Bind::Viewport> viewport;
	std::shared_ptr<Bind::SwapChain> swapChain;
	std::shared_ptr<Bind::RenderTarget> renderTarget;
	std::shared_ptr<Bind::DepthStencil> depthStencil;
};

#endif