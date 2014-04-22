// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using System.IO;
using System.Runtime.InteropServices;
using PS4Texture = Sce.PlayStation4.Graphics.Texture;
using PS4TextureFormat = Sce.PlayStation4.Graphics.TextureFormat;

namespace Microsoft.Xna.Framework.Graphics
{
    public partial class Texture2D
    {
        protected void PlatformConstruct(int width, int height, bool mipmap, SurfaceFormat format, SurfaceType type, bool shared)
        {
            /*
            if (type != SurfaceType.Texture)
                return;
             */

            _texture = PS4Texture.Create2D((PS4TextureFormat)format, width, height, (mipmap ? _levelCount : 1));
        }

        protected void PlatformSetData<T>(int level, Rectangle? rect, T[] data, int startIndex, int elementCount)
        {
            var dataHandle = GCHandle.Alloc(data, GCHandleType.Pinned);

            var elementSizeInByte = Marshal.SizeOf(typeof(T));
            var startBytes = startIndex * elementSizeInByte;
            var dataPtr = (IntPtr)(dataHandle.AddrOfPinnedObject().ToInt64() + startBytes);

            if (rect.HasValue)
            {
                throw new NotImplementedException();
            }
            else
            {
                unsafe
                {
                    _texture.SetData((uint)level, (byte*)dataPtr, (uint)(elementSizeInByte * elementCount));
                }
            }

            dataHandle.Free();
        }

        protected void PlatformGetData<T>(int level, Rectangle? rect, T[] data, int startIndex, int elementCount)
        {
            var dataHandle = GCHandle.Alloc(data, GCHandleType.Pinned);

            var elementSizeInByte = Marshal.SizeOf(typeof(T));
            var startBytes = startIndex * elementSizeInByte;
            var dataPtr = (IntPtr)(dataHandle.AddrOfPinnedObject().ToInt64() + startBytes);

            if (rect.HasValue)
            {
                throw new NotImplementedException();
            }
            else
            {
                unsafe
                {
                    _texture.GetData((uint)level, (byte*)dataPtr, (uint)(elementSizeInByte * elementCount));
                }
            }

            dataHandle.Free();
        }

        private static Texture2D PlatformFromStream(GraphicsDevice graphicsDevice, Stream stream)
        {
            throw new NotImplementedException();
        }

        private void PlatformSaveAsJpeg(Stream stream, int width, int height)
        {
            throw new NotImplementedException();
        }

        private void PlatformSaveAsPng(Stream stream, int width, int height)
        {
            throw new NotImplementedException();
        }

        private void PlatformReload(Stream textureStream)
        {
            throw new NotImplementedException();
        }
    }
}
