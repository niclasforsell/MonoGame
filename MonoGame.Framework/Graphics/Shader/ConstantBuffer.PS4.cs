// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using PSShaderStage = Sce.PlayStation4.Graphics.ShaderStage;

namespace Microsoft.Xna.Framework.Graphics
{
    internal partial class ConstantBuffer
    {
        private void PlatformInitialize()
        {
            _dirty = false;
        }

        private void PlatformClear()
        {
            _dirty = true;
        }

        internal void PlatformApply(GraphicsDevice device, ShaderStage stage, int slot)
        {
            // PS4 we copy the data always!
            _dirty = false;

            var system = GraphicsDevice._system;
            var sizeInBytes = _buffer.Length;

            unsafe
            {
                fixed(void *data = _buffer)
                {
                    // Set the buffer to the right stage.
                    if (stage == ShaderStage.Vertex)
                        system.SetShaderConstants(PSShaderStage.ShaderStage_Vertex, data, (uint)sizeInBytes);
                    else
                        system.SetShaderConstants(PSShaderStage.ShaderStage_Pixel, data, (uint)sizeInBytes);
                }
            }
        }
    }
}
