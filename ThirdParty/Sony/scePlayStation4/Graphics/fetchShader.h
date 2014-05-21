#pragma once

#include "predecls.h"


namespace Graphics {

	
class CS_API FetchShader
{
	friend class GraphicsSystem;

private:

	void *_fsMem;
	uint32_t _shaderModifier;

public:
	
	FetchShader(VertexShader *shader, const uint32_t* remap, int32_t count);

	virtual ~FetchShader();

};


} // namespace Graphics
