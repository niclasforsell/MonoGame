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
            // Convert the elements over
            var elements = VertexDeclaration.GetVertexElements();
            var psElements = new int[elements.Length];
            for (var i=0; i < elements.Length; i++)            
                psElements[i] = (int)elements[i].VertexElementFormat;

            unsafe
            {
                fixed (int* ptr = psElements)
                    _buffer = new Sce.PlayStation4.Graphics.VertexBuffer(ptr, psElements.Length, VertexDeclaration.VertexStride, VertexCount);
            }
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

            // TODO: We need to figure out the correct behavior 
            // for SetDataOptions.None on a dynamic buffer.
            //
            // For now we always discard as it is a pretty safe default.
            //
            var discard =   _isDynamic &&
                            (   options == SetDataOptions.Discard || 
                                options == SetDataOptions.None);

            unsafe
            {
                _buffer.SetData(GraphicsDevice._system, offsetInBytes, (byte*)dataPtr, dataBytes, discard);
            }

            dataHandle.Free();
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                _buffer.Dispose();
                _buffer = null;
            }

            base.Dispose(disposing);
        }
    }
}
