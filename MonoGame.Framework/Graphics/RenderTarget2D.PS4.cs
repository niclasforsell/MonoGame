// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using Sce.PlayStation4.Graphics;

namespace Microsoft.Xna.Framework.Graphics
{
    public partial class RenderTarget2D
    {
        internal Sce.PlayStation4.Graphics.RenderTarget _target;

        private void PlatformConstruct(GraphicsDevice graphicsDevice, int width, int height, bool mipMap,
            SurfaceFormat preferredFormat, DepthFormat preferredDepthFormat, int preferredMultiSampleCount,
            RenderTargetUsage usage, bool shared)
        {
            _target = new RenderTarget( (TextureFormat)preferredFormat, width, height, 
                                        (Sce.PlayStation4.Graphics.DepthFormat)preferredDepthFormat);
        }

        private void PlatformGraphicsDeviceResetting()
        {
            // Don't think this will happen on PS4.
            throw new NotImplementedException();
        }

        // Use new to replace the PlatformDispose method in Texture2D.
        // This could definitely use some cleanup.
        private new void PlatformDispose(bool disposing)
        {
            if (disposing)
            {
                _target.Dispose();
                _target = null;
            }

            // No need to call base PlatformDispose, we didn't allocate
            // a texture there at construction.
        }
    }
}
