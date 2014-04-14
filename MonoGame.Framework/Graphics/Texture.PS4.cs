// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;

namespace Microsoft.Xna.Framework.Graphics
{
    public partial class Texture
    {
        private void PlatformGraphicsDeviceResetting()
        {
            throw new NotImplementedException();
        }

        protected virtual void PlatformDispose(bool disposing)
        {
            throw new NotImplementedException();
        }
    }
}
