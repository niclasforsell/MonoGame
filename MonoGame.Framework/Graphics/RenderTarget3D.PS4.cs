// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using PS4RenderTarget = Sce.PlayStation4.Graphics.RenderTarget;
using PS4Texture = Sce.PlayStation4.Graphics.Texture;
using PS4DepthFormat = Sce.PlayStation4.Graphics.DepthFormat;
using PS4TextureFormat = Sce.PlayStation4.Graphics.TextureFormat;

namespace Microsoft.Xna.Framework.Graphics
{
    public partial class RenderTarget3D
    {
        private PS4RenderTarget _target;

        private void PlatformConstruct(GraphicsDevice graphicsDevice, int width, int height, bool mipMap,
            SurfaceFormat preferredFormat, DepthFormat preferredDepthFormat, int preferredMultiSampleCount, RenderTargetUsage usage)
        {
            _target = PS4RenderTarget.Create3D((PS4TextureFormat)preferredFormat, width, height, Depth, (PS4DepthFormat)preferredDepthFormat);
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing && _target != null)
            {
                _target.Dispose();
                _target = null;
            }

            base.Dispose(disposing);
        }
    }
}
