#pragma once

#include "predecls.h"


namespace Graphics {

	
class CS_API PixelShader
{
	friend class GraphicsSystem;

private:

	sce::Gnmx::PsShader *_shader;
	void *_binary;

public:

	PixelShader(const void *data);

	virtual ~PixelShader();

	CS_IGNORE sce::Gnmx::PsShader* GetShader() const;
};


} // namespace Graphics
