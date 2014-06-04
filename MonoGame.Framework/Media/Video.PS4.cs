// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using System.IO;


namespace Microsoft.Xna.Framework.Media
{
    public sealed partial class Video
    {
        // Temporary hack to bypass content pipeline
        public static Video FromFile(string filename)
        {
            return new Video(filename);
        }

        private void PlatformInitialize()
        {
        }

        private void PlatformDispose(bool disposing)
        {
        }
    }
}
