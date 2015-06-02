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
	RenderTarget() { }
	RenderTarget( const RenderTarget & ) { }

	void Init(TextureFormat format, int32_t width, int32_t height, int32_t slices, DepthFormat depthFormat, bool isCubemap);

public:

	RenderTarget(GraphicsSystem *system);
	virtual ~RenderTarget();

	void Init2D(TextureFormat format, int32_t width, int32_t height, DepthFormat depthFormat);
	void Init3D(TextureFormat format, int32_t width, int32_t height, int32_t slices, DepthFormat depthFormat);
	void InitCube(TextureFormat format, int32_t width, int32_t height, DepthFormat depthFormat);

	void InitFromTexture2D(Texture* texture, DepthFormat depthFormat);

	CS_IGNORE unsigned char* GetDataDetiled(uint64_t *levelOffset, uint64_t *levelSize, uint32_t mipLevel);

};

} // namespace Graphics
