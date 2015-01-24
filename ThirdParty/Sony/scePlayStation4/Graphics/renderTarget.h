#pragma once

#include "predecls.h"
#include "graphicsEnums.h"
#include "texture.h"

namespace Graphics {


class CS_API RenderTarget : Texture
{
	friend class GraphicsSystem;

private:

	sce::Gnm::RenderTarget *_renderTarget;
	void* _detileTemp;


	sce::Gnm::DepthRenderTarget *_depthTarget;
	bool _hasStencil;

	// No public default or copy constructors.
	RenderTarget();
	RenderTarget( const RenderTarget & );

	static RenderTarget* Create(TextureFormat format, int32_t width, int32_t height, int32_t slices, DepthFormat depthFormat, bool isCubemap);

public:
	static RenderTarget* Create2D(TextureFormat format, int32_t width, int32_t height, DepthFormat depthFormat);
	static RenderTarget* Create3D(TextureFormat format, int32_t width, int32_t height, int32_t slices, DepthFormat depthFormat);
	static RenderTarget* CreateCube(TextureFormat format, int32_t width, int32_t height, DepthFormat depthFormat);

	static RenderTarget* CreateFromTexture2D(Texture* texture, DepthFormat depthFormat);

	CS_IGNORE unsigned char* GetDataDetiled(uint64_t *levelOffset, uint64_t *levelSize, uint32_t mipLevel);

	virtual ~RenderTarget();

};

} // namespace Graphics
