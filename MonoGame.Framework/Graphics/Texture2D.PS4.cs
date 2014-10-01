// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using System.IO;
using System.Runtime.InteropServices;
using PS4Texture = Sce.PlayStation4.Graphics.Texture;
using PS4TextureFormat = Sce.PlayStation4.Graphics.TextureFormat;
using PngReader = Sce.PlayStation4.Graphics.PngReader;
using PngReaderResult = Sce.PlayStation4.Graphics.PngReaderResult;

namespace Microsoft.Xna.Framework.Graphics
{
    public partial class Texture2D
    {
        protected void PlatformConstruct(int width, int height, bool mipmap, SurfaceFormat format, SurfaceType type, bool shared)
        {
            if (type != SurfaceType.Texture)
                return;

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
                    _texture.SetData((uint)level, (byte*)dataPtr, 0, (uint)(elementSizeInByte * elementCount));
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
                    _texture.GetData((uint)level, (byte*)dataPtr, 0, (uint)(elementSizeInByte * elementCount));
                }
            }

            dataHandle.Free();
        }

        private static readonly byte[] _pngSignature = { 137, 80, 78, 71, 13, 10, 26, 10 };

        private static unsafe Texture2D PlatformFromStream(GraphicsDevice graphicsDevice, Stream stream)
        {
            var rawBytes = new byte[stream.Length];
            stream.Read(rawBytes, 0, (int)stream.Length);

            if (rawBytes.Length >= _pngSignature.Length)
            {
                var isPng = true;
                for (var i = 0; i < _pngSignature.Length; i++)
                {
                    if (rawBytes[i] != _pngSignature[i])
                    {
                        isPng = false;
                        break;
                    }
                }

                if (isPng)
                    return FromPngStream(graphicsDevice, rawBytes);
            }

            throw new NotSupportedException("File corrupt or format not supported.");
        }

        private static unsafe Texture2D FromPngStream(GraphicsDevice graphicsDevice, byte[] rawBytes)
        {
            using (var reader = new PngReader())
            {
                PngReaderResult result = PngReaderResult.Fatal;

                result = reader.Initialize(8192); // TODO: Is there a good place to get this size hint?
                if (result != PngReaderResult.OK)
                    throw new Exception("Error initializing PNG reader: " + result.ToString());

                fixed (byte* compressed = rawBytes)
                    result = reader.LoadCompressed(compressed, rawBytes.Length);

                if (result != PngReaderResult.OK)
                    throw new Exception("Unable to load PNG from stream: " + result.ToString());

                var decomp = new byte[reader.OutputSize];
                fixed (byte* uncompressed = decomp)
                {
                    result = reader.OutputUncompressed(uncompressed, decomp.Length);
                    if (result != PngReaderResult.OK)
                        throw new Exception("Error decompressing PNG: " + result.ToString());
                }

                var texture = new Texture2D(graphicsDevice, reader.Width, reader.Height);
                texture.SetData(decomp);

                reader.Cleanup();

                return texture;
            }
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
