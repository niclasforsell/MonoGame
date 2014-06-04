// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using Microsoft.Xna.Framework.Graphics;
using PS4VideoPlayer = Sce.PlayStation4.Media.VideoPlayer;

namespace Microsoft.Xna.Framework.Media
{
    public sealed partial class VideoPlayer
    {
        PS4VideoPlayer _player;
        Texture2D _texture;

        private void PlatformInitialize()
        {
            _player = new PS4VideoPlayer();
            _texture = new Texture2D(_player.Texture);
        }

        private Texture2D PlatformGetTexture()
        {
            return _texture;
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