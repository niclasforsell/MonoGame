// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Sce.PlayStation4.Media;
using PS4Song = Sce.PlayStation4.Media.Song;

namespace Microsoft.Xna.Framework.Media
{
    public sealed partial class Song : IEquatable<Song>, IDisposable
    {
        private string _fileName;
        private PS4Song _player;

        internal void PlatformInitialize(string fileName)
        {
            _fileName = fileName;
        }

        internal void SetEventHandler(Action<object, EventArgs> OnSongFinishedPlaying)
        {
        }

        public void PlatformDispose(bool disposing)
        {
            Stop();
        }

        internal void Play()
        {
            if (_player != null)
                Stop();

            _player = new PS4Song();
            if (!_player.Load(_fileName))
                throw new Exception();

            _player.Play();
        }

        internal void Resume()
        {
            if (_player == null)
                return;

            _player.Resume();
        }

        internal void Pause()
        {
            if (_player == null)
                return;

            _player.Pause();
        }

        internal void Stop()
        {
            if (_player == null)
                return;

            _player.Stop();
            _player.Dispose();
            _player = null;
        }

        internal float Volume
        {
            get
            {
                if (_player == null)
                    return 0.0f;

                return _player.Volume;
            }
            set
            {
                if (_player == null)
                    return;

                _player.Volume = MathHelper.Clamp(value, 0, 1);
            }
        }

        internal TimeSpan Position
        {
            get
            {
                if (_player == null)
                    return TimeSpan.Zero;

                return TimeSpan.FromMilliseconds(Duration.TotalMilliseconds * _player.Position);
            }
        }
    }
}
