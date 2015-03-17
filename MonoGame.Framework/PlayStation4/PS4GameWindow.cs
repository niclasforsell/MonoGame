// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using Sce.PlayStation4.System;


namespace Microsoft.Xna.Framework
{
    internal class PS4GameWindow : GameWindow
    {
        private readonly PS4GamePlatform _platform;

        #region Internal Properties

        internal Game Game { get; private set; }

        #endregion

        #region Public Properties

        public override IntPtr Handle { get { return IntPtr.Zero; } }

        public override string ScreenDeviceName { get { return String.Empty; } }

        public override Rectangle ClientBounds
        {
            get
            {
                return new Rectangle(0, 0, 1920, 1080);
            }
        }

        public override bool AllowUserResizing
        {
            get { return false; }
            set
            {
                throw new NotSupportedException();
            }
        }

        public override DisplayOrientation CurrentOrientation
        {
            get { return DisplayOrientation.Default; }
        }

        protected internal override void SetSupportedOrientations(DisplayOrientation orientations)
        {
        }

        public override bool IsBorderless
        {
            get { return true; }
            set
            {
                throw new NotSupportedException();
            }
        }

        #endregion

        internal PS4GameWindow(PS4GamePlatform platform)
        {
            _platform = platform;
            Game = platform.Game;
        }

        internal void Initialize(int width, int height)
        {           
        }

        protected override void SetTitle(string title)
        {
        }

        internal void RunLoop()
        {
            while (true)
            {
                // Update the active state.
                _platform.IsActive = !SystemService.IsInBackgroundExecution;

                Game.Tick();

                //if (IsExiting)
                    //break;
            }
        }

        #region Public Methods

        public void Dispose()
        {
        }

        public override void BeginScreenDeviceChange(bool willBeFullScreen)
        {
        }

        public override void EndScreenDeviceChange(string screenDeviceName, int clientWidth, int clientHeight)
        {
        }

        public void MouseVisibleToggled()
        {
        }

        #endregion
    }
}

