// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using Sce.PlayStation4.Graphics;

namespace Microsoft.Xna.Framework.Graphics
{
    public partial class GraphicsDevice
    {
        internal GraphicsSystem _system;

        private void PlatformSetup()
        {
            _system = new GraphicsSystem();
        }

        private void PlatformInitialize()
        {
            _system.Initialize();
        }

        private void PlatformClear(ClearOptions options, Vector4 color, float depth, int stencil)
        {
            _system.Clear(color.X, color.Y, color.Z, color.W);
        }

        private void PlatformDispose()
        {
            _system.Dispose();
            _system = null;
        }

        private void PlatformPresent()
        {
            _system.Present();
        }

        private void PlatformSetViewport(ref Viewport viewport)
        {
            _system.SetViewport(viewport.X, viewport.Y, viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth);
        }

        private void PlatformApplyDefaultRenderTarget()
        {
            _system.SetRenderTarget(null);
        }

        private IRenderTarget PlatformApplyRenderTargets()
        {
            // TODO: Need to support multiple render targets!

            var target = _currentRenderTargetBindings[0].RenderTarget as RenderTarget2D;
            _system.SetRenderTarget(target._target);
            return target;
        }

        internal void PlatformApplyState(bool applyShaders)
        {
            if (_vertexBufferDirty)
            {
                _system.SetVertexBuffer(_vertexBuffer._buffer);
                _vertexBufferDirty = false;
            }

            if (_vertexShader == null)
                throw new InvalidOperationException("A vertex shader must be set!");
            if (_pixelShader == null)
                throw new InvalidOperationException("A pixel shader must be set!");

            if (_vertexShaderDirty)
            {
                _system.SetVertexShader(_vertexShader._vertexShader);
                _vertexShaderDirty = false;
            }

            if (_pixelShaderDirty)
            {
                _system.SetPixelShader(_pixelShader._pixelShader);
                _pixelShaderDirty = false;
            }            
        }

        private void PlatformDrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int startIndex, int primitiveCount)
        {
            throw new NotImplementedException();
        }

        private void PlatformDrawUserPrimitives<T>(PrimitiveType primitiveType, T[] vertexData, int vertexOffset, VertexDeclaration vertexDeclaration, int vertexCount) where T : struct
        {
            throw new NotImplementedException();
        }

        private void PlatformDrawPrimitives(PrimitiveType primitiveType, int vertexStart, int vertexCount)
        {
            throw new NotImplementedException();
        }

        private void PlatformDrawUserIndexedPrimitives<T>(PrimitiveType primitiveType, T[] vertexData, int vertexOffset, int numVertices, short[] indexData, int indexOffset, int primitiveCount, VertexDeclaration vertexDeclaration) where T : struct
        {
            throw new NotImplementedException();
        }

        private void PlatformDrawUserIndexedPrimitives<T>(PrimitiveType primitiveType, T[] vertexData, int vertexOffset, int numVertices, int[] indexData, int indexOffset, int primitiveCount, VertexDeclaration vertexDeclaration) where T : struct, IVertexType
        {
            throw new NotImplementedException();
        }
    }
}
