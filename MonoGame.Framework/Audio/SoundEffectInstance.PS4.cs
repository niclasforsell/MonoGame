// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using Microsoft.Xna.Framework.Audio;
using System;
using System.IO;

namespace Microsoft.Xna.Framework.Audio
{
    public sealed partial class SoundEffectInstance
    {
        internal Sce.PlayStation4.Audio.SamplerVoice _voice;
        internal Sce.PlayStation4.Audio.AudioBuffer _buffer;

        private void PlatformInitialize(byte[] buffer, int sampleRate, int channels)
        {
            throw new NotImplementedException();
        }

        private void PlatformApply3D(AudioListener listener, AudioEmitter emitter)
        {
            if (_voice == null)
                return;

            Sce.PlayStation4.Audio.SoundSystem.Instance.SubmitPlaybackEvent(_voice, _buffer, (int)SoundState.Paused, -1);
        }

        private void PlatformPause()
        {
            if (_voice == null)
                return;

            Sce.PlayStation4.Audio.SoundSystem.Instance.SubmitPlaybackEvent(_voice, _buffer, (int)SoundState.Paused, -1);
        }

        private void PlatformPlay()
        {
            if (_voice == null)
                return;

            Sce.PlayStation4.Audio.SoundSystem.Instance.SubmitPlaybackEvent(_voice, _buffer, (int)SoundState.Playing, -1);
        }

        private void PlatformResume()
        {
            if (_voice == null)
                return;

            Sce.PlayStation4.Audio.SoundSystem.Instance.SubmitPlaybackEvent(_voice, _buffer, (int)SoundState.Playing, -1);
        }

        private void PlatformStop(bool immediate)
        {
            if (_voice == null)
                return;

            Sce.PlayStation4.Audio.SoundSystem.Instance.SubmitPlaybackEvent(_voice, _buffer, (int)SoundState.Stopped, -1);
        }

        private void PlatformSetIsLooped(bool value)
        {
            if (_voice == null)
                return;

            _voice.Looped = value;
        }

        private bool PlatformGetIsLooped()
        {
            if (_voice == null)
                return false;

            return _voice.Looped;
        }

        private float PlatformGetPan()
        {
            if (_voice == null)
                return 0.0f;

            return _voice.Pan;
        }

        private void PlatformSetPan(float value)
        {
            if (_voice == null)
                return;

            _voice.Pan = value;
        }

        private void PlatformSetPitch(float value)
        {
            if (_voice == null)
                return;

            _voice.Pitch = value;
        }

        private float PlatformGetPitch()
        {
            if (_voice == null)
                return 0.0f;

            return _voice.Pitch;
        }

        private SoundState PlatformGetState()
        {
            if (_voice == null)
                return SoundState.Stopped;

            // This assumes the native SoundState mirrors the managed one.
            return (SoundState)_voice.State;
        }

        private void PlatformSetVolume(float value)
        {
            if (_voice == null)
                return;

            _voice.Volume = value;
        }

        private float PlatformGetVolume()
        {
            if (_voice == null)
                return 0.0f;

            return _voice.Volume;
        }

        private void PlatformDispose()
        {
            if (_voice != null)
            {
                _voice.Dispose();
                _voice = null;
            }
        }
    }
}
