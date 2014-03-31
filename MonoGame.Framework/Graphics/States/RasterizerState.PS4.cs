// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using PSGraphicsDevice = Sce.PlayStation4.Graphics.GraphicsSystem;
using PSCullMode = Sce.PlayStation4.Graphics.CullMode;
using PSFillMode = Sce.PlayStation4.Graphics.FillMode;

namespace Microsoft.Xna.Framework.Graphics
{
    public partial class RasterizerState
    {
        private bool _dirty = true;
        private uint _prim0;
        private uint _flag1;

        internal void PlatformApplyState(GraphicsDevice device)
        {
            if (_dirty)
            {
                 PSGraphicsDevice.CreateRasterizerState(
                    (PSCullMode)CullMode,
                    (PSFillMode)FillMode,
                    MultiSampleAntiAlias,
                    ScissorTestEnable,
                    out _prim0, out _flag1);

                _dirty = false;
            }

            device._system.SetRasterizerState(_prim0, _flag1, DepthBias, SlopeScaleDepthBias);
        }
    }
}
