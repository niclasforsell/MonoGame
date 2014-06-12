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
        private RenderTargetBinding[] _prevRenderTargets;

        public VideoPlayer(GraphicsDevice graphicsDevice)
        {
            _graphicsDevice = graphicsDevice;
            PlatformInitialize();
        }

        private void PlatformInitialize()
        {
            if (_graphicsDevice == null)
                throw new Exception("PS4 requires VideoPlayer to have access the graphics device.");

            // Use a render target that will preserve contents, to allow frame doubling
            // instead of flickering in the case of dropped frames.
            _renderTarget = new RenderTarget2D(_graphicsDevice,
                                               _graphicsDevice.PresentationParameters.BackBufferWidth,
                                               _graphicsDevice.PresentationParameters.BackBufferHeight,
                                               false,
                                               SurfaceFormat.Color,
                                               DepthFormat.None,
                                               1,
                                               RenderTargetUsage.PreserveContents);

            // Clear initial render target contents
            _prevRenderTargets = new RenderTargetBinding[_graphicsDevice.RenderTargetCount];
            _graphicsDevice.GetRenderTargets(_prevRenderTargets);
            _graphicsDevice.SetRenderTarget(_renderTarget);
            _graphicsDevice.Clear(Color.Black);
            _graphicsDevice.SetRenderTargets(_prevRenderTargets);
            _graphicsDevice.PlatformSetDirty();

            _player = new PS4VideoPlayer(_graphicsDevice._system);
        }

        private Texture2D PlatformGetTexture()
        {
            // GrabFrame will take whatever render target is set and draw the video
            // into it. Here we store the render target state, swap our own in for
            // the video render, and then restore it at the end.

            if (_prevRenderTargets == null || _prevRenderTargets.Length != _graphicsDevice.RenderTargetCount)
                _prevRenderTargets = new RenderTargetBinding[_graphicsDevice.RenderTargetCount];

            _graphicsDevice.GetRenderTargets(_prevRenderTargets);
            _graphicsDevice.SetRenderTarget(_renderTarget);

            if(_player.GrabFrame())
                _graphicsDevice.PlatformSetDirty();

            _graphicsDevice.SetRenderTargets(_prevRenderTargets);

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

        private void PlatformSetIsLooped()
        {
            _player.IsLooped = _isLooped;
        }

        private void PlatformSetIsMuted()
        {
            _player.IsMuted = _isMuted;
        }

        private TimeSpan PlatformGetPlayPosition()
        {
            return TimeSpan.FromMilliseconds(_player.PlayPosition);
        }

        private void PlatformDispose(bool disposing)
        {
            _player.Dispose();
            _renderTarget.Dispose();
        }
    }
}