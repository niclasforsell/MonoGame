// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using System.Runtime.InteropServices;
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

        private void PlatformSetData<T>(int level, int left, int top, int right, int bottom, int front, int back,
            T[] data, int startIndex, int elementCount, int width, int height, int depth)
        {
            var elementSizeInBytes = Marshal.SizeOf(typeof(T));
            var startBytes = elementSizeInBytes * startIndex;
            var dataHandle = GCHandle.Alloc(data, GCHandleType.Pinned);
            var dataPtr = (IntPtr)(dataHandle.AddrOfPinnedObject().ToInt64() + startBytes);

            var pitchRow = GetPitch(_width);
            var pitchSlice = pitchRow * _height;
            var sliceOffset = pitchSlice * front + pitchRow * top + left;

            unsafe
            {
                _texture.SetData((uint)level, (byte*)dataPtr, (uint)sliceOffset, (uint)(elementSizeInBytes * elementCount));
            }

            dataHandle.Free();
        }

        private void PlatformGetData<T>(int level, int left, int top, int right, int bottom, int front, int back,
            T[] data, int startIndex, int elementCount) where T : struct
        {
            var elementSizeInBytes = Marshal.SizeOf(typeof(T));
            var startBytes = elementSizeInBytes * startIndex;
            var dataHandle = GCHandle.Alloc(data, GCHandleType.Pinned);
            var dataPtr = (IntPtr)(dataHandle.AddrOfPinnedObject().ToInt64() + startBytes);

            var pitchRow = GetPitch(_width);
            var pitchSlice = pitchRow * _height;
            var sliceOffset = pitchSlice * front + pitchRow * top + left;

            unsafe
            {
                _texture.GetData((uint)level, (byte*)dataPtr, (uint)sliceOffset, (uint)(elementSizeInBytes * elementCount));
            }

            dataHandle.Free();
        }
    }
}
