// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;
using Game = Microsoft.Xna.Framework.Game;
using Microsoft.Xna.Framework.Audio;
using SoundSystem = Sce.PlayStation4.Audio.SoundSystem;
using UserService = Sce.PlayStation4.UserService;

namespace Microsoft.Xna.Framework
{
    [AttributeUsage(AttributeTargets.Method)]
    internal sealed class MonoPInvokeCallbackAttribute : Attribute
    {
        public MonoPInvokeCallbackAttribute(Type t) { }
    }

    internal class PS4GamePlatform : GamePlatform
    {
        internal static string LaunchParameters;

        private PS4GameWindow _window;
        private readonly List<Keys> _keyState;

        public PS4GamePlatform(Game game)
            : base(game)
        {
//            _keyState = new List<Keys>();
//            Keyboard.SetKeys(_keyState);

            _window = new PS4GameWindow(this);
            _window.KeyState = _keyState;

            //Mouse.SetWindows(_window._form);
            
            Window = _window;
        }

        public override GameRunBehavior DefaultRunBehavior
        {
            get { return GameRunBehavior.Synchronous; }
        }

        protected override void OnIsMouseVisibleChanged()
        {
            _window.MouseVisibleToggled();
        }

        public override void BeforeInitialize()
        {
            _window.Initialize(Game.graphicsDeviceManager.PreferredBackBufferWidth, Game.graphicsDeviceManager.PreferredBackBufferHeight);

            SoundSystem.Instance.Initialize();
            UserService.Initialize();
//            SaveDataSystem.Initialize();
            Sce.PlayStation4.Game.ManagedCollect = CollectAndWait;

            base.BeforeInitialize();
        }

        [MonoPInvokeCallback(typeof(Sce.PlayStation4.collect_function))]
        static void CollectAndWait()
        {
            Console.Out.WriteLine("START MANAGED COLLECTION!");
            GC.Collect();
            GC.WaitForPendingFinalizers();
            Console.Out.WriteLine("END MANAGED COLLECTION!");
        }

        public override void RunLoop()
        {
            Sce.PlayStation4.Game.HideSplashScreen();
            _window.RunLoop();
        }

        public override void StartRunLoop()
        {
            throw new NotSupportedException("This platform does not support an asynchronous run loop.");
        }
        
        public override void Exit()
        {
            UserService.Terminate();
        }

        public override bool BeforeUpdate(GameTime gameTime)
        {
            var dt = (float)gameTime.ElapsedGameTime.TotalSeconds;
            UserService.Update(dt);

//            if (AudioEngine.Instance != null)
//                AudioEngine.Instance.Update(dt);
            
            return true;
        }

        public override bool BeforeDraw(GameTime gameTime)
        {
//            var device = Game.GraphicsDevice;
//            if (device != null)
//                device.ResetRenderTargets();

            return true;
        }

        public override void EnterFullScreen()
        {
        }

        public override void ExitFullScreen()
        {
        }

        public override void BeginScreenDeviceChange(bool willBeFullScreen)
        {
        }

        public override void EndScreenDeviceChange(string screenDeviceName, int clientWidth, int clientHeight)
        {
        }

        public override void Log(string message)
        {
            Debug.WriteLine(message);
        }

        public override void Present()
        {
            var device = Game.GraphicsDevice;
            if ( device != null )
                device.Present();
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (_window != null)
                {
                    _window.Dispose();
                    _window = null;
                    Window = null;
                }
            }

            base.Dispose(disposing);
        }
    }
}
