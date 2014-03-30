#pragma once

#include <gnm/dataformats.h>
#include <gnm/constants.h>
#include "graphicsEnums.h"

using namespace sce;

namespace Graphics {

static inline sce::Gnm::DataFormat ToDataFormat(TextureFormat format)
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

static inline sce::Gnm::DataFormat ToSwapchainDataFormat(TextureFormat format)
{
	switch (format)
	{
	default:
	case TextureFormat_Color:
		return Gnm::kDataFormatB8G8R8A8Unorm;
	case TextureFormat_Rgba1010102:
		return Gnm::kDataFormatB10G10R10A2Unorm;
	};
}

static inline sce::Gnm::DataFormat ToDataFormat(DepthFormat format)
{
	switch (format)
	{
	default:
	case DepthFormat_Depth24:
	case DepthFormat_Depth24Stencil8:
		return Gnm::DataFormat::build(Gnm::kZFormat32Float);
	case DepthFormat_Depth16:
		return Gnm::DataFormat::build(Gnm::kZFormat16);
	};
}

static inline sce::Gnm::PrimitiveType ToPrimitiveType(PrimitiveType type)
{
    switch (type)
    {
		default:
        case PrimitiveType_TriangleList:
            return sce::Gnm::PrimitiveType::kPrimitiveTypeTriList;
        case PrimitiveType_TriangleStrip:
            return sce::Gnm::PrimitiveType::kPrimitiveTypeTriStrip;
        case PrimitiveType_LineList:
            return sce::Gnm::PrimitiveType::kPrimitiveTypeLineList;
        case PrimitiveType_LineStrip:
            return sce::Gnm::PrimitiveType::kPrimitiveTypeLineStrip;
    }
}

static inline uint32_t ToPrimitiveElementCount(PrimitiveType primitiveType, uint32_t primitiveCount)
{
    switch (primitiveType)
    {
        case PrimitiveType_TriangleList:
            return primitiveCount * 3;
        case PrimitiveType_TriangleStrip:
            return 3 + (primitiveCount - 1);
        case PrimitiveType_LineList:
            return primitiveCount * 2;
        case PrimitiveType_LineStrip:
            return primitiveCount + 1;
    }
}

static inline Gnm::WrapMode ToWrapMode(TextureAddressMode mode)
{
    switch (mode)
    {
		default:
        case TextureAddressMode_Wrap:
            return Gnm::kWrapModeWrap;
        case TextureAddressMode_Clamp:
            return Gnm::kWrapModeClampLastTexel;
        case TextureAddressMode_Mirror:
            return Gnm::kWrapModeMirror;
    }
}

static inline Gnm::CompareFunc ToCompareFunction(CompareFunction func)
{
    switch (func)
    {
		default:
		case CompareFunction_Never:
			return Gnm::kCompareFuncNever;
		case CompareFunction_Always:
			return Gnm::kCompareFuncAlways;
		case CompareFunction_Less:
			return Gnm::kCompareFuncLess;
		case CompareFunction_LessEqual:
			return Gnm::kCompareFuncLessEqual;
		case CompareFunction_Equal:
			return Gnm::kCompareFuncEqual;
		case CompareFunction_GreaterEqual:
			return Gnm::kCompareFuncGreaterEqual;
		case CompareFunction_Greater:
			return Gnm::kCompareFuncGreater;
		case CompareFunction_NotEqual:
			return Gnm::kCompareFuncNotEqual;
    }
}

} // namespace Graphics