// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;

namespace Microsoft.Xna.Framework.Media
{
    public sealed class Video
    {
        internal Video(string fileName)
        {
            throw new NotImplementedException();
        }

        public TimeSpan Duration { get; internal set; }
        public float FramesPerSecond { get; internal set; }
        public int Height { get; internal set; }
        public int Width { get; internal set; }
        public VideoSoundtrackType VideoSoundtrackType { get; internal set; }

        internal static string Normalize(string fileName)
        {
            throw new NotImplementedException();
        }
    }
}
