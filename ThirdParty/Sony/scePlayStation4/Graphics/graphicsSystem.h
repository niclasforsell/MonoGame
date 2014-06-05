#pragma once

#include "predecls.h"
#include "graphicsEnums.h"

namespace Graphics {


class CS_API GraphicsSystem
{
private:

	static const uint32_t kDisplayBufferCount				= 2;
	static const bool kHtileEnabled							= true;
	
	const uint32_t kCueRingEntries							= 16;
	const uint32_t kDcbSizeInBytes							= 8 * 1024 * 1024;
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
	sce::Gnm::RenderTarget *_currentRenderTarget;

	PixelShader *_clearPS;
	VertexShader *_clearVS;
	FetchShader *_clearFS;

	PixelShader *_videoPS;
	VertexShader *_videoVS;
	FetchShader *_videoFS;

	void prepareBackBuffer();

	void _applyRenderTargets(sce::Gnm::RenderTarget *render0,
							sce::Gnm::RenderTarget *render1,
							sce::Gnm::RenderTarget *render2,
							sce::Gnm::RenderTarget *render3,
							sce::Gnm::DepthRenderTarget *depthTarget);

	void _discardBuffer(uint8_t *&buffer, uint32_t &actualSize, uint32_t requiredSize);

	void _applyBuffers(DisplayBuffer *backBuffer, int baseVertex);



public:

	GraphicsSystem();
	~GraphicsSystem();

	void Initialize(int backbufferWidth, int backbufferHeight, TextureFormat backbufferFormat, DepthFormat depthFormat);

	void SetRenderTargets(RenderTarget *target0, RenderTarget *target1, RenderTarget *target2, RenderTarget *target3);

	void Clear(ClearOptions options, float r, float g, float b, float a, float depth, int stencil);
	void Present();

    void DrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int startIndex, int primitiveCount);
    void DrawPrimitives(PrimitiveType primitiveType, int vertexStart, int vertexCount);

	void SetTexture(int slot, Texture* texture);
	void SetTextureRT(int slot, RenderTarget* target);
	void SetSamplerState(int slot, uint32_t desc0, uint32_t desc1, uint32_t desc2, uint32_t desc3);

	void SetViewport(int left, int top, int width, int height, float minDepth, float maxDepth);
	void SetScissorRectangle(int left, int top, int right, int bottom);

	void SetVertexBuffer(VertexBuffer *buffer);
	void SetIndexBuffer(IndexBuffer *buffer);

	void CS_IGNORE _discardBuffer(VertexBuffer *buffer);
	void CS_IGNORE _discardBuffer(IndexBuffer *buffer);

	void SetVertexShader(VertexShader *shader, FetchShader *fetch);
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
											CS_OUT uint32_t &depth0,
											CS_OUT uint32_t &stencil0,
											CS_OUT uint32_t &stencil1);

	void SetDepthStencilState(uint32_t depth0, uint32_t stencil0, uint32_t stencil1);


	static void CreateBlendState(	Blend colorSourceBlend,
									Blend colorDestBlend,
									BlendFunction colorBlendFunc,
									Blend alphaSourceBlend,
									Blend alphaDestBlend,
									BlendFunction alphaBlendFunc,
									CS_OUT uint32_t &blend);

	void SetBlendState(uint32_t blend0, uint32_t blend1, uint32_t blend2, uint32_t blend3, uint32_t colorWrites);

	CS_IGNORE void DrawYCbCr(	sce::Gnm::Texture* luma,
								sce::Gnm::Texture* chroma,
								float left,
								float right,
								float top,
								float bottom);
};

} // namespace Graphics