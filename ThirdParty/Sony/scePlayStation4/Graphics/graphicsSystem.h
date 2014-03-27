#pragma once

#include "predecls.h"

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

	static sce::Gnm::DataFormat GetFormat(TextureFormat format);

public:

	GraphicsSystem();
	virtual ~GraphicsSystem();

	virtual void Initialize();

	virtual RenderTarget* CreateRenderTarget(TextureFormat format, uint32_t width, uint32_t height);
	virtual void GetRenderTargetData(RenderTarget *target, unsigned char *data, uint32_t bytes);
	
	virtual Texture* CreateTexture(TextureFormat format, uint32_t width, uint32_t height, uint32_t mips);
	//virtual Texture* CreateTextureFromPng(unsigned char *data, uint32_t bytes);

	virtual void SetRenderTarget(RenderTarget *renderTarget);

	virtual void Clear(float r, float g, float b, float a);
	virtual void Present();

	//virtual void DrawIndexedSprites(uint32_t vertCount, void *vertexData, uint32_t idxCount, const void *indexData);

	virtual void SetTexture(int slot, Texture* texture);
	virtual void SetTextureRT(int slot, RenderTarget* target);

	virtual void SetViewport(int left, int top, int width, int height, float minDepth, float maxDepth);

	virtual void SetBlendState(const char* name);

	virtual void SetVertexShader(VertexShader *shader);
	virtual void SetPixelShader(PixelShader *shader);

	//virtual Effect* CreateEffect(const char *name);
	//virtual void ApplyEffect(Effect *effect);
};

} // namespace Graphics