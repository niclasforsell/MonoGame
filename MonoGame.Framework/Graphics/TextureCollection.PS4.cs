// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

namespace Microsoft.Xna.Framework.Graphics
{
    public sealed partial class TextureCollection
    {
        private void PlatformInit()
        {
        }

        internal void ClearTargets(GraphicsDevice device, RenderTargetBinding[] targets)
        {
            // We assume 4 targets to avoid a loop within a loop below.
            var target0 = targets[0].RenderTarget;
            var target1 = targets[1].RenderTarget;
            var target2 = targets[2].RenderTarget;
            var target3 = targets[3].RenderTarget;

            // Make one pass across all the texture slots.
            for (var i = 0; i < _textures.Length; i++)
            {
                var tex = _textures[i];
                if (tex == null || 
                    (   tex != target0 &&
                        tex != target1 &&
                        tex != target2 &&
                        tex != target3))
                    continue;

                // Immediately clear the texture from the device.
                _dirty &= ~(1 << i);
                _textures[i] = null;

                device._system.SetTexture(i, null);
            }
        }

        private void PlatformClear()
        {
        }

        private void PlatformSetTextures(GraphicsDevice device)
        {
            // Skip out if nothing has changed.
            if (_dirty == 0)
                return;

            for (var i = 0; i < _textures.Length; i++)
            {
                var mask = 1 << i;
                if ((_dirty & mask) == 0)
                    continue;

                var rt2D = _textures[i] as RenderTarget2D;
                var tex2D = _textures[i] as Texture2D;
                if (rt2D != null)
                    device._system.SetTextureRT(i, rt2D._target);
                else if (tex2D != null)
                    device._system.SetTexture(i, tex2D._texture);
                else
                    device._system.SetTexture(i, null);

                _dirty &= ~mask;
                if (_dirty == 0)
                    break;
            }

            _dirty = 0;
        }
    }
}
