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
            _system.Initialize( PresentationParameters.BackBufferWidth, 
                                PresentationParameters.BackBufferHeight, 
                                (TextureFormat)PresentationParameters.BackBufferFormat,
                                (Sce.PlayStation4.Graphics.DepthFormat)PresentationParameters.DepthStencilFormat);
            _viewport = new Viewport(0, 0, PresentationParameters.BackBufferWidth, PresentationParameters.BackBufferHeight);
        }

        private void PlatformClear(ClearOptions options, Vector4 color, float depth, int stencil)
        {
            _system.Clear((Sce.PlayStation4.Graphics.ClearOptions)options, color.X, color.Y, color.Z, color.W, depth, stencil);

            // Clear clobbers a bunch of state, so just make it 
            // all dirty and it will get re-applied on the next draw.
            _depthStencilStateDirty = true;
            _blendStateDirty = true;
            _pixelShaderDirty = true;
            _vertexShaderDirty = true;
            _rasterizerStateDirty = true;
            _scissorRectangleDirty = true;
        }

        private void PlatformDispose()
        {
            _system.Dispose();
            _system = null;
        }

        private void PlatformPresent()
        {
            _system.Present();
            
            // Restore the viewport after present.
            _viewport = new Viewport(0, 0, PresentationParameters.BackBufferWidth, PresentationParameters.BackBufferHeight);
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
                if (_vertexBuffer != null)
                    _system.SetVertexBuffer(_vertexBuffer._buffer);
                _vertexBufferDirty = false;
            }

            if (_indexBufferDirty)
            {
                if (_indexBuffer != null)
                    _system.SetIndexBuffer(_indexBuffer._buffer);
                _indexBufferDirty = false;
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

            Textures.SetTextures(this);
            SamplerStates.PlatformSetSamplers(this);
        }

        private void PlatformDrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int startIndex, int primitiveCount)
        {
            PlatformApplyState(true);
            _system.DrawIndexedPrimitives((Sce.PlayStation4.Graphics.PrimitiveType)primitiveType, baseVertex, startIndex, primitiveCount);
        }

        private void PlatformDrawUserPrimitives<T>(PrimitiveType primitiveType, T[] vertexData, int vertexOffset, VertexDeclaration vertexDeclaration, int vertexCount) where T : struct
        {
            throw new NotImplementedException();
        }

        private void PlatformDrawPrimitives(PrimitiveType primitiveType, int vertexStart, int vertexCount)
        {
            PlatformApplyState(true);
            _system.DrawPrimitives((Sce.PlayStation4.Graphics.PrimitiveType)primitiveType, vertexStart, vertexCount);
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
