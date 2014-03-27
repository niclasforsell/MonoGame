
#include "graphicsSystem.h"

#include "texture.h"
#include "vertexShader.h"
#include "pixelShader.h"
#include "renderTarget.h"
#include "displayBuffer.h"
#include "vertexBuffer.h"

#include "../allocator.h"

#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
//#include <scebase.h>
#include <kernel.h>

#include <video_out.h>
#include <user_service.h>
#include <gnm.h>
#include <gnmx.h>
#include <libsysmodule.h>
#include <png_dec.h>


using namespace sce;
using namespace sce::Gnmx;
using namespace Graphics;

/*
class SpriteMeshVertex
{
public:
	Vector3Unaligned Position;
	Vector4Unaligned Color;
	Vector2Unaligned TexCoords;
};
*/

GraphicsSystem::GraphicsSystem()
{
	sceSysmoduleLoadModule(SCE_SYSMODULE_PNG_DEC);

	_backBufferIndex = 0;
	memset(_surfaceAddresses, 0, sizeof(_surfaceAddresses));

	_videoOutHandle = 0;
	_frameIndex = 0;

	//_effect = NULL;
	//_effectDirty = false;
}

GraphicsSystem::~GraphicsSystem()
{
}

void GraphicsSystem::Initialize()
{
	//printf("Initialize!\n");

	// Open the video output port
	_videoOutHandle = sceVideoOutOpen(0, SCE_VIDEO_OUT_BUS_TYPE_MAIN, 0, NULL);
	if( _videoOutHandle < 0 )
	{
		printf("sceVideoOutOpen failed: 0x%08X\n", _videoOutHandle);
		return;
	}

	// Initialize the flip rate: 0: 60Hz, 1: 30Hz or 2: 20Hz
	int ret = sceVideoOutSetFlipRate(_videoOutHandle, 0);
	if( ret != SCE_OK )
	{
		printf("sceVideoOutSetFlipRate failed: 0x%08X\n", ret);
		return;
	}

	// Create the event queue for used to synchronize with end-of-pipe interrupts
	ret = sceKernelCreateEqueue(&_eopEventQueue, "EOP QUEUE");
	if( ret != SCE_OK )
	{
		printf("sceKernelCreateEqueue failed: 0x%08X\n", ret);
		return;
	}

	// Register for the end-of-pipe events
	ret = Gnm::addEqEvent(_eopEventQueue, Gnm::kEqEventGfxEop, NULL);
	if( ret != SCE_OK )
	{
		printf("Gnm::addEqEvent failed: 0x%08X\n", ret);
		return;
	}

	/*
	// Allocate the scratch pad memory required by the Toolkit module
	Toolkit::MemoryRequests toolkitMemRequests;
	toolkitMemRequests.initialize();
	Toolkit::addToMemoryRequests(&toolkitMemRequests);
	void *toolkitMemGarlic = Allocator::Get()->allocate(toolkitMemRequests.m_garlic.m_sizeAlign, SCE_KERNEL_WC_GARLIC);
	void *toolkitMemOnion = Allocator::Get()->allocate(toolkitMemRequests.m_onion.m_sizeAlign, SCE_KERNEL_WB_ONION);
	if( !toolkitMemGarlic || !toolkitMemOnion )
	{
		printf("Cannot allocate the scratch pad area for the Toolkit module\n");
		return; // SCE_KERNEL_ERROR_ENOMEM;
	}

	// Initialize the Toolkit module
	toolkitMemRequests.m_garlic.fulfill(toolkitMemGarlic);
	toolkitMemRequests.m_onion.fulfill(toolkitMemOnion);
	Toolkit::initializeWithMemoryRequests(&toolkitMemRequests);
	*/

	static const sce::Gnm::ZFormat kZFormat				= sce::Gnm::kZFormat32Float;
	static const sce::Gnm::StencilFormat kStencilFormat	= sce::Gnm::kStencil8;
	_displayBuffers = new DisplayBuffer[kDisplayBufferCount];

	for(uint32_t i=0; i<kDisplayBufferCount; ++i)
	{
		// Allocate the shadow copy of the CP ram
		_displayBuffers[i].cpRamShadow = malloc(
			Gnmx::ConstantUpdateEngine::computeCpRamShadowSize());

		if( !_displayBuffers[i].cpRamShadow )
		{
			printf("Cannot allocate the CP ram shadow memory\n");
			return;
		}

		// Allocate the CUE heap memory
		_displayBuffers[i].cueHeap = Allocator::Get()->allocate(
			Gnmx::ConstantUpdateEngine::computeHeapSize(kCueRingEntries),
			Gnm::kAlignmentOfBufferInBytes,
			SCE_KERNEL_WC_GARLIC);

		if( !_displayBuffers[i].cueHeap )
		{
			printf("Cannot allocate the CUE heap memory\n");
			return;
		}

		// Allocate the draw command buffer
		_displayBuffers[i].dcbBuffer = Allocator::Get()->allocate(
			kDcbSizeInBytes,
			Gnm::kAlignmentOfBufferInBytes,
			SCE_KERNEL_WB_ONION);

		if( !_displayBuffers[i].dcbBuffer )
		{
			printf("Cannot allocate the draw command buffer memory\n");
			return; // SCE_KERNEL_ERROR_ENOMEM;
		}

		// Allocate the constants command buffer
		_displayBuffers[i].ccbBuffer = Allocator::Get()->allocate(
			kCcbSizeInBytes,
			Gnm::kAlignmentOfBufferInBytes,
			SCE_KERNEL_WB_ONION);

		if( !_displayBuffers[i].ccbBuffer )
		{
			printf("Cannot allocate the constants command buffer memory\n");
			return; // SCE_KERNEL_ERROR_ENOMEM;
		}

		// Initialize the current display context
		_displayBuffers[i].context.init(
			_displayBuffers[i].cpRamShadow,
			_displayBuffers[i].cueHeap,
			kCueRingEntries,
			_displayBuffers[i].dcbBuffer,
			kDcbSizeInBytes,
			_displayBuffers[i].ccbBuffer,
			kCcbSizeInBytes);

		// Compute the tiling mode for the render target
		Gnm::TileMode tileMode;
		Gnm::DataFormat format = Gnm::kDataFormatB8G8R8A8Unorm;
		if( !GpuAddress::computeSurfaceTileMode(&tileMode,
			GpuAddress::kSurfaceTypeColorTargetDisplayable, format, 1) )
		{
			printf("Cannot compute the tile mode for the render target surface\n");
			return; // SCE_KERNEL_ERROR_UNKNOWN;
		}

		// Initialize the render target descriptor
		Gnm::SizeAlign sizeAlign = _displayBuffers[i].renderTarget.init(
			kDisplayBufferWidth,
			kDisplayBufferHeight,
			1,
			format,
			tileMode,
			Gnm::kNumSamples1,
			Gnm::kNumFragments1,
			NULL,
			NULL);

		// Allocate the render target memory
		_surfaceAddresses[i] = Allocator::Get()->allocate(sizeAlign, SCE_KERNEL_WC_GARLIC);
		if( !_surfaceAddresses[i] )
		{
			printf("Cannot allocate the render target memory\n");
			return; // SCE_KERNEL_ERROR_ENOMEM;
		}
		_displayBuffers[i].renderTarget.setAddresses(_surfaceAddresses[i], 0, 0);

		// Compute the tiling mode for the depth buffer
		Gnm::DataFormat depthFormat = Gnm::DataFormat::build(kZFormat);
		Gnm::TileMode depthTileMode;
		if( !GpuAddress::computeSurfaceTileMode(&depthTileMode,
			GpuAddress::kSurfaceTypeDepthOnlyTarget, depthFormat, 1) )
		{
			printf("Cannot compute the tile mode for the depth stencil surface\n");
			return; // SCE_KERNEL_ERROR_UNKNOWN;
		}

		// Initialize the depth buffer descriptor
		Gnm::SizeAlign stencilSizeAlign;
		Gnm::SizeAlign htileSizeAlign;
		Gnm::SizeAlign depthTargetSizeAlign = _displayBuffers[i].depthTarget.init(
			kDisplayBufferWidth,
			kDisplayBufferHeight,
			depthFormat.getZFormat(),
			kStencilFormat,
			depthTileMode,
			Gnm::kNumFragments1,
			kStencilFormat != Gnm::kStencilInvalid ? &stencilSizeAlign : NULL,
			kHtileEnabled ? &htileSizeAlign : NULL);

		// Initialize the HTILE buffer, if enabled
		if( kHtileEnabled )
		{
			void *htileMemory = Allocator::Get()->allocate(htileSizeAlign, SCE_KERNEL_WC_GARLIC);
			if( !htileMemory )
			{
				printf("Cannot allocate the HTILE buffer\n");
				return; // SCE_KERNEL_ERROR_ENOMEM;
			}

			_displayBuffers[i].depthTarget.setHtileAddress(htileMemory);
		}

		// Initialize the stencil buffer, if enabled
		void *stencilMemory = NULL;
		if( kStencilFormat != Gnm::kStencilInvalid )
		{
			stencilMemory = Allocator::Get()->allocate(stencilSizeAlign, SCE_KERNEL_WC_GARLIC);
			if( !stencilMemory )
			{
				printf("Cannot allocate the stencil buffer\n");
				return; // SCE_KERNEL_ERROR_ENOMEM;
			}
		}

		// Allocate the depth buffer
		void *depthMemory = Allocator::Get()->allocate(depthTargetSizeAlign, SCE_KERNEL_WC_GARLIC);
		if( !depthMemory )
		{
			printf("Cannot allocate the depth buffer\n");
			return; // SCE_KERNEL_ERROR_ENOMEM;
		}
		_displayBuffers[i].depthTarget.setAddresses(depthMemory, stencilMemory);

		_displayBuffers[i].state = (volatile uint32_t*) Allocator::Get()->allocate(4, 8, SCE_KERNEL_WB_ONION);
		if( !_displayBuffers[i].state )
		{
			printf("Cannot allocate a GPU label\n");
			return; // SCE_KERNEL_ERROR_ENOMEM;
		}

		_displayBuffers[i].state[0] = kDisplayBufferIdle;
		
		/*
		// Allocate enough space for 4000 verts and index data.
		_displayBuffers[i].userData = Allocator::Get()->allocate(DisplayBuffer::MaxBufferSize, Gnm::kAlignmentOfBufferInBytes, SCE_KERNEL_WC_GARLIC);
		_displayBuffers[i].userOffset = 0;
		*/
	}

	// Initialization the VideoOut buffer descriptor
	SceVideoOutBufferAttribute videoOutBufferAttribute;
	sceVideoOutSetBufferAttribute(
		&videoOutBufferAttribute,
		SCE_VIDEO_OUT_PIXEL_FORMAT_A8R8G8B8_SRGB,
		SCE_VIDEO_OUT_TILING_MODE_TILE,
		SCE_VIDEO_OUT_ASPECT_RATIO_16_9,
		_displayBuffers[0].renderTarget.getWidth(),
		_displayBuffers[0].renderTarget.getHeight(),
		_displayBuffers[0].renderTarget.getPitch());

	// Register the buffers to the slot: [0..kDisplayBufferCount-1]
	ret = sceVideoOutRegisterBuffers(
		_videoOutHandle,
		0, // Start index
		_surfaceAddresses,
		kDisplayBufferCount,
		&videoOutBufferAttribute);
	if( ret != SCE_OK )
	{
		printf("sceVideoOutRegisterBuffers failed: 0x%08X\n", ret);
		return;
	}

	prepareBackBuffer();
}

void GraphicsSystem::_applyRenderTarget(sce::Gnm::RenderTarget *renderTarget)
{
	Gnmx::GfxContext &gfxc = _displayBuffers[_backBufferIndex].context;
	
	gfxc.setRenderTarget(0, renderTarget);
	gfxc.setDepthRenderTarget(NULL);
	gfxc.setRenderTargetMask(0xF);

	SetViewport(0, 0, renderTarget->getWidth(), renderTarget->getHeight(), 0.0f, 1.0f);

	// Setup some default state.
	sce::Gnm::PrimitiveSetup prim;
	prim.init();
	prim.setCullFace(sce::Gnm::kPrimitiveSetupCullFaceNone);
	gfxc.setPrimitiveSetup(prim);
	sce::Gnm::ClipControl clip;
	clip.init();
	clip.setClipSpace(sce::Gnm::kClipControlClipSpaceDX);
	gfxc.setClipControl(clip);

	//_effectDirty = true;
}

void GraphicsSystem::SetRenderTarget(RenderTarget *renderTarget)
{
	//printf("SettingRenderTarget to 0x%08X\n", renderTarget);

	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];

	sce::Gnm::RenderTarget *rtNativeHandle;

	if (renderTarget)
		rtNativeHandle = renderTarget->_renderTarget;
	else
		rtNativeHandle = &backBuffer->renderTarget;

	_currentRenderTarget = renderTarget;

	_applyRenderTarget(rtNativeHandle);
}

void GraphicsSystem::Clear(float r, float g, float b, float a)
{
	//printf("Clear!\n");

	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	// If we've set a different render target, clear that instead
	// of the back buffer.
	sce::Gnm::RenderTarget *renTar = _currentRenderTarget ? _currentRenderTarget->_renderTarget : &backBuffer->renderTarget;

	// Clear the color and the depth target
	//Toolkit::SurfaceUtil::clearRenderTarget(gfxc, renTar, Vector4(r, g, b, a));
	//Toolkit::SurfaceUtil::clearDepthTarget(gfxc, &backBuffer->depthTarget, 1.f);
}

void GraphicsSystem::SetVertexBuffer(VertexBuffer *buffer)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	gfxc.setVertexBuffers(sce::Gnm::ShaderStage::kShaderStageVs, 0, buffer->_bufferCount, buffer->_buffers);
}

/*
void GraphicsSystem::DrawIndexedSprites(uint32_t vertCount, void *vertexData, uint32_t idxCount, const void *idxData)
{
	//printf("Drawing Indexed Sprites\n");

	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	uint32_t vertexStride = 24;

	// Copy the incoming data to GPU mapped memory. 
	uint8_t* baseVert = (uint8_t*)backBuffer->userData + backBuffer->userOffset;
	{
		uint32_t totalBytes = vertCount * vertexStride;
		memcpy(baseVert, vertexData, totalBytes);
		backBuffer->userOffset += totalBytes;
	}
	uint8_t* baseIndex = (uint8_t*)backBuffer->userData + backBuffer->userOffset;
	{
		uint32_t totalBytes = idxCount * 2;
		memcpy(baseIndex, idxData, totalBytes);
		backBuffer->userOffset += totalBytes;
	}

	if (backBuffer->userOffset > DisplayBuffer::MaxBufferSize)
	{
		printf("BUFFER OVERFLOW!\n");
		abort();
	}

	backBuffer->userBuffer[0].initAsVertexBuffer(baseVert + 0, Gnm::kDataFormatR32G32B32Float, vertexStride, vertCount);
	backBuffer->userBuffer[1].initAsVertexBuffer(baseVert + 12, Gnm::kDataFormatR8G8B8A8Unorm, vertexStride, vertCount);
	backBuffer->userBuffer[2].initAsVertexBuffer(baseVert + 16, Gnm::kDataFormatR32G32Float, vertexStride, vertCount);
	gfxc.setVertexBuffers(sce::Gnm::ShaderStage::kShaderStageVs, 0, 3, backBuffer->userBuffer);

	gfxc.setIndexSize(sce::Gnm::IndexSize::kIndexSize16);
	gfxc.setPrimitiveType(sce::Gnm::PrimitiveType::kPrimitiveTypeTriList);	
	gfxc.drawIndex(idxCount, baseIndex);
}
*/

void GraphicsSystem::Present()
{
	//printf("Present!\n");

	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	// Write the label that indicates that the GPU finished working on this frame
	// and trigger a software interrupt to signal the EOP event queue
	gfxc.writeAtEndOfPipeWithInterrupt(
		Gnm::kEopFlushCbDbCaches,
		Gnm::kEventWriteDestMemory,
		(void*) backBuffer->state,
		Gnm::kEventWriteSource32BitsImmediate,
		kDisplayBufferIdle,
		Gnm::kCacheActionNone,
		Gnm::kCachePolicyLru);
	
	// Submit the command buffers and request a flip of the display buffer
	int ret = gfxc.submitAndFlip(
		_videoOutHandle,
		_backBufferIndex,
		SCE_VIDEO_OUT_FLIP_MODE_VSYNC,
		0);
	if( ret != sce::Gnm::kSubmissionSuccess )
	{
		printf("GfxContext::submitAndFlip failed: 0x%08X\n", ret);
		backBuffer->state[0] = kDisplayBufferIdle;
	}

	// Signal the system that every draw for this frame has been submitted.
	// This function gives permission to the OS to hibernate when all the
	// currently running GPU tasks (graphics and compute) are done.
	ret = Gnm::submitDone();
	if( ret != SCE_OK )
	{
		printf("Gnm::submitDone failed: 0x%08X\n", ret);
	}

	// Update the display chain pointers
	_backBufferIndex = (_backBufferIndex + 1) % kDisplayBufferCount;

	// Prepare the backbuffer for the next frame.
	prepareBackBuffer();
	_frameIndex++;
}

void GraphicsSystem::prepareBackBuffer()
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	// Wait for the EOP event, discarding the first #DisplayBuffers frames.
	// This is important to make sure that the command buffers are not
	// overridden by the CPU while the GPU is still reading them
	if(_frameIndex >= kDisplayBufferCount)
	{
		SceKernelEvent eopEvent;
		int count;
		int ret = sceKernelWaitEqueue(_eopEventQueue, &eopEvent, 1, &count, NULL);
		if( ret != SCE_OK )
		{
			printf("sceKernelWaitEqueue failed: 0x%08X\n", ret);
		}

		// Safety check: make sure that the GPU passed the prepareFlip
		// command for the DCB that we are going to use in this frame
		SceVideoOutFlipStatus flipStatus;
		for(;;)
		{
			ret = sceVideoOutGetFlipStatus(_videoOutHandle, &flipStatus);
			if( ret != SCE_OK )
			{
				printf("sceVideoOutGetFlipStatus failed: 0x%08X\n", ret);
			}

			// gcQueueNum contains the number of queued flips for which the
			// GPU still needs to execute the relative prepareFlip command
			if( flipStatus.gcQueueNum < kDisplayBufferCount )
				break;

			sceKernelUsleep(1);
		}
	}

	// Flag the display buffer as "in use"
	backBuffer->state[0] = kDisplayBufferInUse;

	// Reset the graphical context and initialize the hardware state
	gfxc.reset();
	gfxc.initializeDefaultHardwareState();

	// The waitUntilSafeForRendering stalls the GPU until the scan-out
	// operations on the current display buffer have been completed.
	// This command is not blocking for the CPU.
	gfxc.waitUntilSafeForRendering(_videoOutHandle, _backBufferIndex);

	// Setup the viewport to match the entire screen.
	// The z-scale and z-offset values are used to specify the transformation
	// from clip-space to screen-space

	_applyRenderTarget(&backBuffer->renderTarget);
	
	// Clear the gpu mapped vertex memory.
	//backBuffer->userOffset = 0;

	// We always use the vertex and pixel shader stages.
	gfxc.setActiveShaderStages(Gnm::kActiveShaderStagesVsPs);

	_currentRenderTarget = NULL;
}

sce::Gnm::DataFormat GraphicsSystem::GetFormat(TextureFormat format)
{
	switch (format)
	{
	default:
	case TextureFormat_Color:
		return Gnm::kDataFormatR8G8B8A8Unorm;
	case TextureFormat_Bgr565:
		return Gnm::kDataFormatB5G6R5Unorm;
	case TextureFormat_Bgra5551:
		return Gnm::kDataFormatB5G5R5A1Unorm;
	case TextureFormat_Bgra4444:
		return Gnm::kDataFormatB4G4R4A4Unorm;
	case TextureFormat_Dxt1:
		return Gnm::kDataFormatBc1Unorm;
	case TextureFormat_Dxt3:
		return Gnm::kDataFormatBc2Unorm;
	case TextureFormat_Dxt5:
		return Gnm::kDataFormatBc3Unorm;
	case TextureFormat_NormalizedByte2:
		return Gnm::kDataFormatR8G8Snorm;
	case TextureFormat_NormalizedByte4:
		return Gnm::kDataFormatR8G8B8A8Snorm;
	case TextureFormat_Rgba1010102:
		return Gnm::kDataFormatR10G10B10A2Unorm;
	case TextureFormat_Rg32:
		return Gnm::kDataFormatR16G16Unorm;
	case TextureFormat_Rgba64:
		return Gnm::kDataFormatR16G16B16A16Unorm;
	case TextureFormat_Alpha8:
		return Gnm::kDataFormatA8Unorm;
	case TextureFormat_Single:
		return Gnm::kDataFormatR32Float;
	case TextureFormat_Vector2:
		return Gnm::kDataFormatR32G32Float;
	case TextureFormat_Vector4:
		return Gnm::kDataFormatR32G32B32A32Float;
	case TextureFormat_HalfSingle:
		return Gnm::kDataFormatR16Float;
	case TextureFormat_HalfVector2:
		return Gnm::kDataFormatR16G16Float;
	case TextureFormat_HalfVector4:
		return Gnm::kDataFormatR16G16B16A16Float;
	case TextureFormat_HdrBlendable:
		return Gnm::kDataFormatR16G16B16A16Float;
	};
}

RenderTarget* GraphicsSystem::CreateRenderTarget(TextureFormat format_, uint32_t width, uint32_t height)
{
	sce::Gnm::RenderTarget *renderTarget = new sce::Gnm::RenderTarget();

	Gnm::TileMode tileMode;
	auto format = GetFormat(format_);
	GpuAddress::computeSurfaceTileMode(&tileMode, GpuAddress::kSurfaceTypeColorTarget, format, 1);

	Gnm::SizeAlign renTargetSizeAlign = renderTarget->init(
		width, height, 1,
		format,
		tileMode,
		Gnm::kNumSamples1,
		Gnm::kNumFragments1,
		NULL, NULL);

	void *rtData = Allocator::Get()->allocate(renTargetSizeAlign, SCE_KERNEL_WC_GARLIC);
	renderTarget->setAddresses(rtData, 0, 0);

	return new RenderTarget(renderTarget);
}

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
	/*
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
	*/
}

Texture* GraphicsSystem::CreateTexture(TextureFormat format, uint32_t width, uint32_t height, uint32_t mips)
{
	sce::Gnm::Texture *texture = new sce::Gnm::Texture();

	Gnm::SizeAlign textureSizeAlign = texture->initAs2d(
		width, height, mips,
		GetFormat(format),
		Gnm::kTileModeDisplay_LinearAligned,
		Gnm::kNumSamples1);

	// Allocate the texture data using the alignment returned by initAs2d
	void *textureData = Allocator::Get()->allocate(textureSizeAlign, SCE_KERNEL_WC_GARLIC);
	if( !textureData )
	{
		printf("Cannot allocate the texture data\n");
		delete texture;
		return NULL;
	}

	texture->setBaseAddress(textureData);

	return new Texture(texture);
}

void GraphicsSystem::SetViewport(int left, int top, int width, int height, float minDepth, float maxDepth)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	// TODO: min/max depth needs to be implemented!

	gfxc.setupScreenViewport(left, top, left + width, top + height, 1.0f, 0.0f);
}

/*
Texture* GraphicsSystem::CreateTextureFromPng(unsigned char *data, uint32_t bytes)
{
	ScePngDecParseParam parseParam;
	parseParam.pngMemAddr = data;
	parseParam.pngMemSize = bytes;
	parseParam.reserved0 = 0;

	ScePngDecImageInfo imageInfo;
	int ret = scePngDecParseHeader(&parseParam, &imageInfo);
	if (ret < 0) 
	{
		printf("Error: scePngDecParseHeader(), ret 0x%08x\n", ret);
		return NULL;
	}

	Texture* texture = CreateTexture(imageInfo.imageWidth, imageInfo.imageHeight, 1);
	if (texture == NULL)
		return NULL;

	ScePngDecCreateParam createParam;
	createParam.thisSize = sizeof(createParam);
	createParam.attribute = imageInfo.bitDepth >> 4;
	createParam.maxImageWidth = imageInfo.imageWidth;
	int memorySize = scePngDecQueryMemorySize(&createParam);
	if (memorySize < 0) 
	{
		printf("Error: scePngDecQueryMemorySize(), ret 0x%08x\n", memorySize);
		return NULL;
	}

	// allocate memory for PNG decoder
	// create PNG decoder
	void *decoderBuffer = new char[memorySize];	
	ScePngDecHandle	handle;
	ret = scePngDecCreate(&createParam, decoderBuffer, memorySize, &handle);
	if (ret < 0) 
	{
		printf("Error: scePngDecCreate(), ret 0x%08x\n", ret);
		return NULL;
	}

	//void *image = texture->_texture.
	
	// decode PNG image
	ScePngDecDecodeParam decodeParam;
	decodeParam.pngMemAddr	= data;
	decodeParam.pngMemSize	= bytes;
	decodeParam.imageMemAddr = texture->_texture->getBaseAddress();
	decodeParam.imageMemSize = imageInfo.imageWidth * imageInfo.imageHeight * 4;
	decodeParam.imagePitch	= 0;
	decodeParam.pixelFormat	= SCE_PNG_DEC_PIXEL_FORMAT_R8G8B8A8;
	decodeParam.alphaValue	= 255;
	ret = scePngDecDecode(handle, &decodeParam, NULL);
	if (ret < 0) 
	{
		printf("Error: scePngDecDecode(), ret 0x%08x\n", ret);
		scePngDecDelete(handle);
		return NULL;
	}

	// Cleanup
	scePngDecDelete(handle);
	delete [] decoderBuffer;

	return texture;
}
*/

void GraphicsSystem::_setSamplerState(int slot)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	Gnm::Sampler sampler;
	sampler.init();
	sampler.setLodRange(0, 0);
	sampler.setLodBias(0.0f, 0.0f);
	sampler.setMipFilterMode(Gnm::kMipFilterModePoint);
	sampler.setAnisotropyRatio(Gnm::kAnisotropyRatio4);
	sampler.setWrapMode(Gnm::kWrapModeClampLastTexel, Gnm::kWrapModeClampLastTexel, Gnm::kWrapModeClampLastTexel);
	sampler.setXyFilterMode(Gnm::kFilterModePoint, Gnm::kFilterModePoint);
	gfxc.setSamplers(Gnm::kShaderStagePs, slot, 1, &sampler);
}

void GraphicsSystem::SetTexture(int slot, Texture* texture)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	sce::Gnm::Texture *tex = texture != NULL ? texture->_texture : NULL;
	gfxc.setTextures(Gnm::kShaderStagePs, slot, 1, tex);

	_setSamplerState(slot);
}

void GraphicsSystem::SetTextureRT(int slot, RenderTarget* target)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	if (target == NULL)
	{
		gfxc.setTextures(Gnm::kShaderStagePs, slot, 1, NULL);
		return;
	}

	gfxc.waitForGraphicsWrites(
		target->_renderTarget->getBaseAddress256ByteBlocks(), 
		target->_renderTarget->getSizeInBytes()>>8,
		Gnm::kWaitTargetSlotCb0, Gnm::kCacheActionWriteBackAndInvalidateL1andL2, Gnm::kExtendedCacheActionFlushAndInvalidateCbCache,
		Gnm::kStallCommandBufferParserDisable);

	//_effectDirty = true;
	gfxc.setTextures(Gnm::kShaderStagePs, slot, 1, target->_texture);
	
	_setSamplerState(slot);
}

void GraphicsSystem::SetBlendState(const char* name)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	Gnm::BlendControl blendControl;
	blendControl.init();

	if (strcmp(name, "BlendState.Opaque") == 0)
		blendControl.setBlendEnable(false);

	else if (strcmp(name, "BlendState.AlphaBlend") == 0)
	{
		blendControl.setBlendEnable(true);
		blendControl.setColorEquation(Gnm::kBlendMultiplierOne, Gnm::kBlendFuncAdd, Gnm::kBlendMultiplierOneMinusSrcAlpha);
	}

	else if (strcmp(name, "BlendState.LightingBlend") == 0)
	{
		blendControl.setBlendEnable(true);
		blendControl.setColorEquation(Gnm::kBlendMultiplierSrcAlpha, Gnm::kBlendFuncReverseSubtract, Gnm::kBlendMultiplierOne);
		blendControl.setAlphaEquation(Gnm::kBlendMultiplierZero, Gnm::kBlendFuncReverseSubtract, Gnm::kBlendMultiplierOne);
	}
	
	else if (strcmp(name, "BlendState.GunnStyle") == 0)
	{
		blendControl.setBlendEnable(true);
		blendControl.setColorEquation(Gnm::kBlendMultiplierSrcAlpha, Gnm::kBlendFuncReverseSubtract, Gnm::kBlendMultiplierOne);
		blendControl.setAlphaEquation(Gnm::kBlendMultiplierOne, Gnm::kBlendFuncReverseSubtract, Gnm::kBlendMultiplierOne);
	}

	gfxc.setBlendControl(0, blendControl);
}

void GraphicsSystem::SetVertexShader(VertexShader *shader)
{
	Gnmx::GfxContext &gfxc = _displayBuffers[_backBufferIndex].context;
	gfxc.setVsShader(shader->_shader, shader->_shaderModifier, shader->_fsMem);
}

void GraphicsSystem::SetPixelShader(PixelShader *shader)
{
	Gnmx::GfxContext &gfxc = _displayBuffers[_backBufferIndex].context;
	gfxc.setPsShader(shader->_shader);
}

void GraphicsSystem::SetShaderConstants(ShaderStage stage, void *data, uint32_t sizeInBytes)
{
	Gnmx::GfxContext &gfxc = _displayBuffers[_backBufferIndex].context;

	void *constants = gfxc.allocateFromCommandBuffer(sizeInBytes, Gnm::kEmbeddedDataAlignment4);
	memcpy(constants, data, sizeInBytes);

	Gnm::Buffer constBuffer;
	constBuffer.initAsConstantBuffer(constants, sizeInBytes);
	constBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	gfxc.setConstantBuffers(stage == ShaderStage_Pixel ? Gnm::kShaderStagePs : Gnm::kShaderStageVs, 0, 1, &constBuffer);
}