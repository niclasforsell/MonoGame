// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using PSGraphicsDevice = Sce.PlayStation4.Graphics.GraphicsSystem;
using PSTextureFilter = Sce.PlayStation4.Graphics.TextureFilter;
using PSTextureAddressMode = Sce.PlayStation4.Graphics.TextureAddressMode;

namespace Microsoft.Xna.Framework.Graphics
{
    public partial class SamplerStateCollection
    {
        private int _dirty = int.MaxValue;

        private void PlatformSetSamplerState(int index)
        {
            _dirty |= 1 << index;
        }

        private void PlatformClear()
        {
            _dirty = int.MaxValue;
        }

        private void PlatformDirty()
        {
            _dirty = int.MaxValue;
        }

        internal void PlatformSetSamplers(GraphicsDevice device)
        {
            // Skip out if nothing has changed.
            if (_dirty == 0)
                return;

            for (var i = 0; i < _samplers.Length; i++)
            {
                var mask = 1 << i;
                if ((_dirty & mask) == 0)
                    continue;

                var sampler = _samplers[i];
                if (sampler != null)
                {
                    if (sampler._dirty)
                    {
                        unsafe
                        {
                            fixed (uint *desc0 = &sampler._desc0)
                            fixed (uint* desc1 = &sampler._desc1)
                            fixed (uint* desc2 = &sampler._desc2)
                            fixed (uint* desc3 = &sampler._desc3)
                            {
                                PSGraphicsDevice.CreateSamplerState(
                                    (PSTextureFilter)sampler.Filter,
                                    (PSTextureAddressMode)sampler.AddressU,
                                    (PSTextureAddressMode)sampler.AddressU,
                                    (PSTextureAddressMode)sampler.AddressU,
                                    sampler.MaxAnisotropy,
                                    sampler.MaxMipLevel,
                                    sampler.MipMapLevelOfDetailBias,
                                    desc0, desc1, desc2, desc3);
                            }
                        }

                        sampler._dirty = false;
                    }

                    device._system.SetSamplerState(i, sampler._desc0, sampler._desc1, sampler._desc2, sampler._desc3);
                }
                
                _dirty &= ~mask;
                if (_dirty == 0)
                    break;
            }

            _dirty = 0;
        }
    }
}
