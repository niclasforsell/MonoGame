// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using PSGraphicsDevice = Sce.PlayStation4.Graphics.GraphicsSystem;
using PSBlend = Sce.PlayStation4.Graphics.Blend;
using PSBlendFunction = Sce.PlayStation4.Graphics.BlendFunction;

namespace Microsoft.Xna.Framework.Graphics
{
    public partial class BlendState
    {
        private bool _dirty = true;
        private uint _blend0;
        private uint _blend1;
        private uint _blend2;
        private uint _blend3;

        internal void PlatformApplyState(GraphicsDevice device)
        {
            if (_dirty)
            {
                PSGraphicsDevice.CreateBlendState(
                    (PSBlend)_targetBlendState[0].ColorSourceBlend,
                    (PSBlend)_targetBlendState[0].ColorDestinationBlend,
                    (PSBlendFunction)_targetBlendState[0].ColorBlendFunction,
                    out _blend0);

                PSGraphicsDevice.CreateBlendState(
                    (PSBlend)_targetBlendState[1].ColorSourceBlend,
                    (PSBlend)_targetBlendState[1].ColorDestinationBlend,
                    (PSBlendFunction)_targetBlendState[1].ColorBlendFunction,
                    out _blend1);

                PSGraphicsDevice.CreateBlendState(
                    (PSBlend)_targetBlendState[2].ColorSourceBlend,
                    (PSBlend)_targetBlendState[2].ColorDestinationBlend,
                    (PSBlendFunction)_targetBlendState[2].ColorBlendFunction,
                    out _blend2);

                PSGraphicsDevice.CreateBlendState(
                    (PSBlend)_targetBlendState[3].ColorSourceBlend,
                    (PSBlend)_targetBlendState[3].ColorDestinationBlend,
                    (PSBlendFunction)_targetBlendState[3].ColorBlendFunction,
                    out _blend3);

                _dirty = false;
            }

            device._system.SetBlendState(_blend0, _blend1, _blend2, _blend3);            
        }

        private void PlatformDispose()
        {
        }
    }
}
