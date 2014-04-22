// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using PS4Texture = Sce.PlayStation4.Graphics.Texture;
using PS4TextureFormat = Sce.PlayStation4.Graphics.TextureFormat;

namespace Microsoft.Xna.Framework.Graphics
{
    public partial class Texture3D
    {
        private void PlatformConstruct(GraphicsDevice graphicsDevice, int width, int height, int depth, bool mipMap,
            SurfaceFormat format, bool renderTarget)
        {
            _texture = PS4Texture.Create3D((PS4TextureFormat)format, width, height, depth, mipMap ? _levelCount : 1);
        }

        private void PlatformSetData(int level, int left, int top, int right, int bottom, int front, int back,
            IntPtr dataPtr, int width, int height, int depth)
        {
            throw new NotImplementedException();
        }

        private void PlatformGetData<T>(int level, int left, int top, int right, int bottom, int front, int back,
            T[] data, int startIndex, int elementCount) where T : struct
        {
            throw new NotImplementedException();
        }
    }
}
