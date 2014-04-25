// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using PS4Texture = Sce.PlayStation4.Graphics.Texture;
using PS4TextureFormat = Sce.PlayStation4.Graphics.TextureFormat;

namespace Microsoft.Xna.Framework.Graphics
{
    public partial class TextureCube
    {
        private void PlatformConstruct(GraphicsDevice graphicsDevice, int size, bool mipMap,
            SurfaceFormat format, bool renderTarget)
        {
            _texture = PS4Texture.CreateCube((PS4TextureFormat)format, size, size, mipMap ? _levelCount : 1);
        }

        private void PlatformGetData<T>(CubeMapFace cubeMapFace, T[] data)
            where T : struct
        {
            throw new NotImplementedException();
        }

        private void PlatformSetData(CubeMapFace face, int level, IntPtr dataPtr,
            int xOffset, int yOffset, int width, int height)
        {
            throw new NotImplementedException();
        }
    }
}
