// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using Microsoft.Xna.Framework;

namespace Microsoft.Xna.Framework.Input
{
    public struct GamePadTouch
    {
        /// <summary>
        /// Touch report ID (see ScePadTouch documentation)
        /// </summary>
        public byte Id;

        /// <summary>
        /// Normalized position on the touchpad with the top-left corner as the origin.
        /// </summary>
        public Vector2 Position;
    }

    public partial struct GamePadState
    {
        public Quaternion Orientation { get; internal set; }
        public Vector3 Acceleration { get; internal set; }
        public Vector3 AngularVelocity { get; internal set; }

        /// <summary>
        /// Number of active touch points (always between 0 and 2)
        /// </summary>
        public int TouchCount { get; internal set; }
        public GamePadTouch TouchPoint1 { get; internal set; }
        public GamePadTouch TouchPoint2 { get; internal set; }
    }
}
