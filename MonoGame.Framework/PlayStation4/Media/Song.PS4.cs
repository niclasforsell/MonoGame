using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Sce.PlayStation4.Media;

namespace Microsoft.Xna.Framework.Media
{
    internal class PS4Song : IDisposable
    {
        private readonly MusicPlayer _player;

        internal PS4Song(string fileName)
        {
            _player = new MusicPlayer();
            if (!_player.LoadAT9(fileName))
                throw new Exception();
        }

        public void Dispose()
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
    }
}
