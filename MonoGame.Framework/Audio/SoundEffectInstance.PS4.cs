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
        private void PlatformInitialize(byte[] buffer, int sampleRate, int channels)
        {
            throw new NotImplementedException();
        }

        private void PlatformApply3D(AudioListener listener, AudioEmitter emitter)
        {
            throw new NotImplementedException();
        }

        private void PlatformPause()
        {
            throw new NotImplementedException();
        }

        private void PlatformPlay()
        {
            throw new NotImplementedException();
        }

        private void PlatformResume()
        {
            throw new NotImplementedException();
        }

        private void PlatformStop(bool immediate)
        {
            throw new NotImplementedException();
        }

        private void PlatformSetIsLooped(bool value)
        {
            throw new NotImplementedException();
        }

        private bool PlatformGetIsLooped()
        {
            throw new NotImplementedException();
        }

        private void PlatformSetPan(float value)
        {
            throw new NotImplementedException();
        }

        private void PlatformSetPitch(float value)
        {
            throw new NotImplementedException();
        }

        private float PlatformGetPitch()
        {
            throw new NotImplementedException();
        }

        private SoundState PlatformGetState()
        {
            throw new NotImplementedException();
        }

        private void PlatformSetVolume(float value)
        {
            throw new NotImplementedException();
        }

        private float PlatformGetVolume()
        {
            throw new NotImplementedException();
        }

        private void PlatformDispose()
        {
            throw new NotImplementedException();
        }
    }
}
