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
    public partial class RenderTarget2D
    {
        internal PS4RenderTarget _target;

        private void PlatformConstruct(GraphicsDevice graphicsDevice, int width, int height, bool mipMap,
            SurfaceFormat preferredFormat, DepthFormat preferredDepthFormat, int preferredMultiSampleCount,
            RenderTargetUsage usage, bool shared)
        {
            _target = new PS4RenderTarget((PS4TextureFormat)preferredFormat, width, height, (PS4DepthFormat)preferredDepthFormat);
        }

        private void PlatformGraphicsDeviceResetting()
        {
            // Don't think this will happen on PS4.
            throw new NotImplementedException();
        }

        protected override void PlatformDispose(bool disposing)
        {
            if (disposing && _target != null)
            {
                _target.Dispose();
                _target = null;
            }

            base.PlatformDispose(disposing);
        }
    }
}
