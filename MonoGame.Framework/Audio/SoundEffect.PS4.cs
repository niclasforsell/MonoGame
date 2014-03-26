// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using Microsoft.Xna.Framework.Audio;
using System;
using System.IO;

namespace Microsoft.Xna.Framework.Audio
{
    public sealed partial class SoundEffect
    {
        private void PlatformInitialize(byte[] buffer, int sampleRate, AudioChannels channels)
        {
            throw new NotImplementedException();
        }

        private void PlatformLoadAudioStream(Stream s)
        {
            throw new NotImplementedException();
        }

        private void PlatformInitialize(byte[] buffer, int offset, int count, int sampleRate, AudioChannels channels, int loopStart, int loopLength)
        {
            throw new NotImplementedException();
        }

        private void PlatformSetupInstance(SoundEffectInstance inst)
        {
            throw new NotImplementedException();
        }

        private bool PlatformPlay()
        {
            throw new NotImplementedException();
        }

        private bool PlatformPlay(float volume, float pitch, float pan)
        {
            throw new NotImplementedException();
        }

        private TimeSpan PlatformGetDuration()
        {
            throw new NotImplementedException();
        }

        private static void PlatformSetMasterVolume()
        {
            throw new NotImplementedException();
        }

        private void PlatformDispose()
        {
            throw new NotImplementedException();
        }
    }
}
