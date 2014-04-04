// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using Microsoft.Xna.Framework;

namespace Microsoft.Xna.Framework.Input
{
    public partial struct GamePadState
    {
        public Quaternion Orientation { get; internal set; }
        public Vector3 Acceleration { get; internal set; }
        public Vector3 AngularVelocity { get; internal set; }
    }
}
