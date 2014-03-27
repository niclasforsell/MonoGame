#pragma once

#include "predecls.h"


namespace Graphics {

	
class __declspec(dllexport) PixelShader
{
	friend class GraphicsSystem;

private:

	sce::Gnmx::PsShader *_shader;
	void *_binary;

public:

	PixelShader(const void *data);

	virtual ~PixelShader();

};


} // namespace Graphics
