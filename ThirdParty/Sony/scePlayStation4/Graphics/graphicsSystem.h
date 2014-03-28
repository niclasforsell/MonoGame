#pragma once

#include "predecls.h"
#include "graphicsEnums.h"

namespace Graphics {


class __declspec(dllexport) GraphicsSystem
{
private:

	static const uint32_t kDisplayBufferWidth				= 1280;
	static const uint32_t kDisplayBufferHeight				= 720;
	static const uint32_t kDisplayBufferCount				= 2;
	static const bool kHtileEnabled							= true;
	
	const uint32_t kCueRingEntries							= 16;
	const uint32_t kDcbSizeInBytes							= 2 * 1024 * 1024;
	const uint32_t kCcbSizeInBytes							= 2 * 1024 * 1024;

	enum DisplayBufferState
	{
		kDisplayBufferIdle = 0,
		kDisplayBufferInUse
	};

	int _videoOutHandle;

	void *_surfaceAddresses[kDisplayBufferCount];
	DisplayBuffer *_displayBuffers;
	uint32_t _backBufferIndex;
	_SceKernelEqueue *_eopEventQueue;
	uint32_t _frameIndex;
	RenderTarget *_currentRenderTarget;

	void prepareBackBuffer();

	//void _applyEffect(sce::Gnmx::GfxContext &gfxc);

	void _applyRenderTarget(sce::Gnm::RenderTarget *renderTarget);
	void _setSamplerState(int slot);

public:

	GraphicsSystem();
	virtual ~GraphicsSystem();

	virtual void Initialize();

	virtual void SetRenderTarget(RenderTarget *renderTarget);

	virtual void Clear(float r, float g, float b, float a);
	virtual void Present();

    virtual void DrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int startIndex, int primitiveCount);
    virtual void DrawPrimitives(PrimitiveType primitiveType, int vertexStart, int vertexCount);

	virtual void SetTexture(int slot, Texture* texture);
	virtual void SetTextureRT(int slot, RenderTarget* target);

	virtual void SetViewport(int left, int top, int width, int height, float minDepth, float maxDepth);

	virtual void SetBlendState(const char* name);

	virtual void SetVertexBuffer(VertexBuffer *buffer);
	virtual void SetIndexBuffer(IndexBuffer *buffer);

	virtual void SetVertexShader(VertexShader *shader);
	virtual void SetPixelShader(PixelShader *shader);
	virtual void SetShaderConstants(ShaderStage stage, void *data, uint32_t sizeInBytes);
};

} // namespace Graphics