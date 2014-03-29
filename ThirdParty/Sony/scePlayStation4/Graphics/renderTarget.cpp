#include "renderTarget.h"

#include "graphicsHelpers.h"
#include "..\allocator.h"
#include <gnm.h>

using namespace Graphics;


RenderTarget::RenderTarget(TextureFormat format_, int32_t width, int32_t height, DepthFormat depthFormat_)
{
	_renderTarget = new sce::Gnm::RenderTarget();

	Gnm::TileMode tileMode;
	auto format = ToDataFormat(format_);
	GpuAddress::computeSurfaceTileMode(&tileMode, GpuAddress::kSurfaceTypeColorTarget, format, 1);

	Gnm::SizeAlign renTargetSizeAlign = _renderTarget->init(
		width, height, 1,
		format,
		tileMode,
		Gnm::kNumSamples1,
		Gnm::kNumFragments1,
		NULL, NULL);

	void *rtData = Allocator::Get()->allocate(renTargetSizeAlign, SCE_KERNEL_WC_GARLIC);
	_renderTarget->setAddresses(rtData, 0, 0);;

	_texture = new sce::Gnm::Texture();
	_texture->initFromRenderTarget(_renderTarget, false);
	_texture->setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

	_depthTarget = NULL;
	_hasStencil = depthFormat_ == DepthFormat_Depth24Stencil8;
	if (depthFormat_ == DepthFormat_None)
		return;

	_depthTarget = new sce::Gnm::DepthRenderTarget();

	auto kStencilFormat = _hasStencil ? Gnm::kStencil8 : Gnm::kStencilInvalid;
	auto depthFormat = ToDataFormat(depthFormat_);
	Gnm::TileMode depthTileMode;
	GpuAddress::computeSurfaceTileMode(&depthTileMode, GpuAddress::kSurfaceTypeDepthOnlyTarget, depthFormat, 1);

	// Initialize the depth buffer descriptor
	Gnm::SizeAlign stencilSizeAlign;
	Gnm::SizeAlign htileSizeAlign;
	auto depthTargetSizeAlign = _depthTarget->init(
		width,
		height,
		depthFormat.getZFormat(),
		kStencilFormat,
		depthTileMode,
		Gnm::kNumFragments1,
		kStencilFormat != Gnm::kStencilInvalid ? &stencilSizeAlign : NULL,
		NULL);

	// Initialize the stencil buffer, if enabled
	void *stencilMemory = NULL;
	if( kStencilFormat != Gnm::kStencilInvalid )
		stencilMemory = Allocator::Get()->allocate(stencilSizeAlign, SCE_KERNEL_WC_GARLIC);

	// Allocate the depth buffer
	void *depthMemory = Allocator::Get()->allocate(depthTargetSizeAlign, SCE_KERNEL_WC_GARLIC);
	_depthTarget->setAddresses(depthMemory, stencilMemory);
}

RenderTarget::~RenderTarget()
{
	if (_depthTarget)
	{
		Allocator::Get()->release(_depthTarget->getStencilReadAddress());
		Allocator::Get()->release(_depthTarget->getZReadAddress());
		delete _depthTarget;
	}

	Allocator::Get()->release(_renderTarget->getBaseAddress());
	delete _renderTarget;
	delete _texture;
}

/*
void GraphicsSystem::GetRenderTargetData(RenderTarget *target, unsigned char *data, uint32_t bytes)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	gfxc.waitForGraphicsWrites(
		target->_renderTarget->getBaseAddress256ByteBlocks(), 
		target->_renderTarget->getSizeInBytes()>>8,
		Gnm::kWaitTargetSlotCb0, Gnm::kCacheActionWriteBackAndInvalidateL1andL2, Gnm::kExtendedCacheActionFlushAndInvalidateCbCache,
		Gnm::kStallCommandBufferParserEnable);

	// TODO: This blows!
	GpuAddress::TilingParameters tilingParameters;
	tilingParameters.m_tileMode = target->_renderTarget->getTileMode();
	tilingParameters.m_elemFormat = target->_renderTarget->getDataFormat();
	tilingParameters.m_linearWidth = target->_renderTarget->getWidth();
	tilingParameters.m_linearHeight = target->_renderTarget->getHeight();
	tilingParameters.m_linearDepth = 1;
	tilingParameters.m_numElementsPerPixel = 1;
	tilingParameters.m_baseTiledPitch = 0;
	tilingParameters.m_mipLevel = 0;
	tilingParameters.m_arraySlice = 0;
	tilingParameters.m_surfaceFlags.m_value = 0;
	tilingParameters.m_surfaceFlags.m_prt = 1;

	GpuAddress::detileSurface(data, target->_texture->getBaseAddress(), &tilingParameters);
}
*/
