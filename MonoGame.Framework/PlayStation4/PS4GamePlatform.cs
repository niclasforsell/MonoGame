// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;

namespace Microsoft.Xna.Framework
{
    internal class PS4GamePlatform : GamePlatform
    {
        public PS4GamePlatform(Game game)
            : base(game)
        {
        }

        public override GameRunBehavior DefaultRunBehavior
        {
            get { throw new NotImplementedException(); }
        }

        public override void Exit()
        {
            throw new NotImplementedException();
        }

        public override void RunLoop()
        {
            throw new NotImplementedException();
        }

        public override void StartRunLoop()
        {
            throw new NotImplementedException();
        }

        public override bool BeforeUpdate(GameTime gameTime)
        {
            throw new NotImplementedException();
        }

        public override bool BeforeDraw(GameTime gameTime)
        {
            throw new NotImplementedException();
        }

        public override void EnterFullScreen()
        {
            throw new NotImplementedException();
        }

        public override void ExitFullScreen()
        {
            throw new NotImplementedException();
        }

        public override void BeginScreenDeviceChange(bool willBeFullScreen)
        {
            throw new NotImplementedException();
        }

        public override void EndScreenDeviceChange(string screenDeviceName, int clientWidth, int clientHeight)
        {
            throw new NotImplementedException();
        }
    }
}
