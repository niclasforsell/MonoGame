
#include "graphicsSystem.h"

#include "texture.h"
#include "vertexShader.h"
#include "fetchShader.h"
#include "pixelShader.h"
#include "renderTarget.h"
#include "displayBuffer.h"
#include "vertexBuffer.h"
#include "indexBuffer.h"
#include "graphicsHelpers.h"

#include "../allocator.h"

#include "defaultShaders.h"
#include "videoShaders.h"

#include <stdio.h>
#include <stdlib.h>
#include <kernel.h>

#include <video_out.h>
#include <gnm.h>
#include <gnmx.h>
#include <assert.h>
#include <scebase.h>


using namespace sce;
using namespace sce::Gnmx;
using namespace Graphics;


GraphicsSystem::GraphicsSystem()
{
	_backBufferIndex = 0;
	memset(_surfaceAddresses, 0, sizeof(_surfaceAddresses));

	_videoOutHandle = 0;
	_frameIndex = 0;
	_nullTexture = NULL;

	_clearPS = new PixelShader(clear_p);
	_clearVS = new VertexShader(clear_vv);
	_clearFS = new FetchShader(_clearVS, NULL, 0);

	_videoPS = new PixelShader(video_p_pssl_sb);
	_videoVS = new VertexShader(video_vv_pssl_sb);
}

GraphicsSystem::~GraphicsSystem()
{
}

void GraphicsSystem::Initialize(int backbufferWidth, int backbufferHeight, TextureFormat backbufferFormat, DepthFormat depthFormat_)
{
	//printf("Initialize!\n");

	// Open the video output port
	_videoOutHandle = sceVideoOutOpen(SCE_USER_SERVICE_USER_ID_SYSTEM, SCE_VIDEO_OUT_BUS_TYPE_MAIN, 0, NULL);
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
	_displayBuffers = mem::alloc_array<DisplayBuffer>(kDisplayBufferCount);

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
		_displayBuffers[i].cueHeap = mem::allocShared(
			Gnmx::ConstantUpdateEngine::computeHeapSize(kCueRingEntries),
			Gnm::kAlignmentOfBufferInBytes);

		if( !_displayBuffers[i].cueHeap )
		{
			printf("Cannot allocate the CUE heap memory\n");
			return;
		}

		// Allocate the draw command buffer
		_displayBuffers[i].dcbBuffer = mem::alloc(
			kDcbSizeInBytes,
			Gnm::kAlignmentOfBufferInBytes);

		if( !_displayBuffers[i].dcbBuffer )
		{
			printf("Cannot allocate the draw command buffer memory\n");
			return; // SCE_KERNEL_ERROR_ENOMEM;
		}

		// Allocate the constants command buffer
		_displayBuffers[i].ccbBuffer = mem::alloc(
			kCcbSizeInBytes,
			Gnm::kAlignmentOfBufferInBytes);

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

		bool tileSurfaceFailed;

#if SCE_ORBIS_SDK_VERSION >= 0x01700081u // SDK Version 1.7

		tileSurfaceFailed = GpuAddress::computeSurfaceTileMode(&tileMode, GpuAddress::kSurfaceTypeColorTargetDisplayable, format, 1) != SCE_OK;
#else
		tileSurfaceFailed = !GpuAddress::computeSurfaceTileMode(&tileMode, GpuAddress::kSurfaceTypeColorTargetDisplayable, format, 1);

#endif

		if( tileSurfaceFailed )
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
		_surfaceAddresses[i] = mem::allocShared(sizeAlign);
		if( !_surfaceAddresses[i] )
		{
			printf("Cannot allocate the render target memory\n");
			return; // SCE_KERNEL_ERROR_ENOMEM;
		}
		_displayBuffers[i].renderTarget.setAddresses(_surfaceAddresses[i], 0, 0);

		// Compute the tiling mode for the depth buffer		
		_displayBuffers[i].hasDepthTarget = depthFormat_ != DepthFormat_None;
		if (_displayBuffers[i].hasDepthTarget)
		{
			auto depthFormat = ToDataFormat(depthFormat_);
			Gnm::TileMode depthTileMode;

#if SCE_ORBIS_SDK_VERSION >= 0x01700081u // SDK Version 1.7

			tileSurfaceFailed = GpuAddress::computeSurfaceTileMode(&depthTileMode, GpuAddress::kSurfaceTypeDepthOnlyTarget, depthFormat, 1) != SCE_OK;
#else
			tileSurfaceFailed = !GpuAddress::computeSurfaceTileMode(&depthTileMode, GpuAddress::kSurfaceTypeDepthOnlyTarget, depthFormat, 1);

#endif

			if( tileSurfaceFailed )
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
				void *htileMemory = mem::allocShared(htileSizeAlign);
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
				stencilMemory = mem::allocShared(stencilSizeAlign);
				if( !stencilMemory )
				{
					printf("Cannot allocate the stencil buffer\n");
					return; // SCE_KERNEL_ERROR_ENOMEM;
				}
			}

			// Allocate the depth buffer
			void *depthMemory = mem::allocShared(depthTargetSizeAlign);
			if( !depthMemory )
			{
				printf("Cannot allocate the depth buffer\n");
				return; // SCE_KERNEL_ERROR_ENOMEM;
			}
			_displayBuffers[i].depthTarget.setAddresses(depthMemory, stencilMemory);
		}

		_displayBuffers[i].state = mem::alloc<volatile uint32_t>(4, 8);
		if( !_displayBuffers[i].state )
		{
			printf("Cannot allocate a GPU label\n");
			return; // SCE_KERNEL_ERROR_ENOMEM;
		}

		_displayBuffers[i].state[0] = kDisplayBufferIdle;

		// Initialize the free and discard buffer counts.
		_displayBuffers[i].freeBufferCount = 0;
		_displayBuffers[i].discardBufferCount = 0;
		
		_displayBuffers[i].currentVB = NULL;
		_displayBuffers[i].currentVBDirty = false;
		_displayBuffers[i].currentVBOffset = 0;

		_displayBuffers[i].currentIB = NULL;
		_displayBuffers[i].currentIBDirty = false;
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

	// Prepare the null texture.
	{
		_nullTexture = new sce::Gnm::Texture();
		auto textureSizeAlign = _nullTexture->initAs2d(
			2, 2, 1,
			Gnm::kDataFormatR8G8B8A8Unorm,
			Gnm::kTileModeDisplay_LinearAligned,
#if SCE_ORBIS_SDK_VERSION >= 0x02000071u // SDK Version 2.0
			Gnm::kNumFragments1);
#else
			Gnm::kNumSamples1);
#endif

		void *textureData = mem::allocShared(textureSizeAlign);
		memset(textureData, 0, textureSizeAlign.m_size);
		_nullTexture->setBaseAddress(textureData);
	}

	prepareBackBuffer();
}

void GraphicsSystem::_applyRenderTargets(sce::Gnm::RenderTarget *render0,
										sce::Gnm::RenderTarget *render1,
										sce::Gnm::RenderTarget *render2,
										sce::Gnm::RenderTarget *render3,
										sce::Gnm::DepthRenderTarget *depthTarget)
{
	Gnmx::GfxContext &gfxc = _displayBuffers[_backBufferIndex].context;
	
	_currentRenderTarget = render0;
	gfxc.setRenderTarget(0, render0);
	gfxc.setRenderTarget(1, render1);
	gfxc.setRenderTarget(2, render2);
	gfxc.setRenderTarget(3, render3);
	gfxc.setDepthRenderTarget(depthTarget);

	SetViewport(0, 0, render0->getWidth(), render0->getHeight(), 0.0f, 1.0f);

	// Do this now that the depth target changed and won't have to later.
	if (depthTarget == NULL)
		gfxc.setPolygonOffsetZFormat(Gnm::kZFormatInvalid);
	else
		gfxc.setPolygonOffsetZFormat(depthTarget->getZFormat());

	// Setup some default state.
	sce::Gnm::ClipControl clip;
	clip.init();
	clip.setClipSpace(sce::Gnm::kClipControlClipSpaceDX);
	gfxc.setClipControl(clip);
}

void GraphicsSystem::SetRenderTargets(RenderTarget *target0, RenderTarget *target1, RenderTarget *target2, RenderTarget *target3)
{
	//printf("SettingRenderTarget to 0x%08X\n", renderTarget);

	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];

	sce::Gnm::RenderTarget *renderTarget;
	sce::Gnm::DepthRenderTarget *depthTarget;

	if (target0)
	{
		renderTarget = target0->_renderTarget;
		depthTarget = target0->_depthTarget;
	}
	else
	{
		renderTarget = &backBuffer->renderTarget;
		depthTarget = backBuffer->hasDepthTarget ? &backBuffer->depthTarget : NULL;
	}

	_applyRenderTargets( renderTarget,
						target1 != NULL ? target1->_renderTarget : NULL,
						target2 != NULL ? target2->_renderTarget : NULL,
						target3 != NULL ? target3->_renderTarget : NULL,
						depthTarget);
}

void GraphicsSystem::Clear(ClearOptions options, float r, float g, float b, float a, float depth, int stencil)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	// Setup the clear shader.
	SetVertexShader(_clearVS, _clearFS);
	SetPixelShader(_clearPS);
	float color[4] = {r, g, b, a};
	SetShaderConstants(ShaderStage_Pixel, &color, sizeof(color));

	// Are we clearing the color target?
	if (options & ClearOptions_Target)
		gfxc.setRenderTargetMask(0x0000FFFF);
	else
		gfxc.setRenderTargetMask(0x00000000);

	// We should clear the entire target.
	gfxc.setupScreenViewport(0, 0,	_currentRenderTarget->getWidth(), 
									_currentRenderTarget->getHeight(), 1.0f, 0.0f);

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
	sce::Gnm::StencilOpControl stencilOpControl;
	stencilOpControl.init();
	if (clearStencil)
	{
		stencilControl.m_testVal = 0xFF;
		stencilControl.m_mask = 0xFF;
		stencilControl.m_writeMask = 0xFF;
		stencilControl.m_opVal = 0xFF;
		stencilOpControl.setStencilOps(Gnm::StencilOp::kStencilOpReplaceOp, Gnm::StencilOp::kStencilOpReplaceOp, Gnm::StencilOp::kStencilOpReplaceOp);
		stencilOpControl.setStencilOpsBack(Gnm::StencilOp::kStencilOpReplaceOp, Gnm::StencilOp::kStencilOpReplaceOp, Gnm::StencilOp::kStencilOpReplaceOp);
	}
	gfxc.setStencil(stencilControl);
	gfxc.setStencilOpControl(stencilOpControl);

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
	gfxc.setScanModeControl(Gnm::kScanModeControlAaDisable, Gnm::kScanModeControlViewportScissorDisable);

	// We do the draw using a rect primitive and a vertex
	// shader that generates the position from ids.
	gfxc.setPrimitiveType(sce::Gnm::PrimitiveType::kPrimitiveTypeRectList);	
	gfxc.drawIndexAuto(3);
}

void GraphicsSystem::SetVertexBuffer(VertexBuffer *buffer)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	backBuffer->currentVB = buffer;
	backBuffer->currentVBDirty = true;
}

void GraphicsSystem::SetIndexBuffer(IndexBuffer *buffer)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	backBuffer->currentIB = buffer;
	backBuffer->currentIBDirty = true;
}

void GraphicsSystem::_discardBuffer(VertexBuffer *buffer)
{
	_discardBuffer(buffer->_bufferData, buffer->_actualSize, buffer->_requiredSize);

	// Reset the base address.
	auto offset = 0;
	for (auto i=0; i < buffer->_bufferCount; i++)
	{
		auto format = buffer->_buffers[i].getDataFormat();
		buffer->_buffers[i].setBaseAddress(buffer->_bufferData + offset);
		offset += format.getBytesPerElement();
	}

	// TODO: Should we clear the buffer to zeros?  Maybe we
	// should clear it to 0xd34db33f in debug modes to ensure
	// the error is very apparent?
	//memset(buffer->_bufferData, 0xd34db33f, buffer->_actualSize);

	// If this is the current VB then update the command buffer.
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	backBuffer->currentVBDirty |= backBuffer->currentVB == buffer;
}

void GraphicsSystem::_discardBuffer(IndexBuffer *buffer)
{
	_discardBuffer(buffer->_bufferData, buffer->_actualSize, buffer->_requiredSize);

	// TODO: Should we clear the buffer to zeros?  Maybe we
	// should clear it to 0xd34db33f in debug modes to ensure
	// the error is very apparent?
	//memset(buffer->_bufferData, 0xd34db33f, buffer->_actualSize);

	// If this is the current IB then update the command buffer
	// to point at the new index data... the size and count should
	// both still be valid.
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	backBuffer->currentIBDirty |= backBuffer->currentIB == buffer;
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
			actualSize = requiredSize;

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
		buffer = mem::allocShared<uint8_t>(requiredSize, Gnm::kAlignmentOfBufferInBytes);
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

void GraphicsSystem::_applyBuffers(DisplayBuffer *backBuffer, int baseVertex)
{
	auto &gfxc = backBuffer->context;

	if (backBuffer->currentVBDirty || backBuffer->currentVBOffset != baseVertex)
	{
		auto currentVB = backBuffer->currentVB;
		auto bufferCount = currentVB->_bufferCount;
		auto buffers = currentVB->_buffers;
		auto bufferData = currentVB->_bufferData + (baseVertex * buffers[0].getStride());

		auto offset = 0;
		for (auto i=0; i < bufferCount; i++)
		{
			auto format = buffers[i].getDataFormat();
			buffers[i].setBaseAddress(bufferData + offset);
			offset += format.getBytesPerElement();
		}

		gfxc.setVertexBuffers(sce::Gnm::ShaderStage::kShaderStageVs, 0, bufferCount, buffers);

		backBuffer->currentVBOffset = baseVertex;
		backBuffer->currentVBDirty = false;
	}

	if (backBuffer->currentIBDirty)
	{
		auto currentIB = backBuffer->currentIB;

		gfxc.setIndexSize((Gnm::IndexSize)currentIB->_indexSize);
		gfxc.setIndexCount(currentIB->_indexCount);
		gfxc.setIndexBuffer(currentIB->_bufferData);

		backBuffer->currentIBDirty = false;
	}
}



void GraphicsSystem::DrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int startIndex, int primitiveCount)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	auto &gfxc = backBuffer->context;

	_applyBuffers(backBuffer, baseVertex);

	gfxc.setPrimitiveType(ToPrimitiveType(primitiveType));	
	auto indexCount = ToPrimitiveElementCount(primitiveType, primitiveCount);
	gfxc.drawIndexOffset(startIndex, indexCount);	
}

void GraphicsSystem::DrawPrimitives(PrimitiveType primitiveType, int vertexStart, int vertexCount)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	_applyBuffers(backBuffer, vertexStart);

	gfxc.setPrimitiveType(ToPrimitiveType(primitiveType));	
	gfxc.drawIndexAuto(vertexCount);	
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

	// Make sure we are using DirectX style pixel centering.
	gfxc.setVertexQuantization(	sce::Gnm::kVertexQuantizationMode16_8, 
								sce::Gnm::kVertexQuantizationRoundModeRoundToEven, 
								sce::Gnm::kVertexQuantizationCenterAtZero);

	// The waitUntilSafeForRendering stalls the GPU until the scan-out
	// operations on the current display buffer have been completed.
	// This command is not blocking for the CPU.
	gfxc.waitUntilSafeForRendering(_videoOutHandle, _backBufferIndex);

	// Setup the viewport to match the entire screen.
	// The z-scale and z-offset values are used to specify the transformation
	// from clip-space to screen-space

	_applyRenderTargets(	&backBuffer->renderTarget, NULL, NULL, NULL,
						backBuffer->hasDepthTarget ? &backBuffer->depthTarget : NULL);
	
	// Move all the previously discarded buffers to 
	// the end of the free buffer list.
	assert(backBuffer->freeBufferCount + backBuffer->discardBufferCount < MAX_DISCARD_BUFFERS);
	memcpy(	backBuffer->freeBuffers + backBuffer->freeBufferCount, 
			backBuffer->discardBuffers, 
			sizeof(BufferInfo) * backBuffer->discardBufferCount);
	backBuffer->freeBufferCount += backBuffer->discardBufferCount;
	backBuffer->discardBufferCount = 0;

	// Clear the current VB/IB.
	backBuffer->currentVB = NULL;
	backBuffer->currentVBOffset = 0;
	backBuffer->currentVBDirty = false;
	backBuffer->currentIB = NULL;
	backBuffer->currentIBDirty = false;

	// We always use the vertex and pixel shader stages.
	gfxc.setActiveShaderStages(Gnm::kActiveShaderStagesVsPs);
}

void GraphicsSystem::SetViewport(int left, int top, int width, int height, float minDepth, float maxDepth)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	// TODO: min/max depth needs to be implemented!

	gfxc.setupScreenViewport(left, top, left + width, top + height, 1.0f, 0.0f);
}

void GraphicsSystem::SetScissorRectangle(int left, int top, int right, int bottom)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	gfxc.setViewportScissor(0, left, top, right, bottom, Gnm::WindowOffsetMode::kWindowOffsetDisable);
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

	auto fixedMaxMip = Gnmx::convertF32ToU4_8((float)maxMipLevel);
	sampler.setLodRange(fixedMaxMip, 0xFFF);

	auto a = Gnmx::convertF32ToS6_8((float)mipMapLevelOfDetailBias);
	auto b = Gnmx::convertF32ToS2_4((float)mipMapLevelOfDetailBias);
	sampler.setLodBias(a, b);

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

	if (texture == NULL)
	{
		gfxc.setTextures(Gnm::kShaderStagePs, slot, 1, _nullTexture);
		return;
	}

	if (texture->_isTarget)
	{
		auto target = (RenderTarget*)texture;

		gfxc.waitForGraphicsWrites(
			target->_renderTarget->getBaseAddress256ByteBlocks(), 
#if SCE_ORBIS_SDK_VERSION >= 0x02000131u // SDK Version 2.0
			target->_renderTarget->getSliceSizeInBytes() >> 8,
#else
			target->_renderTarget->getSizeInBytes() >> 8,
#endif
			Gnm::kWaitTargetSlotCb0, Gnm::kCacheActionWriteBackAndInvalidateL1andL2, Gnm::kExtendedCacheActionFlushAndInvalidateCbCache,
			Gnm::kStallCommandBufferParserDisable);
	}

	gfxc.setTextures(Gnm::kShaderStagePs, slot, 1, texture->_texture);
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
												StencilOperation ccwStencilDepthFail,
												StencilOperation ccwStencilFail,
												CompareFunction ccwStencilFunction,
												StencilOperation ccwStencilPass,
												CompareFunction depthBufferFunction,
												uint8_t referenceStencil,
												StencilOperation stencilDepthBufferFail,
												bool stencilEnable,
												StencilOperation stencilFail,
												CompareFunction stencilFunction,
												uint8_t stencilMask,
												StencilOperation stencilPass,
												uint8_t stencilWriteMask,
												bool twoSidedStencilMode,
												uint32_t &depth0,
												uint32_t &stencil0,
												uint32_t &stencil1)
{
	Gnm::DepthStencilControl depthControl;
	depthControl.init();
	depthControl.setDepthEnable(depthBufferEnable);
	depthControl.setDepthControl(	depthBufferWriteEnable ? 
									Gnm::kDepthControlZWriteEnable : Gnm::kDepthControlZWriteDisable, 
									ToCompareFunction(depthBufferFunction));

	depthControl.setStencilEnable(stencilEnable);
	depthControl.setStencilFunction(ToCompareFunction(stencilFunction));
	depthControl.setStencilFunctionBack(ToCompareFunction(ccwStencilFunction));

	depth0 = depthControl.m_reg;

	Gnm::StencilControl stencilControl;
	stencilControl.init();
	stencilControl.m_testVal = referenceStencil;
	stencilControl.m_mask = stencilMask;
	stencilControl.m_writeMask = stencilWriteMask;
	stencilControl.m_opVal = referenceStencil;
	stencil0 =	stencilControl.m_testVal |
				stencilControl.m_mask       << 8 |
				stencilControl.m_writeMask  << 16 |
				stencilControl.m_opVal      << 24;

	Gnm::StencilOpControl stencilOpControl;
	stencilOpControl.init();
	stencilOpControl.setStencilOps(ToStencilOp(stencilFail), ToStencilOp(stencilPass), ToStencilOp(stencilDepthBufferFail));
	if (twoSidedStencilMode)
	{
		stencilOpControl.setStencilOpsBack(ToStencilOp(ccwStencilFail), ToStencilOp(ccwStencilPass), ToStencilOp(ccwStencilDepthFail));
	}
	stencil1 = stencilOpControl.m_reg;
}

void GraphicsSystem::SetDepthStencilState(uint32_t depth0, uint32_t stencil0, uint32_t stencil1)
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

	Gnm::StencilControl stencilControl;
	stencilControl.m_testVal =   stencil0 & 0xff;
	stencilControl.m_mask =      (stencil0 >> 8) & 0xff;
	stencilControl.m_writeMask = (stencil0 >> 16) & 0xff;
	stencilControl.m_opVal =     (stencil0 >> 24) & 0xff;
	gfxc.setStencil(stencilControl);

	Gnm::StencilOpControl stencilOpControl;
	stencilOpControl.m_reg = stencil1;
	gfxc.setStencilOpControl(stencilOpControl);
}

void GraphicsSystem::CreateBlendState(	Blend colorSourceBlend,
										Blend colorDestBlend,
										BlendFunction colorBlendFunc,
										Blend alphaSourceBlend,
										Blend alphaDestBlend,
										BlendFunction alphaBlendFunc,
										CS_OUT uint32_t &blend)
{
	Gnm::BlendControl blendControl;
	blendControl.init();

	auto blendEnabled = !(	colorSourceBlend == Blend_One && 
							colorDestBlend == Blend_Zero &&
							alphaSourceBlend == Blend_One && 
							alphaDestBlend == Blend_Zero);

	blendControl.setBlendEnable(blendEnabled);

	blendControl.setColorEquation(	ToBlendMultiplier(colorSourceBlend, false), 
									ToBlendFunc(colorBlendFunc), 
									ToBlendMultiplier(colorDestBlend, false));

	blendControl.setAlphaEquation(	ToBlendMultiplier(alphaSourceBlend, true), 
									ToBlendFunc(alphaBlendFunc), 
									ToBlendMultiplier(alphaDestBlend, true));

	// If the color and alpha blend values are different 
	// then we're using a seperate alpha blend.
	blendControl.setSeparateAlphaEnable(	colorSourceBlend != alphaSourceBlend || 
											colorDestBlend != alphaDestBlend || 
											colorBlendFunc != alphaBlendFunc);

	blend = blendControl.m_reg;
}

void GraphicsSystem::SetBlendState(uint32_t blend0, uint32_t blend1, uint32_t blend2, uint32_t blend3, uint32_t colorWrites)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	Gnm::BlendControl blendControl;
	blendControl.m_reg = blend0;
	gfxc.setBlendControl(0, blendControl);
	blendControl.m_reg = blend1;
	gfxc.setBlendControl(1, blendControl);
	blendControl.m_reg = blend2;
	gfxc.setBlendControl(2, blendControl);
	blendControl.m_reg = blend3;
	gfxc.setBlendControl(3, blendControl);

	gfxc.setRenderTargetMask(colorWrites);
}

void GraphicsSystem::SetVertexShader(VertexShader *shader, FetchShader *fetch)
{
	Gnmx::GfxContext &gfxc = _displayBuffers[_backBufferIndex].context;
	gfxc.setVsShader(shader->_shader, fetch->_shaderModifier, fetch->_fsMem);
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

void GraphicsSystem::DrawYCbCr(Gnm::Texture* luma,
										Gnm::Texture* chroma,
										float left,
										float right,
										float top,
										float bottom)
{
	DisplayBuffer *backBuffer = &_displayBuffers[_backBufferIndex];
	Gnmx::GfxContext &gfxc = backBuffer->context;

	Gnm::Sampler sampler;
	sampler.init();
	sampler.setMipFilterMode(Gnm::kMipFilterModeLinear);
	sampler.setXyFilterMode(Gnm::kFilterModeBilinear, Gnm::kFilterModeBilinear);

	auto renderWidth = _currentRenderTarget->getWidth();
	auto renderHeight = _currentRenderTarget->getHeight();

	gfxc.setupScreenViewport(0, 0, renderWidth, renderHeight, 1.0f, 0.0f);
	gfxc.setClipRectangle(0, 0, 0, renderWidth, renderHeight);

	void *fetchShaderAddr = NULL;
	gfxc.setVsShader(_videoVS->_shader, 0, fetchShaderAddr);
	gfxc.setPsShader(_videoPS->_shader);

	gfxc.setTextures(Gnm::kShaderStagePs, 0, 1, luma);
	gfxc.setTextures(Gnm::kShaderStagePs, 1, 1, chroma);
	gfxc.setSamplers(Gnm::kShaderStagePs, 0, 1, &sampler);
	gfxc.setSamplers(Gnm::kShaderStagePs, 1, 1, &sampler);

	auto cb = static_cast<VideoShaderConstants*>(gfxc.allocateFromCommandBuffer(sizeof(VideoShaderConstants), Gnm::kEmbeddedDataAlignment4));
	if (cb)
	{
		cb->pos_left = -1.0f;
		cb->pos_right = 1.0f;
		cb->pos_top = 1.0f;
		cb->pos_bottom = -1.0f;

		cb->tex_left = left;
		cb->tex_right = right;
		cb->tex_top = top;
		cb->tex_bottom = bottom;

		Gnm::Buffer constBuffer;
		constBuffer.initAsConstantBuffer(cb, sizeof(VideoShaderConstants));
		gfxc.setConstantBuffers(Gnm::kShaderStageVs, 0, 1, &constBuffer);
	}

	gfxc.setPrimitiveType(Gnm::kPrimitiveTypeTriStrip);
	gfxc.drawIndexAuto(4);
}

void GraphicsSystem::SetMarker(const char* markerName, uint32_t rgbColor)
{
	Gnmx::GfxContext &gfxc = _displayBuffers[_backBufferIndex].context;
	gfxc.setMarker(markerName, rgbColor);
}

void GraphicsSystem::PushMarker(const char* markerName, uint32_t rgbColor)
{
	Gnmx::GfxContext &gfxc = _displayBuffers[_backBufferIndex].context;
	gfxc.pushMarker(markerName, rgbColor);
}

void GraphicsSystem::PopMarker()
{
	Gnmx::GfxContext &gfxc = _displayBuffers[_backBufferIndex].context;
	gfxc.popMarker();
}
