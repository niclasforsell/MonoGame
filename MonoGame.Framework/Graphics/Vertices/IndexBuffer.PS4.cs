// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using Sce.PlayStation4.Graphics;
using PSIndexBuffer = Sce.PlayStation4.Graphics.IndexBuffer;

namespace Microsoft.Xna.Framework.Graphics
{
    public partial class IndexBuffer
    {
        internal PSIndexBuffer _buffer;

        private void PlatformConstruct(IndexElementSize indexElementSize, int indexCount)
        {
            _buffer = new PSIndexBuffer((IndexElement)IndexElementSize, (uint)indexCount);
        }

        private void PlatformGraphicsDeviceResetting()
        {
            throw new NotSupportedException();
        }

        private void PlatformGetData<T>(int offsetInBytes, T[] data, int startIndex, int elementCount) where T : struct
        {
            throw new NotImplementedException();
        }

        private void PlatformSetDataInternal<T>(int offsetInBytes, T[] data, int startIndex, int elementCount, SetDataOptions options) where T : struct
        {
            throw new NotImplementedException();
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
