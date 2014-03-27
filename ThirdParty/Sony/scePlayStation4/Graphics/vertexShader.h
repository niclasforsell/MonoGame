#pragma once

#include "predecls.h"


namespace Graphics {

	
class __declspec(dllexport) VertexShader
{
	friend class GraphicsSystem;

private:

	sce::Gnmx::VsShader *_shader;
	void *_binary;
	void *_fsMem;
	uint32_t _shaderModifier;

public:
	
	VertexShader(const void *data);

	virtual ~VertexShader();

};


} // namespace Graphics
