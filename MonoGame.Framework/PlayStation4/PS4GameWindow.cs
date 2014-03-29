// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.InteropServices;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Input.Touch;
using ButtonState = Microsoft.Xna.Framework.Input.ButtonState;
using Rectangle = Microsoft.Xna.Framework.Rectangle;

namespace Microsoft.Xna.Framework
{
    internal class PS4GameWindow : GameWindow
    {
        private readonly PS4GamePlatform _platform;

        private bool _isMouseHidden;

        private bool _isMouseInBounds;

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

        #region Non-Public Properties

        internal List<Keys> KeyState { get; set; }

        #endregion

        internal PS4GameWindow(PS4GamePlatform platform)
        {
            _platform = platform;
            Game = platform.Game;

            /*
            _form = new WinFormsGameForm();
            
            // When running unit tests this can return null.
            var assembly = Assembly.GetEntryAssembly();
            if (assembly != null)
                _form.Icon = Icon.ExtractAssociatedIcon(assembly.Location);

            _form.MaximizeBox = false;
            _form.FormBorderStyle = FormBorderStyle.FixedSingle;
            _form.StartPosition = FormStartPosition.CenterScreen;           

            // Capture mouse events.
            _form.MouseDown += OnMouseState;
            _form.MouseMove += OnMouseState;
            _form.MouseUp += OnMouseState;
            _form.MouseWheel += OnMouseState;
            _form.MouseEnter += OnMouseEnter;
            _form.MouseLeave += OnMouseLeave;            

            // Use RawInput to capture key events.
            Device.RegisterDevice(UsagePage.Generic, UsageId.GenericKeyboard, DeviceFlags.None);
            Device.KeyboardInput += OnRawKeyEvent;

            _form.Activated += OnActivated;
            _form.Deactivate += OnDeactivate;
            _form.ClientSizeChanged += OnClientSizeChanged;

            _form.KeyPress += OnKeyPress;
            */
        }

        private void OnActivated(object sender, EventArgs eventArgs)
        {
            _platform.IsActive = true;
        }

        private void OnDeactivate(object sender, EventArgs eventArgs)
        {
            _platform.IsActive = false;

            if (KeyState != null)
                KeyState.Clear();
        }

        /*
        private void OnMouseState(object sender, MouseEventArgs mouseEventArgs)
        {
            var previousState = MouseState.LeftButton;

            MouseState.X = mouseEventArgs.X;
            MouseState.Y = mouseEventArgs.Y;
            MouseState.LeftButton = (mouseEventArgs.Button & MouseButtons.Left) == MouseButtons.Left ? ButtonState.Pressed : ButtonState.Released;
            MouseState.MiddleButton = (mouseEventArgs.Button & MouseButtons.Middle) == MouseButtons.Middle ? ButtonState.Pressed : ButtonState.Released;
            MouseState.RightButton = (mouseEventArgs.Button & MouseButtons.Right) == MouseButtons.Right ? ButtonState.Pressed : ButtonState.Released;
            MouseState.ScrollWheelValue += mouseEventArgs.Delta;
            
            TouchLocationState? touchState = null;
            if (MouseState.LeftButton == ButtonState.Pressed)
                if (previousState == ButtonState.Released)
                    touchState = TouchLocationState.Pressed;
                else
                    touchState = TouchLocationState.Moved;
            else if (previousState == ButtonState.Pressed)
                touchState = TouchLocationState.Released;

            if (touchState.HasValue)
                 TouchPanel.AddEvent(0, touchState.Value, new Vector2(MouseState.X, MouseState.Y), true);
        } 

        private void OnMouseEnter(object sender, EventArgs e)
        {
            _isMouseInBounds = true;
            if (!_platform.IsMouseVisible && !_isMouseHidden)
            {
                _isMouseHidden = true;
                Cursor.Hide();
            }
        }

        private void OnMouseLeave(object sender, EventArgs e)
        {
            _isMouseInBounds = false;
            if (_isMouseHidden)
            {
                _isMouseHidden = false;
                Cursor.Show();
            }
        }

        private void OnRawKeyEvent(object sender, KeyboardInputEventArgs args)
        {
            if (KeyState == null)
                return;

            XnaKey xnaKey;

            switch (args.MakeCode)
            {
                case 0x2a: // LShift
                    xnaKey = XnaKey.LeftShift;
                    break;

                case 0x36: // RShift
                    xnaKey = XnaKey.RightShift;
                    break;

                case 0x1d: // Ctrl
                    xnaKey = (args.ScanCodeFlags & ScanCodeFlags.E0) != 0 ? XnaKey.RightControl : XnaKey.LeftControl;
                    break;

                case 0x38: // Alt
                    xnaKey = (args.ScanCodeFlags & ScanCodeFlags.E0) != 0 ? XnaKey.RightAlt : XnaKey.LeftAlt;
                    break;

                default:
                    xnaKey = (XnaKey)args.Key;
                    break;
            }

            if (args.State == SharpDX.RawInput.KeyState.KeyDown && !KeyState.Contains(xnaKey))
                KeyState.Add(xnaKey);
            else if (args.State == SharpDX.RawInput.KeyState.KeyUp)
                KeyState.Remove(xnaKey);
        }

        private void OnKeyPress(object sender, KeyPressEventArgs e)
        {
            OnTextInput(sender, new TextInputEventArgs(e.KeyChar));
        }
        */

        internal void Initialize(int width, int height)
        {           
            /*
            _form.ClientSize = new Size(width, height);
            _form.Show();
            */
        }

        /*
        private void OnClientSizeChanged(object sender, EventArgs eventArgs)
        {
            if (Game.Window == this)
            {
                var manager = Game.graphicsDeviceManager;

                // Set the default new back buffer size and viewport, but this
                // can be overloaded by the two events below.

                var newWidth = _form.ClientRectangle.Width;
                var newHeight = _form.ClientRectangle.Height;
                manager.PreferredBackBufferWidth = newWidth;
                manager.PreferredBackBufferHeight = newHeight;

                if (manager.GraphicsDevice == null)
                    return;
            }

            // Set the new view state which will trigger the 
            // Game.ApplicationViewChanged event and signal
            // the client size changed event.
            OnClientSizeChanged();
        }
        */

        protected override void SetTitle(string title)
        {
        }

        internal void RunLoop()
        {
            while (true)
            {
                // Update the active state.
                _platform.IsActive = Sce.PlayStation4.Game.IsActive;

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

