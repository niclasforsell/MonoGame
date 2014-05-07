#pragma once

#include "predecls.h"


namespace Graphics {

	
class __declspec(dllexport) FetchShader
{
	friend class GraphicsSystem;

private:

	void *_fsMem;
	uint32_t _shaderModifier;

public:
	
	FetchShader(VertexShader *shader);

	virtual ~FetchShader();

};


} // namespace Graphics
