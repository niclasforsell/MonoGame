// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;

namespace Microsoft.Xna.Framework.Graphics
{
    public partial class BlendState
    {
        private bool _dirty = true;
        private uint _blend0;

        internal void PlatformApplyState(GraphicsDevice device)
        {
            if (_dirty)
            {
                /*
	            private Color _blendFactor;
	            private int _multiSampleMask;
	            private bool _independentBlendEnable;
                _targetBlendState
                */

                /*
                PSGraphicsDevice.CreateBlendState(
                    DepthBufferEnable,
                    DepthBufferWriteEnable,
                    (PSCompareFunction)DepthBufferFunction,
                    out _depth0);
                */

                _dirty = false;
            }

            device._system.SetBlendState(_blend0);            
        }

        private void PlatformDispose()
        {
        }
    }
}
