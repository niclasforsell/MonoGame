// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using System.IO;
using Sce.PlayStation4.Graphics;

namespace Microsoft.Xna.Framework.Graphics
{
    internal partial class Shader
    {
        internal VertexShader _vertexShader;
        internal PixelShader _pixelShader;

        private void PlatformConstruct(BinaryReader reader, bool isVertexShader, byte[] shaderBytecode)
        {
            unsafe
            {
                fixed (void* data = shaderBytecode)
                {
                    if (isVertexShader)
                        _vertexShader = new VertexShader(data);
                    else
                        _pixelShader = new PixelShader(data);
                }
            }

            HashKey = MonoGame.Utilities.Hash.ComputeHash(shaderBytecode);
        }

        private void PlatformGraphicsDeviceResetting()
        {
            // I don't think this happens on PS4!
            throw new NotImplementedException();
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (_vertexShader != null)
                    _vertexShader.Dispose();
                if (_pixelShader != null)
                    _pixelShader.Dispose();
            }

            base.Dispose(disposing);
        }
    }
}
