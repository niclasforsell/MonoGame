#include "stdafx.h"
#include "TextureTools.h"
#include <texture_tool/image.h>
#include <texture_tool/filter.h>
#include <texture_tool/mipped_image.h>

namespace sce { namespace PlayStation4 { namespace Tools {

using namespace sce::TextureTool;

int Max(int a, int b)
{
	return a > b ? a : b;
}

array<Byte>^ TextureTools::CompressImage2D(int width, int height, int mips, int level, bool fracAlpha, array<Byte>^ levelData)
{
	auto dataFormat = fracAlpha ? Gnm::kDataFormatBc3Unorm : Gnm::kDataFormatBc1Unorm;
	auto tileMode = Gnm::kTileModeThin_1dThin;
	uint32_t padFlags = mips > 1 ? GnmTextureGen::kPadToPow2 : 0;

	// We figure out the exact surface size needed at runtime by creating
	// the texture description then getting the current mip level size.
	uint64_t surfaceSize;
	{
		sce::Gnm::Texture texture;
		texture.initAs2d(width, height, mips, dataFormat, tileMode, Gnm::kNumFragments1);
		GpuAddress::computeTextureSurfaceOffsetAndSize(nullptr, &surfaceSize, &texture, level, 0);
	}

	// Figure out the size for the current mip level.
	width = Max(1, width >> level);
	height = Max(1, height >> level);

	// Fill the image with the incoming mip level data.
	Image image(width, height, 4, Image::Plane::kTypeNormal);
	pin_ptr<uint8_t> dataHandle = &levelData[levelData->GetLowerBound(0)];
	auto offset = 0;
	for (auto y = 0; y < height; y++)
	{
		for (auto x = 0; x < width; x++)
		{
			auto r = levelData[offset + 0];
			auto g = levelData[offset + 1];
			auto b = levelData[offset + 2];
			auto a = levelData[offset + 3];

			image.setPixel(x, y, Image::Channel::kChannelRed,   r / 255.0f);
			image.setPixel(x, y, Image::Channel::kChannelGreen, g / 255.0f);
			image.setPixel(x, y, Image::Channel::kChannelBlue,  b / 255.0f);
			image.setPixel(x, y, Image::Channel::kChannelAlpha, a / 255.0f);

			offset += 4;
		}
	}

	// Get the compressed, tiled, and padded surface.
	auto surface = gcnew array<Byte>(surfaceSize);
	pin_ptr<uint8_t> pinned = &surface[surface->GetLowerBound(0)];
	image.fillSurface(pinned, surfaceSize, tileMode, dataFormat, 0, padFlags);

	return surface;
}

}}}