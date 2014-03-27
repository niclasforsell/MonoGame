// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using Microsoft.Xna.Framework.Audio;
using System;
using System.IO;
using System.Runtime.InteropServices;

namespace Microsoft.Xna.Framework.Audio
{
    public sealed partial class SoundEffect
    {
        private Sce.PlayStation4.Audio.AudioBuffer _buffer;

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
                _buffer = new Sce.PlayStation4.Audio.AudioBuffer((void*)addr, (uint)count);
                // TODO: Looping?

                handle.Free();
            }
        }

        private void PlatformLoadAudioStream(Stream s)
        {
            throw new NotImplementedException();
        }

        private void PlatformSetupInstance(SoundEffectInstance inst)
        {
            inst._voice = Sce.PlayStation4.Audio.SoundSystem.Instance.CreateVoice(_buffer);
            inst._buffer = _buffer;
        }

        private static void PlatformSetMasterVolume()
        {
            throw new NotImplementedException();
        }

        private void PlatformDispose()
        {
            if (_buffer != null)
            {
                _buffer.Dispose();
                _buffer = null;
            }
        }
    }
}
