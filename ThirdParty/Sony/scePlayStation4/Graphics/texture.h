#pragma once

#include "predecls.h"
#include "graphicsEnums.h"

namespace Graphics {

class CS_API Texture
{
	friend class GraphicsSystem;
	friend class RenderTarget;

private:

	sce::Gnm::Texture *_texture;
	bool _ownsTexture;
	bool _isTarget;

	// No public copy constructors.
	Texture( const Texture & ) { }

public:

	Texture();
	virtual ~Texture();

	void Init2D(TextureFormat format, int32_t width, int32_t height, int32_t mips);
	void Init3D(TextureFormat format, int32_t width, int32_t height, int32_t depth, int32_t mips);
	void InitCube(TextureFormat format, int32_t width, int32_t height, int32_t mips);

	void SetData(uint32_t mipLevel, uint8_t* data, uint32_t offset, uint32_t length);
	void GetData(uint32_t mipLevel, uint8_t* data, uint32_t offset, uint32_t length);

	CS_IGNORE sce::Gnm::Texture* GetInternalData() const;
};

} // namespace Graphics
