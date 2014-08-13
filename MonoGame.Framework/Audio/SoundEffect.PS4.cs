// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using Microsoft.Xna.Framework.Audio;
using System;
using System.IO;
using System.Runtime.InteropServices;
using AudioBuffer = Sce.PlayStation4.Audio.AudioBuffer;
using SoundSystem = Sce.PlayStation4.Audio.SoundSystem;

namespace Microsoft.Xna.Framework.Audio
{
    public sealed partial class SoundEffect
    {
        private AudioBuffer _buffer;

        internal SoundEffect(byte[] buffer, int durationMs)
        {
            _duration = TimeSpan.FromMilliseconds(durationMs);

            unsafe
            {
                var handle = GCHandle.Alloc(buffer, GCHandleType.Pinned);
                var addr = (IntPtr)(handle.AddrOfPinnedObject().ToInt64());
                _buffer = AudioBuffer.FromRIFF((void*)addr, (uint)buffer.Length);
                handle.Free();
            }
        }

        /// <summary>
        /// PlayStation 4 specific version of Play to output a sound on the player's controller.
        /// </summary>
        public bool Play(PlayerIndex playerIndex, float volume = 1.0f, float pitch = 0.0f, float pan = 0.0f)
        {
            var inst = GetPooledInstance(false);
            if (inst == null)
                return false;

            inst._controllerPort = (int)playerIndex;
            inst.Volume = volume;
            inst.Pitch = pitch;
            inst.Pan = pan;
            inst.Play();

            return true;
        }

        private void PlatformInitialize(byte[] buffer, int sampleRate, AudioChannels channels)
        {
            PlatformInitialize(buffer, 0, buffer.Length, sampleRate, channels, 0, buffer.Length);
        }

        private void PlatformInitialize(byte[] buffer, int offset, int count, int sampleRate, AudioChannels channels, int loopStart, int loopLength)
        {
            unsafe
            {
                var handle = GCHandle.Alloc(buffer, GCHandleType.Pinned);
                var addr = (IntPtr)(handle.AddrOfPinnedObject().ToInt64() + offset);
                _buffer = AudioBuffer.FromPCM((void*)addr, (uint)count, sampleRate, (int)channels);
                handle.Free();
            }
        }

        private void PlatformLoadAudioStream(Stream s)
        {
            throw new NotImplementedException();
        }

        private void PlatformSetupInstance(SoundEffectInstance inst)
        {
            inst._voice = SoundSystem.Instance.CreateVoice(_buffer);
            inst._buffer = _buffer;
            inst._voice.Looped = inst.IsLooped;
            inst._controllerPort = -1;
        }

        internal static void PlatformShutdown()
        {
        }

        private void PlatformDispose(bool disposing)
        {
            if (disposing)
            {
                if (_buffer != null)
                    _buffer.Dispose();
            }

            _buffer = null;
        }
    }
}
