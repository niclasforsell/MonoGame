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

        private struct Attribute
        {
            public VertexElementUsage usage;
            public int index;
            public string name;
        }

        private Attribute[] _attributes;

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

            // TODO: This should simply move to the common code
            // and we should load shader input attributes for
            // all platforms.

            var attributeCount = (int)reader.ReadByte();
            _attributes = new Attribute[attributeCount];
            for (var a = 0; a < attributeCount; a++)
            {
                _attributes[a].name = reader.ReadString();
                _attributes[a].usage = (VertexElementUsage)reader.ReadByte();
                _attributes[a].index = reader.ReadByte();
                reader.ReadInt16(); //format, unused
            }
        }

        internal uint[] GenerateFetchRemap(VertexDeclaration decl)
        {
            var elements = decl.GetVertexElements();
            var remap = new uint[elements.Length];

            // Match each vertex element to an input shader attribute.
            for (var e=0; e < elements.Length; e++)
            {
                var elem = elements[e];

                // If no remap is found we default to unused.
                remap[e] = 0xFFFFFFFF;

                for (var a = 0; a < _attributes.Length; a++)
                {
                    var attr = _attributes[a];

                    if (elem.VertexElementUsage != attr.usage ||
                        elem.UsageIndex != attr.index)
                        continue;

                    remap[e] = (uint)a;
                    break;
                }
            }

            return remap;
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
