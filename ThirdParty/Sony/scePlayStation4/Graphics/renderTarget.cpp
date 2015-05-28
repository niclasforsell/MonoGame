#include "renderTarget.h"
#include "texture.h"

#include "graphicsSystem.h"
#include "graphicsHelpers.h"
#include "..\allocator.h"
#include <assert.h>
#include <gnm.h>
#include <scebase.h>

using namespace Graphics;


RenderTarget::RenderTarget(GraphicsSystem *system)
	: Texture(system)
{
	_detileTemp = nullptr;
}

RenderTarget::~RenderTarget()
{
	if (_detileTemp != nullptr)
		mem::free(_detileTemp);

	if (_depthTarget)
	{
		_system->_safeDeleteBuffer(_depthTarget->getStencilReadAddress());
		_system->_safeDeleteBuffer(_depthTarget->getZReadAddress());
		delete _depthTarget;
	}

	delete _renderTarget;
}


void RenderTarget::Init(TextureFormat format_, int32_t width, int32_t height, int32_t slices, DepthFormat depthFormat_, bool isCubemap)
{
	auto renderTarget = new sce::Gnm::RenderTarget();
	_renderTarget = renderTarget;
	_isTarget = true;

	Gnm::TileMode tileMode;
	auto format = ToDataFormat(format_);
	GpuAddress::computeSurfaceTileMode(&tileMode, GpuAddress::kSurfaceTypeColorTarget, format, 1);

	Gnm::SizeAlign renTargetSizeAlign = renderTarget->init(
		width, height, slices,
		format,
		tileMode,
		Gnm::kNumSamples1,
		Gnm::kNumFragments1,
		NULL, NULL);

	void *rtData = mem::allocShared(renTargetSizeAlign);
	renderTarget->setAddresses(rtData, 0, 0);;

	_texture = new sce::Gnm::Texture();
	_texture->initFromRenderTarget(renderTarget, isCubemap);
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
		stencilMemory = mem::allocShared(stencilSizeAlign);

	// Allocate the depth buffer
	void *depthMemory = mem::allocShared(depthTargetSizeAlign);
	_depthTarget->setAddresses(depthMemory, stencilMemory);
}

void RenderTarget::Init2D(TextureFormat format, int32_t width, int32_t height, DepthFormat depthFormat)
{
	Init(format, width, height, 1, depthFormat, false);
}

void RenderTarget::Init3D(TextureFormat format, int32_t width, int32_t height, int32_t slices, DepthFormat depthFormat)
{
    Init(format, width, height, slices, depthFormat, false);
}

void RenderTarget::InitCube(TextureFormat format, int32_t width, int32_t height, DepthFormat depthFormat)
{
    Init(format, width, height, 1, depthFormat, true);
}

void RenderTarget::InitFromTexture2D(Texture* texture, DepthFormat depthFormat_)
{
	auto colorTex = texture->GetInternalData();
	auto colorWidth = colorTex->getWidth();
	auto colorHeight = colorTex->getHeight();

	auto renderTarget = new sce::Gnm::RenderTarget();
	_renderTarget = renderTarget;
	_texture = colorTex;
	_ownsTexture = false;
	_isTarget = true;

	auto ret = renderTarget->initFromTexture(colorTex, 0);
	assert(ret == GpuAddress::kStatusSuccess);

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
		colorWidth,
		colorHeight,
		depthFormat.getZFormat(),
		kStencilFormat,
		depthTileMode,
		Gnm::kNumFragments1,
		kStencilFormat != Gnm::kStencilInvalid ? &stencilSizeAlign : NULL,
		NULL);

	// Initialize the stencil buffer, if enabled
	void *stencilMemory = NULL;
	if( kStencilFormat != Gnm::kStencilInvalid )
		stencilMemory = mem::allocShared(stencilSizeAlign);

	// Allocate the depth buffer
	void *depthMemory = mem::allocShared(depthTargetSizeAlign);
	_depthTarget->setAddresses(depthMemory, stencilMemory);
}

unsigned char* RenderTarget::GetDataDetiled(uint64_t *levelOffset, uint64_t *levelSize, uint32_t mipLevel)
{
	assert(mipLevel == 0);

	// TODO: We really should be doing this!
	/*
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	gfxc.waitForGraphicsWrites(
		target->_renderTarget->getBaseAddress256ByteBlocks(), 
		target->_renderTarget->getSizeInBytes()>>8,
		Gnm::kWaitTargetSlotCb0, Gnm::kCacheActionWriteBackAndInvalidateL1andL2, Gnm::kExtendedCacheActionFlushAndInvalidateCbCache,
		Gnm::kStallCommandBufferParserEnable);
	*/

	GpuAddress::TilingParameters tilingParameters;
	tilingParameters.m_tileMode = _renderTarget->getTileMode();
	tilingParameters.m_elemFormat = _renderTarget->getDataFormat();
	tilingParameters.m_linearWidth = _renderTarget->getWidth();
	tilingParameters.m_linearHeight = _renderTarget->getHeight();
	tilingParameters.m_linearDepth = 1;
	tilingParameters.m_numElementsPerPixel = 1;
	tilingParameters.m_baseTiledPitch = 0;
	tilingParameters.m_mipLevel = 0;
	tilingParameters.m_arraySlice = 0;
	tilingParameters.m_surfaceFlags.m_value = 0;
	tilingParameters.m_surfaceFlags.m_prt = 1;

#if SCE_ORBIS_SDK_VERSION >= 0x02000131u // SDK Version 2.0
	uint64_t surfaceSize64 = 0;
	Gnm::AlignmentType surfaceAlign;
	auto status = GpuAddress::computeUntiledSurfaceSize(&surfaceSize64, &surfaceAlign, &tilingParameters);
	SCE_GNM_ASSERT(status == GpuAddress::kStatusSuccess);
	SCE_GNM_ASSERT(surfaceSize64 < 0xFFFFFFFFULL); // surface size is >=4GB and will be clamped to 32 bits; use the 64-bit variant instead.
	Gnm::SizeAlign size(static_cast<uint32_t>(surfaceSize64), surfaceAlign);
#else
	auto size = GpuAddress::computeUntiledSurfaceSize(&tilingParameters);
#endif

	if (_detileTemp == nullptr)
		_detileTemp = mem::alloc(size.m_size, size.m_align);

	GpuAddress::detileSurface(_detileTemp, _renderTarget->getBaseAddress(), &tilingParameters);

	*levelOffset = 0;
	*levelSize = size.m_size;

	return (unsigned char*)_detileTemp;
}
