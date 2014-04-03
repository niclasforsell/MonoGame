// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Sce.PlayStation4.Media;

namespace Microsoft.Xna.Framework.Media
{
    public sealed partial class Song : IEquatable<Song>, IDisposable
    {
        private MusicPlayer _player;

        internal void PlatformInitialize(string fileName)
        {
            _player = new MusicPlayer();
            if (!_player.LoadAT9(fileName))
                throw new Exception();
        }

        internal void SetEventHandler(Action<object, EventArgs> OnSongFinishedPlaying)
        {
        }

        public void PlatformDispose(bool disposing)
        {
            _player.Stop();
            _player.Dispose();
        }

        internal void Play()
        {
            _player.Play();
        }

        internal void Resume()
        {
            _player.Resume();
        }

        internal void Pause()
        {
            _player.Pause();
        }

        internal void Stop()
        {
            _player.Stop();
        }

        internal float Volume
        {
            get { return _player.Volume; }
            set { _player.Volume = value; }
        }

        internal TimeSpan Position
        {
            get
            {
                // TODO: Implement
                return new TimeSpan(0);
            }
        }
    }
}
