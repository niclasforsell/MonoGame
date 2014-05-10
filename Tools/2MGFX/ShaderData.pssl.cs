using System;
using System.Linq;
using System.Collections.Generic;
using Microsoft.Xna.Framework.Graphics;
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

                var sampler = new Sampler
                {
                    // This is the Texture1D/2D/3D/Cube name which
                    // is what ends up being the Effect parameter name.
                    parameterName = desc.Name,

                    // The texture register.
                    textureSlot = desc.Index,

                    // Setup defaults for the sampler.
                    samplerName = string.Empty,
                    samplerSlot = desc.Index,
                };

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

                // Look for the sampler for this texture slot.
                for (var s=0; s < compiler.SamplerCount; s++)
                {
                    var samplerDesc = compiler.GetSamplerDesc(s);

                    // Lookup the sampler state by the texture/parameter name.
                    SamplerStateInfo state;
                    if (    !samplerStates.TryGetValue(samplerDesc.Name, out state) || 
                            state.TextureName != sampler.parameterName)
                        continue;

                    sampler.samplerName = samplerDesc.Name;
                    sampler.samplerSlot = samplerDesc.Index;
                    sampler.state = state.State;
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

            shader._attributes = new Attribute[compiler.AttributeCount];
            for (var i = 0; i < compiler.AttributeCount; i++)
            {
                var attrib = compiler.GetAttributeDesc(i);
                shader._attributes[i].name = attrib.Name;
                shader._attributes[i].index = attrib.SemanticIndex;

                // First try looking up the usage by semantic name.
                switch (attrib.SemanticName.ToLower())
                {
                    case "position":
                        shader._attributes[i].usage = VertexElementUsage.Position;
                        break;
                    case "normal":
                        shader._attributes[i].usage = VertexElementUsage.Normal;
                        break;
                    case "color":
                        shader._attributes[i].usage = VertexElementUsage.Color;
                        break;
                    case "binormal":
                        shader._attributes[i].usage = VertexElementUsage.Binormal;
                        break;
                    case "tangent":
                        shader._attributes[i].usage = VertexElementUsage.Tangent;
                        break;
                    case "texcoord":
                        shader._attributes[i].usage = VertexElementUsage.TextureCoordinate;
                        break;
                    case "fog":
                        shader._attributes[i].usage = VertexElementUsage.Fog;
                        break;
                    case "pointsize":
                        shader._attributes[i].usage = VertexElementUsage.PointSize;
                        break;
                    case "blendindices":
                        shader._attributes[i].usage = VertexElementUsage.BlendIndices;
                        break;
                    case "blendweight":
                        shader._attributes[i].usage = VertexElementUsage.BlendWeight;
                        break;
                    case "depth":
                        shader._attributes[i].usage = VertexElementUsage.Depth;
                        break;
                    case "tessellatefactor":
                        shader._attributes[i].usage = VertexElementUsage.TessellateFactor;
                        break;                        
                    case "sample":
                        shader._attributes[i].usage = VertexElementUsage.Sample;
                        break;                        
                    default:
                    {
                        // Fallback to the actual usage attribute.
                        switch (attrib.Usage)
                        {
		                    case SemanticUsage.Position:
                            case SemanticUsage.SPosition:
                                shader._attributes[i].usage = VertexElementUsage.Position;
                                break;
                            case SemanticUsage.Normal:
                                shader._attributes[i].usage = VertexElementUsage.Normal;
                                break;
		                    case SemanticUsage.Color:
                                shader._attributes[i].usage = VertexElementUsage.Color;
                                break;
                            case SemanticUsage.Binormal:
                                shader._attributes[i].usage = VertexElementUsage.Binormal;
                                break;
                            case SemanticUsage.Tangent:
                                shader._attributes[i].usage = VertexElementUsage.Tangent;
                                break;
                            case SemanticUsage.Texcoord0:
		                    case SemanticUsage.Texcoord1:
		                    case SemanticUsage.Texcoord2:
		                    case SemanticUsage.Texcoord3:
		                    case SemanticUsage.Texcoord4:
		                    case SemanticUsage.Texcoord5:
		                    case SemanticUsage.Texcoord6:
		                    case SemanticUsage.Texcoord7:
		                    case SemanticUsage.Texcoord8:
		                    case SemanticUsage.Texcoord9:
                                shader._attributes[i].usage = VertexElementUsage.TextureCoordinate;
                                break;
                            case SemanticUsage.Fog:
                                shader._attributes[i].usage = VertexElementUsage.Fog;
                                break;
                            case SemanticUsage.Pointsize:
                                shader._attributes[i].usage = VertexElementUsage.PointSize;
                                break;
                            case SemanticUsage.Depth:
                                shader._attributes[i].usage = VertexElementUsage.Depth;
                                break;
                            default:
                                throw new NotSupportedException("Unsupported vertex semantic!");
                        }
                        break;
                    }
                }
            }

            // Strip out all the unneed metadata out of the shader.
            shader.ShaderCode = compiler.Strip();

            return shader;
        }
    }
}