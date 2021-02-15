#pragma once
#ifndef GRAPHICSRESOURCE_H
#define GRAPHICSRESOURCE_H

#include "GraphicsContainer.h"

class GraphicsResource
{
protected:
	static ID3D11DeviceContext* GetContext( GraphicsContainer& gfx ) noexcept { return gfx.context.Get(); }
	static ID3D11Device* GetDevice( GraphicsContainer& gfx ) noexcept { return gfx.device.Get(); }
	virtual void Bind( GraphicsContainer& gfx ) noexcept { UNREFERENCED_PARAMETER( gfx ); }
	virtual ~GraphicsResource() = default;
};

#endif