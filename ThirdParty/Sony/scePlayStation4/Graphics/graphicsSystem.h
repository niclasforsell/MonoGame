#pragma once

#include "predecls.h"
#include "graphicsEnums.h"

namespace Graphics {


class __declspec(dllexport) GraphicsSystem
{
private:

	static const uint32_t kDisplayBufferCount				= 2;
	static const bool kHtileEnabled							= true;
	
	const uint32_t kCueRingEntries							= 16;
	const uint32_t kDcbSizeInBytes							= 4 * 1024 * 1024;
	const uint32_t kCcbSizeInBytes							= 4 * 1024 * 1024;

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

	PixelShader *_clearPS;
	VertexShader *_clearVS;

	void prepareBackBuffer();

	void _applyRenderTarget(sce::Gnm::RenderTarget *renderTarget, sce::Gnm::DepthRenderTarget *depthTarget);

	void _discardBuffer(uint8_t *&buffer, uint32_t &actualSize, uint32_t requiredSize);

	void _applyBuffers(DisplayBuffer *backBuffer, int baseVertex);

public:

	GraphicsSystem();
	~GraphicsSystem();

	void Initialize(int backbufferWidth, int backbufferHeight, TextureFormat backbufferFormat, DepthFormat depthFormat);

	void SetRenderTarget(RenderTarget *renderTarget);

	void Clear(ClearOptions options, float r, float g, float b, float a, float depth, int stencil);
	void Present();

    void DrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int startIndex, int primitiveCount);
    void DrawPrimitives(PrimitiveType primitiveType, int vertexStart, int vertexCount);

	void SetTexture(int slot, Texture* texture);
	void SetTextureRT(int slot, RenderTarget* target);
	void SetSamplerState(int slot, uint32_t desc0, uint32_t desc1, uint32_t desc2, uint32_t desc3);

	void SetViewport(int left, int top, int width, int height, float minDepth, float maxDepth);

	void SetVertexBuffer(VertexBuffer *buffer);
	void SetIndexBuffer(IndexBuffer *buffer);

	void CS_IGNORE _discardBuffer(VertexBuffer *buffer);
	void CS_IGNORE _discardBuffer(IndexBuffer *buffer);

	void SetVertexShader(VertexShader *shader);
	void SetPixelShader(PixelShader *shader);
	void SetShaderConstants(ShaderStage stage, void *data, uint32_t sizeInBytes);

	static void CreateSamplerState(	TextureFilter filter, 
									TextureAddressMode addressU, 
									TextureAddressMode addressV, 
									TextureAddressMode addressW,
									int maxAnisotropy,
									int maxMipLevel,
									float mipMapLevelOfDetailBias,
									CS_OUT uint32_t &desc0,
									CS_OUT uint32_t &desc1,
									CS_OUT uint32_t &desc2,
									CS_OUT uint32_t &desc3);

	static void CreateRasterizerState(	CullMode cullMode, 
										FillMode fillMode,
										bool multiSampleAntiAlias,
										bool scissorTestEnable,
										CS_OUT uint32_t &prim0,
										CS_OUT uint32_t &flag1);

	void SetRasterizerState(uint32_t prim0, uint32_t desc1, float depthBias, float slopeScaleDepthBias);

	static void CreateDepthStencilState(	bool depthBufferEnable,
											bool depthBufferWriteEnable,
											CompareFunction depthBufferFunction,
											CS_OUT uint32_t &depth0);

	void SetDepthStencilState(uint32_t depth0);


	static void CreateBlendState(	Blend colorSourceBlend,
									Blend colorDestBlend,
									BlendFunction colorBlendFunc,
									Blend alphaSourceBlend,
									Blend alphaDestBlend,
									BlendFunction alphaBlendFunc,
									CS_OUT uint32_t &blend);

	void SetBlendState(uint32_t blend0, uint32_t blend1, uint32_t blend2, uint32_t blend3, uint32_t colorWrites);

};

} // namespace Graphics