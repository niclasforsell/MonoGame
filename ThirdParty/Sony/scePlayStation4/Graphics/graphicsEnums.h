#pragma once

namespace Graphics {

enum VertexElement
{
	// NOTE: This should match up with the MonoGame 
	// VertexElementFormat enum for easy interop!

	VertexElement_Single,
    VertexElement_Vector2,
    VertexElement_Vector3,
    VertexElement_Vector4,
    VertexElement_Color,
    VertexElement_Byte4,
    VertexElement_Short2,
    VertexElement_Short4,
    VertexElement_NormalizedShort2,
    VertexElement_NormalizedShort4,
    VertexElement_HalfVector2,
    VertexElement_HalfVector4
};

enum TextureFormat
{
	// NOTE: This should match up with the
	// MonoGame enum for easy interop!

    TextureFormat_Color = 0,
    TextureFormat_Bgr565 = 1,
    TextureFormat_Bgra5551 = 2,
    TextureFormat_Bgra4444 = 3,
    TextureFormat_Dxt1 = 4,
    TextureFormat_Dxt3 = 5,
    TextureFormat_Dxt5 = 6,
    TextureFormat_NormalizedByte2 = 7,
    TextureFormat_NormalizedByte4 = 8,
    TextureFormat_Rgba1010102 = 9,
    TextureFormat_Rg32 = 10,
    TextureFormat_Rgba64 = 11,
    TextureFormat_Alpha8 = 12,
    TextureFormat_Single = 13,
    TextureFormat_Vector2 = 14,
    TextureFormat_Vector4 = 15,
    TextureFormat_HalfSingle = 16,
    TextureFormat_HalfVector2 = 17,
    TextureFormat_HalfVector4 = 18,
    TextureFormat_HdrBlendable = 19,
};

enum DepthFormat
{
	// NOTE: This should match up with the MonoGame 
	// DepthFormat enum for easy interop!

	DepthFormat_None = -1,
	DepthFormat_Depth16 = 54,
	DepthFormat_Depth24 = 51,
	DepthFormat_Depth24Stencil8 = 48,		
};

enum IndexElement
{
	// NOTE: This should match up with the MonoGame 
	// IndexElementSize enum for easy interop!

	IndexElement_SixteenBits,
	IndexElement_ThirtyTwoBits
};

enum ShaderStage
{
	ShaderStage_Vertex,
	ShaderStage_Pixel
};

enum PrimitiveType
{
	// NOTE: This should match up with the MonoGame 
	// PrimitiveType enum for easy interop!

	PrimitiveType_TriangleList,
	PrimitiveType_TriangleStrip,
	PrimitiveType_LineList,
	PrimitiveType_LineStrip,
};


enum ClearOptions
{
	ClearOptions_Target = 1,
    ClearOptions_DepthBuffer = 2,
    ClearOptions_Stencil = 4        
};

} // namespace Graphics