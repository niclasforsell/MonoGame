#pragma once

namespace Graphics {

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

} // namespace Graphics