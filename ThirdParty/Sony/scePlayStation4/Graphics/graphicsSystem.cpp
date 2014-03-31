
#include "graphicsSystem.h"

#include "texture.h"
#include "vertexShader.h"
#include "pixelShader.h"
#include "renderTarget.h"
#include "displayBuffer.h"
#include "vertexBuffer.h"
#include "indexBuffer.h"
#include "graphicsHelpers.h"

#include "../allocator.h"

#include "defaultShaders.h"

#include <stdio.h>
#include <stdlib.h>
#include <kernel.h>

#include <video_out.h>
#include <gnm.h>
#include <gnmx.h>


using namespace sce;
using namespace sce::Gnmx;
using namespace Graphics;


GraphicsSystem::GraphicsSystem()
{
	_backBufferIndex = 0;
	memset(_surfaceAddresses, 0, sizeof(_surfaceAddresses));

	_videoOutHandle = 0;
	_frameIndex = 0;

	_clearPS = new PixelShader(clear_p);
	_clearVS = new VertexShader(clear_vv);
}

GraphicsSystem::~GraphicsSystem()
{
}

void GraphicsSystem::Initialize(int backbufferWidth, int backbufferHeight, TextureFormat backbufferFormat, DepthFormat depthFormat_)
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

	auto kStencilFormat = depthFormat_ == DepthFormat_Depth24Stencil8 ? Gnm::kStencil8 : Gnm::kStencilInvalid;
	_displayBuffers = (DisplayBuffer*)Allocator::Get()->allocate(sizeof(DisplayBuffer) * kDisplayBufferCount);

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
		auto format = ToSwapchainDataFormat(backbufferFormat);
		if( !GpuAddress::computeSurfaceTileMode(&tileMode,
			GpuAddress::kSurfaceTypeColorTargetDisplayable, format, 1) )
		{
			printf("Cannot compute the tile mode for the render target surface\n");
			return; // SCE_KERNEL_ERROR_UNKNOWN;
		}

		// Initialize the render target descriptor
		Gnm::SizeAlign sizeAlign = _displayBuffers[i].renderTarget.init(
			backbufferWidth,
			backbufferHeight,
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
		if (_displayBuffers[i].hasDepthTarget = depthFormat_ != DepthFormat_None)
		{
			auto depthFormat = ToDataFormat(depthFormat_);
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
				backbufferWidth,
				backbufferHeight,
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
		}

		_displayBuffers[i].state = (volatile uint32_t*) Allocator::Get()->allocate(4, 8, SCE_KERNEL_WB_ONION);
		if( !_displayBuffers[i].state )
		{
			printf("Cannot allocate a GPU label\n");
			return; // SCE_KERNEL_ERROR_ENOMEM;
		}

		_displayBuffers[i].state[0] = kDisplayBufferIdle;

		// Initialize the free and discard buffer counts.
		_displayBuffers[i].freeBufferCount = 0;
		_displayBuffers[i].discardBufferCount = 0;
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

void GraphicsSystem::_applyRenderTarget(sce::Gnm::RenderTarget *renderTarget, sce::Gnm::DepthRenderTarget *depthTarget)
{
	Gnmx::GfxContext &gfxc = _displayBuffers[_backBufferIndex].context;
	
	gfxc.setRenderTarget(0, renderTarget);
	gfxc.setDepthRenderTarget(depthTarget);
	gfxc.setRenderTargetMask(0xF);

	SetViewport(0, 0, renderTarget->getWidth(), renderTarget->getHeight(), 0.0f, 1.0f);

	// Do this now that the depth target changed and won't have to later.
	if (depthTarget != NULL)
		gfxc.setPolygonOffsetZFormat(Gnm::kZFormatInvalid);
	else
		gfxc.setPolygonOffsetZFormat(depthTarget->getZFormat());

	// Setup some default state.
	sce::Gnm::ClipControl clip;
	clip.init();
	clip.setClipSpace(sce::Gnm::kClipControlClipSpaceDX);
	gfxc.setClipControl(clip);
}

void GraphicsSystem::SetRenderTarget(RenderTarget *renderTarget_)
{
	//printf("SettingRenderTarget to 0x%08X\n", renderTarget);

	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];

	sce::Gnm::RenderTarget *renderTarget;
	sce::Gnm::DepthRenderTarget *depthTarget;

	if (renderTarget_)
	{
		renderTarget = renderTarget_->_renderTarget;
		depthTarget = renderTarget_->_depthTarget;
	}
	else
	{
		renderTarget = &backBuffer->renderTarget;
		depthTarget = backBuffer->hasDepthTarget ? &backBuffer->depthTarget : NULL;
	}

	_currentRenderTarget = renderTarget_;

	_applyRenderTarget(renderTarget, depthTarget);
}

void GraphicsSystem::Clear(ClearOptions options, float r, float g, float b, float a, float depth, int stencil)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	// Setup the clear shader.
	SetVertexShader(_clearVS);
	SetPixelShader(_clearPS);
	float color[4] = {r, g, b, a};
	SetShaderConstants(ShaderStage_Pixel, &color, sizeof(color));

	// Are we clearing the color target?
	if (options & ClearOptions_Target)
		gfxc.setRenderTargetMask(0xFFFFFFFF);
	else
		gfxc.setRenderTargetMask(0x0000);

	// What else are we clearing?
	auto clearDepth = (options & ClearOptions_DepthBuffer) == ClearOptions_DepthBuffer;
	auto clearStencil = (options & ClearOptions_Stencil) == ClearOptions_Stencil;

	Gnm::DbRenderControl dbRenderControl;
	dbRenderControl.init();
	dbRenderControl.setDepthClearEnable(clearDepth);
	dbRenderControl.setStencilClearEnable(clearStencil);
	gfxc.setDbRenderControl(dbRenderControl);

	sce::Gnm::DepthStencilControl depthControl;
	depthControl.init();
	depthControl.setDepthEnable(clearDepth);
	depthControl.setStencilEnable(clearStencil);
	depthControl.setStencilFunction(Gnm::kCompareFuncAlways);
	if (clearDepth)
		depthControl.setDepthControl(Gnm::kDepthControlZWriteEnable, Gnm::kCompareFuncAlways);
	else
		depthControl.setDepthControl(Gnm::kDepthControlZWriteDisable, Gnm::kCompareFuncAlways);
	gfxc.setDepthStencilControl(depthControl);

	// Are we clearing stencil?
	sce::Gnm::StencilControl stencilControl;
	stencilControl.init();
	if (clearStencil)
	{
		stencilControl.m_testVal = 0xFF;
		stencilControl.m_mask = 0xFF;
		stencilControl.m_writeMask = 0xFF;
		stencilControl.m_opVal = 0xFF;
	}
	gfxc.setStencil(stencilControl);

	gfxc.setStencilClearValue(stencil);
	gfxc.setDepthClearValue(depth);

	// Clobber some more states.
	sce::Gnm::PrimitiveSetup prim;
	prim.init();
	prim.setCullFace(sce::Gnm::kPrimitiveSetupCullFaceNone);
	gfxc.setPrimitiveSetup(prim);
	Gnm::BlendControl blendControl;
	blendControl.init();
	gfxc.setBlendControl(0, blendControl);

	// We do the draw using a rect primitive and a vertex
	// shader that generates the position from ids.
	gfxc.setPrimitiveType(sce::Gnm::PrimitiveType::kPrimitiveTypeRectList);	
	gfxc.drawIndexAuto(3);
}

void GraphicsSystem::SetVertexBuffer(VertexBuffer *buffer)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	gfxc.setVertexBuffers(sce::Gnm::ShaderStage::kShaderStageVs, 0, buffer->_bufferCount, buffer->_buffers);
}

void GraphicsSystem::SetIndexBuffer(IndexBuffer *buffer)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	gfxc.setIndexSize((Gnm::IndexSize)buffer->_indexSize);
	gfxc.setIndexCount(buffer->_indexCount);
	gfxc.setIndexBuffer(buffer->_indexData);
}

void GraphicsSystem::_discardBuffer(uint8_t *&buffer, uint32_t &actualSize, uint32_t requiredSize)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];

	// If we were passed an existing buffer then store it
	// until the start of the next frame.
	if (buffer != NULL)
	{
		assert(backBuffer->discardBufferCount + 1 < MAX_DISCARD_BUFFERS);
		auto discardInfo = backBuffer->discardBuffers + backBuffer->discardBufferCount;
		discardInfo->buffer = buffer;
		discardInfo->bufferSize = actualSize;
		backBuffer->discardBufferCount++;
	}

	// Search for the best fit free buffer.
	BufferInfo *bestBuffer = NULL;
	auto current = backBuffer->freeBuffers;
	auto end = backBuffer->freeBuffers + backBuffer->freeBufferCount;
	for (; current != end; current++)
	{
		// If we have a match return it immediately.
		if (current->bufferSize == requiredSize)
		{
			buffer = current->buffer;

			// Fill the hole with the last entry.
			if (--backBuffer->freeBufferCount > 0)	
				*current = *(end-1);
			return;
		}

		// Is it big enough at all?
		if (requiredSize > current->bufferSize)
			continue;

		// Is it better than the last best fit?
		if (bestBuffer == NULL || current->bufferSize < bestBuffer->bufferSize)
			bestBuffer = current;
	}

	// If we didn't find a new buffer then we 
	// have to allocate a new one.
	if (bestBuffer == NULL)
	{
		buffer = (uint8_t*)Allocator::Get()->allocate(requiredSize, Gnm::kAlignmentOfBufferInBytes, SCE_KERNEL_WC_GARLIC);
		actualSize = requiredSize;
		return;
	}

	// Reture out best one.
	buffer = bestBuffer->buffer;
	actualSize = bestBuffer->bufferSize;

	// Fill the hole with the last entry.
	if (--backBuffer->freeBufferCount > 0)	
		*bestBuffer = *(end-1);
}

void GraphicsSystem::DrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int startIndex, int primitiveCount)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	gfxc.setPrimitiveType(ToPrimitiveType(primitiveType));	
	auto indexCount = ToPrimitiveElementCount(primitiveType, primitiveCount);
	gfxc.drawIndexOffset(startIndex, indexCount, baseVertex, 0);	
}

void GraphicsSystem::DrawPrimitives(PrimitiveType primitiveType, int vertexStart, int vertexCount)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	gfxc.setPrimitiveType(ToPrimitiveType(primitiveType));	
	gfxc.drawIndexAuto(vertexCount, vertexStart, 0);	
}

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

	_applyRenderTarget(	&backBuffer->renderTarget, 
						backBuffer->hasDepthTarget ? &backBuffer->depthTarget : NULL);
	
	// Move all the previously discarded buffers to 
	// the end of the free buffer list.
	assert(backBuffer->freeBufferCount + backBuffer->discardBufferCount < MAX_DISCARD_BUFFERS);
	memcpy(	backBuffer->freeBuffers + backBuffer->freeBufferCount, 
			backBuffer->discardBuffers, 
			sizeof(BufferInfo) * backBuffer->discardBufferCount);
	backBuffer->freeBufferCount += backBuffer->discardBufferCount;
	backBuffer->discardBufferCount = 0;

	// We always use the vertex and pixel shader stages.
	gfxc.setActiveShaderStages(Gnm::kActiveShaderStagesVsPs);

	_currentRenderTarget = NULL;
}

void GraphicsSystem::SetViewport(int left, int top, int width, int height, float minDepth, float maxDepth)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	// TODO: min/max depth needs to be implemented!

	gfxc.setupScreenViewport(left, top, left + width, top + height, 1.0f, 0.0f);
}

void GraphicsSystem::SetSamplerState(int slot, uint32_t desc0, uint32_t desc1, uint32_t desc2, uint32_t desc3)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	Gnm::Sampler sampler;
	sampler.m_regs[0] = desc0;
	sampler.m_regs[1] = desc1;
	sampler.m_regs[2] = desc2;
	sampler.m_regs[3] = desc3;

	gfxc.setSamplers(Gnm::kShaderStagePs, slot, 1, &sampler);
}

void GraphicsSystem::CreateSamplerState(	TextureFilter filter, 
											TextureAddressMode addressU, 
											TextureAddressMode addressV, 
											TextureAddressMode addressW,
											int maxAnisotropy,
											int maxMipLevel,
											float mipMapLevelOfDetailBias,
											uint32_t &desc0,
											uint32_t &desc1,
											uint32_t &desc2,
											uint32_t &desc3)
{
	Gnm::Sampler sampler;
	sampler.init();

	sampler.setWrapMode(ToWrapMode(addressU), ToWrapMode(addressV), ToWrapMode(addressW));

	switch (filter)
    {
		default:
		case TextureFilter_Linear:
			sampler.setXyFilterMode(Gnm::kFilterModeBilinear, Gnm::kFilterModeBilinear);
			sampler.setMipFilterMode(Gnm::kMipFilterModeLinear);
			break;
		case TextureFilter_Point:
			sampler.setXyFilterMode(Gnm::kFilterModePoint, Gnm::kFilterModePoint);
			sampler.setMipFilterMode(Gnm::kMipFilterModePoint);
			break;
		case TextureFilter_Anisotropic:
			sampler.setXyFilterMode(Gnm::kFilterModeAnisoBilinear, Gnm::kFilterModeAnisoBilinear);
			sampler.setMipFilterMode(Gnm::kMipFilterModeLinear);
			break;
		case TextureFilter_LinearMipPoint:
			sampler.setXyFilterMode(Gnm::kFilterModeBilinear, Gnm::kFilterModeBilinear);
			sampler.setMipFilterMode(Gnm::kMipFilterModePoint);
			break;
		case TextureFilter_PointMipLinear:
			sampler.setXyFilterMode(Gnm::kFilterModePoint, Gnm::kFilterModePoint);
			sampler.setMipFilterMode(Gnm::kMipFilterModeLinear);
			break;
		case TextureFilter_MinLinearMagPointMipLinear:
			sampler.setXyFilterMode(Gnm::kFilterModePoint, Gnm::kFilterModeBilinear);
			sampler.setMipFilterMode(Gnm::kMipFilterModeLinear);
			break;
		case TextureFilter_MinLinearMagPointMipPoint:
			sampler.setXyFilterMode(Gnm::kFilterModePoint, Gnm::kFilterModeBilinear);
			sampler.setMipFilterMode(Gnm::kMipFilterModePoint);
			break;
		case TextureFilter_MinPointMagLinearMipLinear:
			sampler.setXyFilterMode(Gnm::kFilterModeBilinear, Gnm::kFilterModePoint);
			sampler.setMipFilterMode(Gnm::kMipFilterModeLinear);
			break;
		case TextureFilter_MinPointMagLinearMipPoint:
			sampler.setXyFilterMode(Gnm::kFilterModeBilinear, Gnm::kFilterModePoint);
			sampler.setMipFilterMode(Gnm::kMipFilterModePoint);
			break;
    }

	sampler.setLodRange(0, maxMipLevel);
	sampler.setLodBias(mipMapLevelOfDetailBias, 0.0f);

	if (maxAnisotropy >= 16)
		sampler.setAnisotropyRatio(Gnm::kAnisotropyRatio16);
	else if (maxAnisotropy >= 8)
		sampler.setAnisotropyRatio(Gnm::kAnisotropyRatio8);
	else if (maxAnisotropy >= 4)
		sampler.setAnisotropyRatio(Gnm::kAnisotropyRatio4);
	else if (maxAnisotropy >= 2)
		sampler.setAnisotropyRatio(Gnm::kAnisotropyRatio2);
	else 
		sampler.setAnisotropyRatio(Gnm::kAnisotropyRatio1);

	desc0 = sampler.m_regs[0];
	desc1 = sampler.m_regs[1];
	desc2 = sampler.m_regs[2];
	desc3 = sampler.m_regs[3];
}

void GraphicsSystem::SetTexture(int slot, Texture* texture)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	sce::Gnm::Texture *tex = texture != NULL ? texture->_texture : NULL;
	gfxc.setTextures(Gnm::kShaderStagePs, slot, 1, tex);
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

	// TODO: Why were we doing this?
	//_effectDirty = true;

	gfxc.setTextures(Gnm::kShaderStagePs, slot, 1, target->_texture);
}

void GraphicsSystem::CreateRasterizerState(	CullMode cullMode,
											FillMode fillMode,
											bool multiSampleAntiAlias,
											bool scissorTestEnable,
											uint32_t &prim0,
											uint32_t &flag1)
{
	Gnm::PrimitiveSetup prim;
	prim.init();
	prim.setFrontFace(Gnm::kPrimitiveSetupFrontFaceCw);

	switch (cullMode)
	{
		case CullMode_CullClockwiseFace:
			prim.setCullFace(Gnm::kPrimitiveSetupCullFaceFront);
			break;

		default:
		case CullMode_CullCounterClockwiseFace:
			prim.setCullFace(Gnm::kPrimitiveSetupCullFaceBack);
			break;

		case CullMode_None:
			prim.setCullFace(Gnm::kPrimitiveSetupCullFaceNone);
			break;
	};

	switch (fillMode)
	{
		default:
		case FillMode_Solid:
			prim.setPolygonMode(Gnm::kPrimitiveSetupPolygonModeFill, Gnm::kPrimitiveSetupPolygonModeFill);
			break;

		case FillMode_WireFrame:
			prim.setPolygonMode(Gnm::kPrimitiveSetupPolygonModeLine, Gnm::kPrimitiveSetupPolygonModeLine);
			break;
	};	

	prim0 = prim.m_reg;
	flag1 = (scissorTestEnable ? 1<<0 : 0) | (multiSampleAntiAlias ? 1<<1 : 0);
}

void GraphicsSystem::SetRasterizerState(uint32_t prim0, uint32_t flag1, float depthBias, float slopeScaleDepthBias)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	gfxc.setScanModeControl((flag1 & (1<<1)) ? Gnm::kScanModeControlAaEnable : Gnm::kScanModeControlAaDisable,
							(flag1 & (1<<0)) ? Gnm::kScanModeControlViewportScissorEnable : Gnm::kScanModeControlViewportScissorDisable);
	gfxc.setPolygonOffsetFront(slopeScaleDepthBias, depthBias);

	Gnm::PrimitiveSetup prim;
	prim.m_reg = prim0;
	gfxc.setPrimitiveSetup(prim);
}

void GraphicsSystem::CreateDepthStencilState(	bool depthBufferEnable,
												bool depthBufferWriteEnable,
												CompareFunction depthBufferFunction,
												uint32_t &depth0)
{
	Gnm::DepthStencilControl depthControl;
	depthControl.init();
	depthControl.setDepthEnable(depthBufferEnable);
	depthControl.setDepthControl(	depthBufferWriteEnable ? 
									Gnm::kDepthControlZWriteEnable : Gnm::kDepthControlZWriteDisable, 
									ToCompareFunction(depthBufferFunction));

	depth0 = depthControl.m_reg;
}

void GraphicsSystem::SetDepthStencilState(uint32_t depth0)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	// TODO: If we kept track of this we could decide
	// to skip changing it saving a context roll.
	Gnm::DbRenderControl dbRenderControl;
	dbRenderControl.init();
	gfxc.setDbRenderControl(dbRenderControl);

	Gnm::DepthStencilControl depthControl;
	depthControl.m_reg = depth0;
	gfxc.setDepthStencilControl(depthControl);

	// TODO: Add stencil support!
	sce::Gnm::StencilControl stencilControl;
	stencilControl.init();
	gfxc.setStencil(stencilControl);
}

void GraphicsSystem::SetBlendState(uint32_t blend0)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	Gnm::BlendControl blendControl;
	blendControl.init();

	/*
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
	*/

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