// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Sce.PlayStation4.Graphics;

namespace Microsoft.Xna.Framework.Graphics
{
    public partial class GraphicsDevice
    {
        internal GraphicsSystem _system;

        private readonly Dictionary<int, DynamicVertexBuffer> _userVertexBuffers = new Dictionary<int, DynamicVertexBuffer>();
        private readonly Dictionary<IndexElementSize, DynamicIndexBuffer> _userIndexBuffers = new Dictionary<IndexElementSize, DynamicIndexBuffer>();

        private readonly Dictionary<ulong, FetchShader> _fetchShaders = new Dictionary<ulong, FetchShader>();

        private void PlatformSetup()
        {
            MaxTextureSlots = 16;
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
            PlatformSetViewport(ref _viewport);
            _depthStencilStateDirty = true;
            _blendStateDirty = true;
            _pixelShaderDirty = true;
            _vertexShaderDirty = true;
            _rasterizerStateDirty = true;
            _scissorRectangleDirty = true;
        }

        private void PlatformDispose()
        {
            foreach (var fetch in _fetchShaders)
                fetch.Value.Dispose();
            _fetchShaders.Clear();

            _system.Dispose();
            _system = null;
        }

        private void PlatformPresent()
        {
            _system.Present();
            
            // Restore the viewport after present.
            _viewport = new Viewport(0, 0, PresentationParameters.BackBufferWidth, PresentationParameters.BackBufferHeight);

            // Reapply all the render states.
            Textures.Dirty();
            SamplerStates.Dirty();
            _depthStencilStateDirty = true;
            _blendStateDirty = true;
            _indexBufferDirty = true;
            _vertexBufferDirty = true;
            _pixelShaderDirty = true;
            _vertexShaderDirty = true;
            _rasterizerStateDirty = true;
            _scissorRectangleDirty = true;
        }

        private void PlatformSetViewport(ref Viewport viewport)
        {
            _system.SetViewport(viewport.X, viewport.Y, viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth);
        }

        private void PlatformApplyDefaultRenderTarget()
        {
            _system.SetRenderTargets(null, null, null, null);
        }

        private IRenderTarget PlatformApplyRenderTargets()
        {
            var target0 = _currentRenderTargetBindings[0].RenderTarget as RenderTarget2D;
            var target1 = _currentRenderTargetBindings[1].RenderTarget as RenderTarget2D;
            var target2 = _currentRenderTargetBindings[2].RenderTarget as RenderTarget2D;
            var target3 = _currentRenderTargetBindings[3].RenderTarget as RenderTarget2D;
            _system.SetRenderTargets(target0._target,
                                    target1 != null ? target1._target : null,
                                    target2 != null ? target2._target : null,
                                    target3 != null ? target3._target : null);

            return target0;
        }

        private FetchShader GetFetchShader(Shader shader, VertexDeclaration decl)
        {
            FetchShader fetch;

            // Lookup the fetch shader using the shader and declaration as the key.
            var key = (ulong)decl.HashKey << 32 | (uint)shader.HashKey;
            if (!_fetchShaders.TryGetValue(key, out fetch))
            {
                var remap = shader.GenerateFetchRemap(decl);
                unsafe
                {
                    fixed (uint* ptr = remap)
                    {
                        fetch = new FetchShader(shader._vertexShader, ptr, remap.Length);
                    }
                }
                _fetchShaders.Add(key, fetch);
            }

            return fetch;
        }

        internal void PlatformApplyState(bool applyShaders)
        {
            if (_scissorRectangleDirty)
            {
                _system.SetScissorRectangle(_scissorRectangle.Left, _scissorRectangle.Top,
                                            _scissorRectangle.Right, _scissorRectangle.Bottom);
                _scissorRectangleDirty = false;
            }
            if (_blendStateDirty)
            {
                _blendState.PlatformApplyState(this);
                _blendStateDirty = false;
            }
            if (_depthStencilStateDirty)
            {
                _depthStencilState.PlatformApplyState(this);
                _depthStencilStateDirty = false;
            }
            if (_rasterizerStateDirty)
            {
                _rasterizerState.PlatformApplyState(this);
                _rasterizerStateDirty = false;
            }

            // If we're not applying shaders then early out now.
            if (!applyShaders)
                return;

            if (_vertexBufferDirty)
            {
                if (_vertexBuffer != null)
                    _system.SetVertexBuffer(_vertexBuffer._buffer);
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

            if (_vertexShaderDirty || _vertexBufferDirty)
            {
                var fetch = GetFetchShader(_vertexShader, _vertexBuffer.VertexDeclaration);
                _system.SetVertexShader(_vertexShader._vertexShader, fetch);
                _vertexShaderDirty = _vertexBufferDirty = false;
            }

            if (_pixelShaderDirty)
            {
                _system.SetPixelShader(_pixelShader._pixelShader);
                _pixelShaderDirty = false;
            }

            _vertexConstantBuffers.SetConstantBuffers(this);
            _pixelConstantBuffers.SetConstantBuffers(this);

            Textures.SetTextures(this);
            SamplerStates.PlatformSetSamplers(this);
        }

        private void PlatformDrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int startIndex, int primitiveCount)
        {
            PlatformApplyState(true);
            _system.DrawIndexedPrimitives((Sce.PlayStation4.Graphics.PrimitiveType)primitiveType, baseVertex, startIndex, primitiveCount);
        }

        private void PlatformDrawPrimitives(PrimitiveType primitiveType, int vertexStart, int vertexCount)
        {
            PlatformApplyState(true);
            _system.DrawPrimitives((Sce.PlayStation4.Graphics.PrimitiveType)primitiveType, vertexStart, vertexCount);
        }

        private int SetUserVertexBuffer<T>(T[] vertexData, int vertexOffset, int vertexCount, VertexDeclaration vertexDecl)
            where T : struct
        {
            DynamicVertexBuffer buffer;

            if (!_userVertexBuffers.TryGetValue(vertexDecl.HashKey, out buffer) || buffer.VertexCount < vertexCount)
            {
                // Dispose the previous buffer if we have one.
                if (buffer != null)
                    buffer.Dispose();

                buffer = new DynamicVertexBuffer(this, vertexDecl, Math.Max(vertexCount, 2000), BufferUsage.WriteOnly);
                _userVertexBuffers[vertexDecl.HashKey] = buffer;
            }

            var startVertex = buffer.UserOffset;


            if ((vertexCount + buffer.UserOffset) < buffer.VertexCount)
            {
                buffer.UserOffset += vertexCount;
                buffer.SetData(startVertex * vertexDecl.VertexStride, vertexData, vertexOffset, vertexCount, vertexDecl.VertexStride, SetDataOptions.NoOverwrite);
            }
            else
            {
                buffer.UserOffset = vertexCount;
                buffer.SetData(vertexData, vertexOffset, vertexCount, SetDataOptions.Discard);
                startVertex = 0;
            }

            SetVertexBuffer(buffer);

            return startVertex;
        }

        private int SetUserIndexBuffer<T>(T[] indexData, int indexOffset, int indexCount)
            where T : struct
        {
            DynamicIndexBuffer buffer;

            var indexType = typeof(T);
            var indexSize = Marshal.SizeOf(indexType);
            var indexElementSize = indexSize == 2 ? IndexElementSize.SixteenBits : IndexElementSize.ThirtyTwoBits;

            if (!_userIndexBuffers.TryGetValue(indexElementSize, out buffer) || buffer.IndexCount < indexCount)
            {
                if (buffer != null)
                    buffer.Dispose();

                buffer = new DynamicIndexBuffer(this, indexElementSize, Math.Max(indexCount, 6000), BufferUsage.WriteOnly);
                _userIndexBuffers[indexElementSize] = buffer;
            }

            var startIndex = buffer.UserOffset;

            if ((indexCount + buffer.UserOffset) < buffer.IndexCount)
            {
                buffer.UserOffset += indexCount;
                buffer.SetData(startIndex * indexSize, indexData, indexOffset, indexCount, SetDataOptions.NoOverwrite);
            }
            else
            {
                startIndex = 0;
                buffer.UserOffset = indexCount;
                buffer.SetData(indexData, indexOffset, indexCount, SetDataOptions.Discard);
            }

            Indices = buffer;

            return startIndex;
        }

        private void PlatformDrawUserPrimitives<T>(PrimitiveType primitiveType, T[] vertexData, int vertexOffset, VertexDeclaration vertexDeclaration, int vertexCount) where T : struct
        {
            var startVertex = SetUserVertexBuffer(vertexData, vertexOffset, vertexCount, vertexDeclaration);

            PlatformApplyState(true);

            _system.DrawPrimitives((Sce.PlayStation4.Graphics.PrimitiveType)primitiveType, startVertex, vertexCount);
        }

        private void PlatformDrawUserIndexedPrimitives<T>(PrimitiveType primitiveType, T[] vertexData, int vertexOffset, int numVertices, short[] indexData, int indexOffset, int primitiveCount, VertexDeclaration vertexDeclaration) where T : struct
        {
            var indexCount = GetElementCountArray(primitiveType, primitiveCount);
            var startVertex = SetUserVertexBuffer(vertexData, vertexOffset, numVertices, vertexDeclaration);
            var startIndex = SetUserIndexBuffer(indexData, indexOffset, indexCount);

            PlatformApplyState(true);

            _system.DrawIndexedPrimitives((Sce.PlayStation4.Graphics.PrimitiveType)primitiveType, startVertex, startIndex, primitiveCount);
        }

        private void PlatformDrawUserIndexedPrimitives<T>(PrimitiveType primitiveType, T[] vertexData, int vertexOffset, int numVertices, int[] indexData, int indexOffset, int primitiveCount, VertexDeclaration vertexDeclaration) where T : struct, IVertexType
        {
            var indexCount = GetElementCountArray(primitiveType, primitiveCount);
            var startVertex = SetUserVertexBuffer(vertexData, vertexOffset, numVertices, vertexDeclaration);
            var startIndex = SetUserIndexBuffer(indexData, indexOffset, indexCount);

            PlatformApplyState(true);

            _system.DrawIndexedPrimitives((Sce.PlayStation4.Graphics.PrimitiveType)primitiveType, startVertex, startIndex, primitiveCount);
        }

        private static GraphicsProfile PlatformGetHighestSupportedGraphicsProfile(GraphicsDevice graphicsDevice)
        {
            return GraphicsProfile.HiDef;
        }
    }
}
