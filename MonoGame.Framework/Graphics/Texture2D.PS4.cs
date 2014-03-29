// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using System.IO;
using System.Runtime.InteropServices;
using Sce.PlayStation4.Graphics;

namespace Microsoft.Xna.Framework.Graphics
{
    public partial class Texture2D
    {
        internal Sce.PlayStation4.Graphics.Texture _texture;

        private void PlatformConstruct(int width, int height, bool mipmap, SurfaceFormat format, SurfaceType type, bool shared)
        {
            if (type != SurfaceType.Texture)
                return;

            _texture = new Sce.PlayStation4.Graphics.Texture((TextureFormat)format, width, height, (mipmap ? _levelCount : 0));
        }

        private void PlatformSetData<T>(int level, Rectangle? rect, T[] data, int startIndex, int elementCount)
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

        private void PlatformGetData<T>(int level, Rectangle? rect, T[] data, int startIndex, int elementCount)
        {
            throw new NotImplementedException();
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
