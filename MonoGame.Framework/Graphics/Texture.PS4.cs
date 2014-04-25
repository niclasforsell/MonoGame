// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using System.Runtime.InteropServices;
using PS4Texture = Sce.PlayStation4.Graphics.Texture;

namespace Microsoft.Xna.Framework.Graphics
{
    public partial class Texture
    {
        internal PS4Texture _texture;

        private void PlatformGraphicsDeviceResetting()
        {
            throw new NotImplementedException();
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing && _texture != null)
            {
                _texture.Dispose();
                _texture = null;
            }

            base.Dispose(disposing);
        }
    }
}
