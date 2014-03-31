// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using PSGraphicsDevice = Sce.PlayStation4.Graphics.GraphicsSystem;
using PSCompareFunction = Sce.PlayStation4.Graphics.CompareFunction;

namespace Microsoft.Xna.Framework.Graphics
{
    public partial class DepthStencilState
    {
        private bool _dirty = true;
        private uint _depth0;

        internal void PlatformApplyState(GraphicsDevice device)
        {
            if (_dirty)
            {
                /*
                public bool StencilEnable { get; set; }
                public int ReferenceStencil { get; set; }
                public CompareFunction StencilFunction { get; set; }
                public CompareFunction CounterClockwiseStencilFunction { get; set; }
                public StencilOperation CounterClockwiseStencilDepthBufferFail { get; set; }
                public StencilOperation CounterClockwiseStencilFail { get; set; }
                public StencilOperation CounterClockwiseStencilPass { get; set; }
                public StencilOperation StencilFail { get; set; }
                public StencilOperation StencilDepthBufferFail { get; set; }
                public int StencilMask { get; set; }
                public StencilOperation StencilPass { get; set; }
                public int StencilWriteMask { get; set; }
                public bool TwoSidedStencilMode { get; set; }
                */

                PSGraphicsDevice.CreateDepthStencilState(
                    DepthBufferEnable,
                    DepthBufferWriteEnable,
                    (PSCompareFunction)DepthBufferFunction,
                    out _depth0);

                _dirty = false;
            }

            device._system.SetDepthStencilState(_depth0);
        }
    }
}
