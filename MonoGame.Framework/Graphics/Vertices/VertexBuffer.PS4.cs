// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using System.Runtime.InteropServices;
using Sce.PlayStation4.Graphics;


namespace Microsoft.Xna.Framework.Graphics
{
    public partial class VertexBuffer
    {
        internal Sce.PlayStation4.Graphics.VertexBuffer _buffer;

        private void PlatformConstruct()
        {
            //_buffer = new Sce.PlayStation4.Graphics.VertexBuffer((uint)VertexDeclaration.VertexStride, (uint)VertexCount);
        }

        private void PlatformGraphicsDeviceResetting()
        {
            throw new NotSupportedException();
        }

        private void PlatformGetData<T>(int offsetInBytes, T[] data, int startIndex, int elementCount, int vertexStride) where T : struct
        {
            throw new NotImplementedException();
        }

        private void PlatformSetDataInternal<T>(int offsetInBytes, T[] data, int startIndex, int elementCount, int vertexStride, SetDataOptions options, int bufferSize, int elementSizeInBytes) where T : struct
        {
            var startBytes = startIndex * elementSizeInBytes;
            var dataBytes = elementCount * elementSizeInBytes;            
            var dataHandle = GCHandle.Alloc(data, GCHandleType.Pinned);
            var dataPtr = (IntPtr)(dataHandle.AddrOfPinnedObject().ToInt64() + startBytes);

            unsafe
            {
                _buffer.SetData((uint)offsetInBytes, (byte*)dataPtr, (uint)dataBytes);
            }

            dataHandle.Free();
        }

        private void PlatformDispose(bool disposing)
        {
            if (disposing)
            {
                _buffer.Dispose();
                _buffer = null;
            }
        }
    }
}
