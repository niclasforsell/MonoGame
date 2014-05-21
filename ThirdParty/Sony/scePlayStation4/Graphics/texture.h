#pragma once

#include "predecls.h"
#include "graphicsEnums.h"

namespace Graphics {

class CS_API Texture
{
	friend class GraphicsSystem;

private:

	sce::Gnm::Texture *_texture;

	// No default or copy constructors.
	Texture() { }
	Texture( const Texture & ) { }

public:

	static Texture* Create2D(TextureFormat format, int32_t width, int32_t height, int32_t mips);
	static Texture* Create3D(TextureFormat format, int32_t width, int32_t height, int32_t depth, int32_t mips);
	static Texture* CreateCube(TextureFormat format, int32_t width, int32_t height, int32_t mips);

	virtual ~Texture();

	void SetData(uint32_t level, uint8_t* data, uint32_t offset, uint32_t length);
	void GetData(uint32_t level, uint8_t* data, uint32_t offset, uint32_t length);

};

} // namespace Graphics
