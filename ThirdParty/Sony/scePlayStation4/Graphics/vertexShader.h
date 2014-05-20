#pragma once

#include "predecls.h"


namespace Graphics {

	
class CS_API VertexShader
{
	friend class GraphicsSystem;
	friend class FetchShader;

private:

	sce::Gnmx::VsShader *_shader;
	void *_binary;

public:
	
	VertexShader(const void *data);

	virtual ~VertexShader();

};


} // namespace Graphics
