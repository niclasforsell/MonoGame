#pragma once

#include "predecls.h"
#include "graphicsEnums.h"

namespace Graphics {

class __declspec(dllexport) Texture
{
	friend class GraphicsSystem;

private:

	sce::Gnm::Texture *_texture;

	// No default or copy constructors.
	Texture() { }
	Texture( const Texture & ) { }

public:

	Texture(TextureFormat format, int32_t width, int32_t height, int32_t mips);
	virtual ~Texture();

	void SetData(uint32_t level, uint8_t *data, uint32_t bytes);
	void GetData(uint32_t level, uint8_t *data, uint32_t bytes);

};

} // namespace Graphics