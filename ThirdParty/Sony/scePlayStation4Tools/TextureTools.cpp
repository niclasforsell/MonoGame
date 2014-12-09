#include "stdafx.h"
#include "TextureTools.h"
#include <texture_tool/image.h>
#include <texture_tool/filter.h>
#include <texture_tool/mipped_image.h>

namespace sce { namespace PlayStation4 { namespace Tools {

using namespace sce::TextureTool;

array<Byte>^ TextureTools::CompressImage2D(int width, int height, bool fracAlpha, bool generateMipmaps, array<Byte>^ data)
{
	Image image(width, height, 4, Image::Plane::kTypeNormal);

	auto dataFormat = fracAlpha ? Gnm::kDataFormatBc3Unorm : Gnm::kDataFormatBc1Unorm;
	auto tileMode = Gnm::kTileModeThin_1dThin;
	Gnm::Texture texture;

	pin_ptr<uint8_t> dataHandle = &data[data->GetLowerBound(0)];
	auto offset = 0;
	for (auto y = 0; y < height; y++)
	{
		for (auto x = 0; x < width; x++)
		{
			auto r = data[offset + 0];
			auto g = data[offset + 1];
			auto b = data[offset + 2];
			auto a = data[offset + 3];

			image.setPixel(x, y, Image::Channel::kChannelRed,   r / 255.0f);
			image.setPixel(x, y, Image::Channel::kChannelGreen, g / 255.0f);
			image.setPixel(x, y, Image::Channel::kChannelBlue,  b / 255.0f);
			image.setPixel(x, y, Image::Channel::kChannelAlpha, a / 255.0f);

			offset += 4;
		}
	}

	// Copy to an output image and optionally generate high quality mipmaps
	MippedImage outputImage(image, sce::TextureTool::Filters::LanczosFilter(), generateMipmaps ? -1 : 1);
	auto sizeAlign = outputImage.initTexture(texture, dataFormat, tileMode);

	auto output = gcnew array<Byte>(sizeAlign.m_size);
	pin_ptr<uint8_t> outHandle = &output[output->GetLowerBound(0)];
	image.fillSurface(outHandle, sizeAlign.m_size, tileMode, dataFormat);

	return output;
}

}}}