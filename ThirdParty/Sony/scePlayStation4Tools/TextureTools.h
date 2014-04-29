#pragma once

using namespace System;

namespace sce { namespace PlayStation4 { namespace Tools {

	public ref class TextureTools abstract
	{
	public:
		static array<Byte>^ CompressImage2D(int width, int height, bool fracAlpha, array<Byte>^ data);
	};

}}}