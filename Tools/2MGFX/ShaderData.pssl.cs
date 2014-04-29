using System;
using System.Collections.Generic;
using sce.PlayStation4.Tools;

namespace TwoMGFX
{
    internal partial class ShaderData
    {
        public static ShaderData CreatePSSL(byte[] byteCode, bool isVertexShader, List<ConstantBufferData> cbuffers, int sharedIndex, Dictionary<string, SamplerStateInfo> samplerStates, bool debug)
        {
            // This is only part of the private PS4 repository.

            var shader = new ShaderData();
            shader.IsVertexShader = isVertexShader;
            shader.SharedIndex = sharedIndex;
            shader.Bytecode = (byte[])byteCode.Clone();

            // Setup the shader reflection.
            var compiler = new ShaderCompiler();
            compiler.Reflect(byteCode);
             
            // Find the samplers.
            var samplers = new List<Sampler>();
            for (var i = 0; i < compiler.BufferCount; i++)
            {
                var desc = compiler.GetBufferDesc(i);
                if (    desc.Type != BufferType.Texture1d &&
                        desc.Type != BufferType.Texture2d &&
                        desc.Type != BufferType.Texture3d &&
                        desc.Type != BufferType.TextureCube)
                    continue;

                var samplerDesc = compiler.GetSamplerDesc(desc.Index);

                var sampler = new Sampler
                {
                    textureSlot = desc.Index,
                    parameterName = desc.Name,
                    samplerName = samplerDesc.Name,
                    samplerSlot = samplerDesc.Index,
                };

                SamplerStateInfo state;
                if (samplerStates.TryGetValue(sampler.samplerName, out state))
                    sampler.state = state.State;
                
                switch (desc.Type)
                {
                    case BufferType.Texture1d:
                    //case BufferType.Texture1dArray:
                        sampler.type = MojoShader.MOJOSHADER_samplerType.MOJOSHADER_SAMPLER_1D;
                        break;

                    case BufferType.Texture2d:
                    //case ShaderResourceViewDimension.Texture2DArray:
                    //case ShaderResourceViewDimension.Texture2DMultisampled:
                    //case ShaderResourceViewDimension.Texture2DMultisampledArray:
                        sampler.type = MojoShader.MOJOSHADER_samplerType.MOJOSHADER_SAMPLER_2D;
                        break;

                    case BufferType.Texture3d:
                        sampler.type = MojoShader.MOJOSHADER_samplerType.MOJOSHADER_SAMPLER_VOLUME;
                        break;

                    case BufferType.TextureCube:
                    //case ShaderResourceViewDimension.TextureCubeArray:
                        sampler.type = MojoShader.MOJOSHADER_samplerType.MOJOSHADER_SAMPLER_CUBE;
                        break;
                }

                samplers.Add(sampler);
            }
            shader._samplers = samplers.ToArray();

            // Gather all the constant buffers used by this shader.
            var cbIndex = new List<int>();
            for (var i = 0; i < compiler.BufferCount; i++)
            {
                var desc = compiler.GetBufferDesc(i);
                if (desc.Type != BufferType.ConstantBuffer)
                    continue;

                var cb = new ConstantBufferData(desc);

                // Look for a duplicate cbuffer in the list.
                for (var c = 0; c < cbuffers.Count; c++)
                {
                    if (cb.SameAs(cbuffers[c]))
                    {
                        cb = null;
                        cbIndex.Add(c);
                        break;
                    }
                }

                // Add a new cbuffer.
                if (cb != null)
                {
                    cbIndex.Add(cbuffers.Count);
                    cbuffers.Add(cb);
                }
            }
            shader._cbuffers = cbIndex.ToArray();

            // Strip out all the unneed metadata out of the shader.
            shader.ShaderCode = compiler.Strip();

            return shader;
        }
    }
}