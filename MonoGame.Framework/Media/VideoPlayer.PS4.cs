// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using Microsoft.Xna.Framework.Graphics;
using PS4VideoPlayer = Sce.PlayStation4.Media.VideoPlayer;
using PS4VertexShader = Sce.PlayStation4.Graphics.VertexShader;
using PS4PixelShader = Sce.PlayStation4.Graphics.PixelShader;

namespace Microsoft.Xna.Framework.Media
{
    public sealed partial class VideoPlayer
    {
        private GraphicsDevice _graphicsDevice;
        private PS4VideoPlayer _player;
        private RenderTarget2D _renderTarget;

        public VideoPlayer(GraphicsDevice graphicsDevice)
        {
            _graphicsDevice = graphicsDevice;
            PlatformInitialize();
        }

        private void PlatformInitialize()
        {
            if (_graphicsDevice == null)
                throw new Exception("PS4 requires VideoPlayer to have access the graphics device.");

            _renderTarget = new RenderTarget2D(_graphicsDevice,
                                               _graphicsDevice.PresentationParameters.BackBufferWidth,
                                               _graphicsDevice.PresentationParameters.BackBufferHeight);

            _player = new PS4VideoPlayer(_graphicsDevice._system);
        }

        private Texture2D PlatformGetTexture()
        {
            _graphicsDevice.SetRenderTarget(_renderTarget);
            _graphicsDevice.Clear(Color.Black);

            _player.GrabFrame();
            _graphicsDevice.PlatformSetDirty();

            _graphicsDevice.SetRenderTarget(null);
            return _renderTarget;
        }

        private void PlatformPause()
        {
            _player.Pause();
        }

        private void PlatformResume()
        {
            _player.Resume();
        }

        private void PlatformPlay()
        {
            _player.Play(_currentVideo.FileName);
        }

        private void PlatformStop()
        {
            _player.Stop();
        }

        private void PlatformSetVolume()
        {
            _player.Volume = _volume;
        }

        private TimeSpan PlatformGetPlayPosition()
        {
            return TimeSpan.FromSeconds(_player.PlayPosition);
        }
    }
}