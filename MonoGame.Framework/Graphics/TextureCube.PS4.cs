// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using System.Runtime.InteropServices;
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
            var dataHandle = GCHandle.Alloc(data, GCHandleType.Pinned);
            var elementSizeInBytes = Marshal.SizeOf(typeof(T));
            var pitchRow = GetPitch(this.size);
            var length = elementSizeInBytes * pitchRow * pitchRow;
            var faceOffset = elementSizeInBytes * (int)cubeMapFace * length;
            var dataPtr = (IntPtr)(dataHandle.AddrOfPinnedObject().ToInt64());

            unsafe
            {
                _texture.GetData(0, (byte*)dataPtr, (uint)faceOffset, (uint)length);
            }

            dataHandle.Free();
        }

        private void PlatformSetData<T>(CubeMapFace face, int level, IntPtr dataPtr, int xOffset, int yOffset, int width, int height)
        {
            var elementSizeInBytes = Marshal.SizeOf(typeof(T));
            var pitchRow = GetPitch(this.size);
            var length = elementSizeInBytes * pitchRow * pitchRow;
            var faceOffset = elementSizeInBytes * (int)face * length;

            unsafe
            {
                _texture.SetData(0, (byte*)dataPtr, (uint)faceOffset, (uint)length);
            }
        }
    }
}
