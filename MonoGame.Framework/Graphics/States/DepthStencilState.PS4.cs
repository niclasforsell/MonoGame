// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using PSGraphicsDevice = Sce.PlayStation4.Graphics.GraphicsSystem;
using PSCompareFunction = Sce.PlayStation4.Graphics.CompareFunction;
using PSStencilOperation = Sce.PlayStation4.Graphics.StencilOperation;

namespace Microsoft.Xna.Framework.Graphics
{
    public partial class DepthStencilState
    {
        private bool _dirty = true;
        private uint _depth0;
        private uint _stencil0;
        private uint _stencil1;

        internal void PlatformApplyState(GraphicsDevice device)
        {
            if (_dirty)
            {
                PSGraphicsDevice.CreateDepthStencilState(
                    DepthBufferEnable,
                    DepthBufferWriteEnable,
                    (PSStencilOperation)CounterClockwiseStencilDepthBufferFail,
                    (PSStencilOperation)CounterClockwiseStencilFail,
                    (PSCompareFunction)CounterClockwiseStencilFunction,
                    (PSStencilOperation)CounterClockwiseStencilPass,
                    (PSCompareFunction)DepthBufferFunction,
                    (byte)ReferenceStencil,
                    (PSStencilOperation)StencilDepthBufferFail,
                    StencilEnable,
                    (PSStencilOperation)StencilFail,
                    (PSCompareFunction)StencilFunction,
                    (byte)StencilMask,
                    (PSStencilOperation)StencilPass,
                    (byte)StencilWriteMask,
                    TwoSidedStencilMode,
                    out _depth0,
                    out _stencil0,
                    out _stencil1);

                _dirty = false;
            }

            device._system.SetDepthStencilState(_depth0, _stencil0, _stencil1);
        }
    }
}
