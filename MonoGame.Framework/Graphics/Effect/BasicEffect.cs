#region File Description
//-----------------------------------------------------------------------------
// BasicEffect.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#endregion

#region Using Statements
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
#endregion

namespace Microsoft.Xna.Framework.Graphics
{
    /// <summary>
    /// Built-in effect that supports optional texturing, vertex coloring, fog, and lighting.
    /// </summary>
    public class BasicEffect : Effect, IEffectMatrices, IEffectLights, IEffectFog
    {
        #region Effect Parameters

        EffectParameter textureParam;
        EffectParameter diffuseColorParam;
        EffectParameter emissiveColorParam;
        EffectParameter specularColorParam;
        EffectParameter specularPowerParam;
        EffectParameter eyePositionParam;
        EffectParameter fogColorParam;
        EffectParameter fogVectorParam;
        EffectParameter worldParam;
        EffectParameter worldInverseTransposeParam;
        EffectParameter worldViewProjParam;

        int _shaderIndex = -1;

        #endregion

        #region Fields

        bool lightingEnabled;
        bool preferPerPixelLighting;
        bool oneLight;
        bool fogEnabled;
        bool textureEnabled;
        bool vertexColorEnabled;

        Matrix world = Matrix.Identity;
        Matrix view = Matrix.Identity;
        Matrix projection = Matrix.Identity;

        Matrix worldView;

        Vector3 diffuseColor = Vector3.One;
        Vector3 emissiveColor = Vector3.Zero;
        Vector3 ambientLightColor = Vector3.Zero;

        float alpha = 1;

        DirectionalLight light0;
        DirectionalLight light1;
        DirectionalLight light2;

        float fogStart = 0;
        float fogEnd = 1;

        EffectDirtyFlags dirtyFlags = EffectDirtyFlags.All;

#if PLAYSTATION4
        static readonly byte[] Bytecode = GenerateBasicEffect();
#else
        static readonly byte[] Bytecode = LoadEffectResource(
#if DIRECTX
            "Microsoft.Xna.Framework.Graphics.Effect.Resources.BasicEffect.dx11.mgfxo"
#elif PSM 
            "MonoGame.Framework.PSMobile.PSSuite.Graphics.Resources.BasicEffect.cgx" //FIXME: This shader is totally incomplete
#elif OPENGL
            "Microsoft.Xna.Framework.Graphics.Effect.Resources.BasicEffect.ogl.mgfxo"
#endif
        );
#endif

        #endregion
        
        #region Public Properties


        /// <summary>
        /// Gets or sets the world matrix.
        /// </summary>
        public Matrix World
        {
            get { return world; }
            
            set
            {
                world = value;
                dirtyFlags |= EffectDirtyFlags.World | EffectDirtyFlags.WorldViewProj | EffectDirtyFlags.Fog;
            }
        }


        /// <summary>
        /// Gets or sets the view matrix.
        /// </summary>
        public Matrix View
        {
            get { return view; }
            
            set
            {
                view = value;
                dirtyFlags |= EffectDirtyFlags.WorldViewProj | EffectDirtyFlags.EyePosition | EffectDirtyFlags.Fog;
            }
        }


        /// <summary>
        /// Gets or sets the projection matrix.
        /// </summary>
        public Matrix Projection
        {
            get { return projection; }
            
            set
            {
                projection = value;
                dirtyFlags |= EffectDirtyFlags.WorldViewProj;
            }
        }


        /// <summary>
        /// Gets or sets the material diffuse color (range 0 to 1).
        /// </summary>
        public Vector3 DiffuseColor
        {
            get { return diffuseColor; }
            
            set
            {
                diffuseColor = value;
                dirtyFlags |= EffectDirtyFlags.MaterialColor;
            }
        }


        /// <summary>
        /// Gets or sets the material emissive color (range 0 to 1).
        /// </summary>
        public Vector3 EmissiveColor
        {
            get { return emissiveColor; }
            
            set
            {
                emissiveColor = value;
                dirtyFlags |= EffectDirtyFlags.MaterialColor;
            }
        }


        /// <summary>
        /// Gets or sets the material specular color (range 0 to 1).
        /// </summary>
        public Vector3 SpecularColor
        {
            get { return specularColorParam.GetValueVector3(); }
            set { specularColorParam.SetValue(value); }
        }


        /// <summary>
        /// Gets or sets the material specular power.
        /// </summary>
        public float SpecularPower
        {
            get { return specularPowerParam.GetValueSingle(); }
            set { specularPowerParam.SetValue(value); }
        }


        /// <summary>
        /// Gets or sets the material alpha.
        /// </summary>
        public float Alpha
        {
            get { return alpha; }
            
            set
            {
                alpha = value;
                dirtyFlags |= EffectDirtyFlags.MaterialColor;
            }
        }


        /// <summary>
        /// Gets or sets the lighting enable flag.
        /// </summary>
        public bool LightingEnabled
        {
            get { return lightingEnabled; }
            
            set
            {
                if (lightingEnabled != value)
                {
                    lightingEnabled = value;
                    dirtyFlags |= EffectDirtyFlags.ShaderIndex | EffectDirtyFlags.MaterialColor;
                }
            }
        }


        /// <summary>
        /// Gets or sets the per-pixel lighting prefer flag.
        /// </summary>
        public bool PreferPerPixelLighting
        {
            get { return preferPerPixelLighting; }
            
            set
            {
                if (preferPerPixelLighting != value)
                {
                    preferPerPixelLighting = value;
                    dirtyFlags |= EffectDirtyFlags.ShaderIndex;
                }
            }
        }


        /// <summary>
        /// Gets or sets the ambient light color (range 0 to 1).
        /// </summary>
        public Vector3 AmbientLightColor
        {
            get { return ambientLightColor; }
            
            set
            {
                ambientLightColor = value;
                dirtyFlags |= EffectDirtyFlags.MaterialColor;
            }
        }


        /// <summary>
        /// Gets the first directional light.
        /// </summary>
        public DirectionalLight DirectionalLight0 { get { return light0; } }


        /// <summary>
        /// Gets the second directional light.
        /// </summary>
        public DirectionalLight DirectionalLight1 { get { return light1; } }


        /// <summary>
        /// Gets the third directional light.
        /// </summary>
        public DirectionalLight DirectionalLight2 { get { return light2; } }


        /// <summary>
        /// Gets or sets the fog enable flag.
        /// </summary>
        public bool FogEnabled
        {
            get { return fogEnabled; }
            
            set
            {
                if (fogEnabled != value)
                {
                    fogEnabled = value;
                    dirtyFlags |= EffectDirtyFlags.ShaderIndex | EffectDirtyFlags.FogEnable;
                }
            }
        }


        /// <summary>
        /// Gets or sets the fog start distance.
        /// </summary>
        public float FogStart
        {
            get { return fogStart; }
            
            set
            {
                fogStart = value;
                dirtyFlags |= EffectDirtyFlags.Fog;
            }
        }


        /// <summary>
        /// Gets or sets the fog end distance.
        /// </summary>
        public float FogEnd
        {
            get { return fogEnd; }
            
            set
            {
                fogEnd = value;
                dirtyFlags |= EffectDirtyFlags.Fog;
            }
        }


        /// <summary>
        /// Gets or sets the fog color.
        /// </summary>
        public Vector3 FogColor
        {
            get { return fogColorParam.GetValueVector3(); }
            set { fogColorParam.SetValue(value); }
        }


        /// <summary>
        /// Gets or sets whether texturing is enabled.
        /// </summary>
        public bool TextureEnabled
        {
            get { return textureEnabled; }
            
            set
            {
                if (textureEnabled != value)
                {
                    textureEnabled = value;
                    dirtyFlags |= EffectDirtyFlags.ShaderIndex;
                }
            }
        }


        /// <summary>
        /// Gets or sets the current texture.
        /// </summary>
        public Texture2D Texture
        {
            get { return textureParam.GetValueTexture2D(); }
            set { textureParam.SetValue(value); }
        }


        /// <summary>
        /// Gets or sets whether vertex color is enabled.
        /// </summary>
        public bool VertexColorEnabled
        {
            get { return vertexColorEnabled; }
            
            set
            {
                if (vertexColorEnabled != value)
                {
                    vertexColorEnabled = value;
                    dirtyFlags |= EffectDirtyFlags.ShaderIndex;
                }
            }
        }


        #endregion

        #region Methods

        /// <summary>
        /// Creates a new BasicEffect with default parameter settings.
        /// </summary>
        public BasicEffect(GraphicsDevice device)
            : base(device, Bytecode)
        {
            CacheEffectParameters(null);

            DirectionalLight0.Enabled = true;
            SpecularColor = Vector3.One;
            SpecularPower = 16;
        }

        /// <summary>
        /// Creates a new BasicEffect by cloning parameter settings from an existing instance.
        /// </summary>
        protected BasicEffect(BasicEffect cloneSource)
            : base(cloneSource)
        {
            CacheEffectParameters(cloneSource);

            lightingEnabled = cloneSource.lightingEnabled;
            preferPerPixelLighting = cloneSource.preferPerPixelLighting;
            fogEnabled = cloneSource.fogEnabled;            
            textureEnabled = cloneSource.textureEnabled;
            vertexColorEnabled = cloneSource.vertexColorEnabled;

            world = cloneSource.world;
            view = cloneSource.view;
            projection = cloneSource.projection;

            diffuseColor = cloneSource.diffuseColor;
            emissiveColor = cloneSource.emissiveColor;
            ambientLightColor = cloneSource.ambientLightColor;

            alpha = cloneSource.alpha;

            fogStart = cloneSource.fogStart;
            fogEnd = cloneSource.fogEnd;
        }


        /// <summary>
        /// Creates a clone of the current BasicEffect instance.
        /// </summary>
        public override Effect Clone()
        {
            return new BasicEffect(this);
        }


        /// <summary>
        /// Sets up the standard key/fill/back lighting rig.
        /// </summary>
        public void EnableDefaultLighting()
        {
            LightingEnabled = true;

            AmbientLightColor = EffectHelpers.EnableDefaultLighting(light0, light1, light2);
        }


        /// <summary>
        /// Looks up shortcut references to our effect parameters.
        /// </summary>
        void CacheEffectParameters(BasicEffect cloneSource)
        {
            #if !PSM
                textureParam                = Parameters["Texture"];
            #else
                textureParam                = Parameters["Texture0"];
            #endif
            diffuseColorParam           = Parameters["DiffuseColor"];
            emissiveColorParam          = Parameters["EmissiveColor"];
            specularColorParam          = Parameters["SpecularColor"];
            specularPowerParam          = Parameters["SpecularPower"];
            eyePositionParam            = Parameters["EyePosition"];
            fogColorParam               = Parameters["FogColor"];
            fogVectorParam              = Parameters["FogVector"];
            worldParam                  = Parameters["World"];
            worldInverseTransposeParam  = Parameters["WorldInverseTranspose"];
            worldViewProjParam          = Parameters["WorldViewProj"];

            light0 = new DirectionalLight(Parameters["DirLight0Direction"],
                                          Parameters["DirLight0DiffuseColor"],
                                          Parameters["DirLight0SpecularColor"],
                                          (cloneSource != null) ? cloneSource.light0 : null);

            light1 = new DirectionalLight(Parameters["DirLight1Direction"],
                                          Parameters["DirLight1DiffuseColor"],
                                          Parameters["DirLight1SpecularColor"],
                                          (cloneSource != null) ? cloneSource.light1 : null);

            light2 = new DirectionalLight(Parameters["DirLight2Direction"],
                                          Parameters["DirLight2DiffuseColor"],
                                          Parameters["DirLight2SpecularColor"],
                                          (cloneSource != null) ? cloneSource.light2 : null);
        }


        /// <summary>
        /// Lazily computes derived parameter values immediately before applying the effect.
        /// </summary>
        protected internal override bool OnApply()
        {
            // Recompute the world+view+projection matrix or fog vector?
            dirtyFlags = EffectHelpers.SetWorldViewProjAndFog(dirtyFlags, ref world, ref view, ref projection, ref worldView, fogEnabled, fogStart, fogEnd, worldViewProjParam, fogVectorParam);
            
            // Recompute the diffuse/emissive/alpha material color parameters?
            if ((dirtyFlags & EffectDirtyFlags.MaterialColor) != 0)
            {
                EffectHelpers.SetMaterialColor(lightingEnabled, alpha, ref diffuseColor, ref emissiveColor, ref ambientLightColor, diffuseColorParam, emissiveColorParam);

                dirtyFlags &= ~EffectDirtyFlags.MaterialColor;
            }

            if (lightingEnabled)
            {
                // Recompute the world inverse transpose and eye position?
                dirtyFlags = EffectHelpers.SetLightingMatrices(dirtyFlags, ref world, ref view, worldParam, worldInverseTransposeParam, eyePositionParam);

                
                // Check if we can use the only-bother-with-the-first-light shader optimization.
                bool newOneLight = !light1.Enabled && !light2.Enabled;
                
                if (oneLight != newOneLight)
                {
                    oneLight = newOneLight;
                    dirtyFlags |= EffectDirtyFlags.ShaderIndex;
                }
            }

            // Recompute the shader index?
            if ((dirtyFlags & EffectDirtyFlags.ShaderIndex) != 0)
            {
                int shaderIndex = 0;
                
                if (!fogEnabled)
                    shaderIndex += 1;
                
                if (vertexColorEnabled)
                    shaderIndex += 2;
                
                if (textureEnabled)
                    shaderIndex += 4;

                if (lightingEnabled)
                {
                    if (preferPerPixelLighting)
                        shaderIndex += 24;
                    else if (oneLight)
                        shaderIndex += 16;
                    else
                        shaderIndex += 8;
                }

                dirtyFlags &= ~EffectDirtyFlags.ShaderIndex;
#if PSM
#warning Major hack as PSM Shaders don't support multiple Techinques (yet)
                shaderIndex = 0;
#endif

                if (_shaderIndex != shaderIndex)
                {
                    _shaderIndex = shaderIndex;
                    CurrentTechnique = Techniques[_shaderIndex];
                    return true;
                }
            }

            return false;
        }


        #endregion

#if PLAYSTATION4

        static byte[] GenerateBasicEffect()
        {
            var stream = new System.IO.MemoryStream();
            var writer = new System.IO.BinaryWriter(stream);

            writer.Write("MGFX".ToCharArray());
            writer.Write((byte)5); // version
            writer.Write((byte)2); // profile

            // Constant Buffers
            writer.Write((byte)1);
            {
                // VSBasicNoFog
                writer.Write((short)(5 * 4 * 4)); // size in bytes
                writer.Write((byte)2); // parameter count
                {
                    writer.Write((byte)3); // parameter index
                    writer.Write((ushort)0); // parameter offset
                    writer.Write((byte)4); // parameter index
                    writer.Write((ushort)(4*4)); // parameter offset
                }

                /*
                // PSBasicNoFog
                writer.Write((short)(4 * 4)); // size in bytes
                writer.Write((byte)1); // parameter count
                {
                    writer.Write((byte)3); // parameter index
                    writer.Write((ushort)0); // parameter offset
                }
                */
            }

            // Shaders
            writer.Write((byte)5);
            {
                writer.Write(true); // is vertex shader
                var VSBasicNoFog = LoadEffectResource("Microsoft.Xna.Framework.Graphics.Effect.Resources.BasicEffect_VSBasicNoFog.sb");
                writer.Write((int)VSBasicNoFog.Length); // shader length
                writer.Write(VSBasicNoFog);
                writer.Write((byte)0); // sampler count
                writer.Write((byte)1); // constant buffer count
                {
                    writer.Write((byte)0); // constant buffer index
                }

                writer.Write(false); // is vertex shader
                var PSBasicNoFog = LoadEffectResource("Microsoft.Xna.Framework.Graphics.Effect.Resources.BasicEffect_PSBasicNoFog.sb");
                writer.Write((int)PSBasicNoFog.Length); // shader length
                writer.Write(PSBasicNoFog);
                writer.Write((byte)0); // sampler count
                writer.Write((byte)0); // constant buffer count

                writer.Write(true); // is vertex shader
                var VSBasicVcNoFog = LoadEffectResource("Microsoft.Xna.Framework.Graphics.Effect.Resources.BasicEffect_VSBasicVcNoFog.sb");
                writer.Write((int)VSBasicVcNoFog.Length); // shader length
                writer.Write(VSBasicVcNoFog);
                writer.Write((byte)0); // sampler count
                writer.Write((byte)1); // constant buffer count
                {
                    writer.Write((byte)0); // constant buffer index
                }

                writer.Write(true); // is vertex shader
                var VSBasicTxNoFog = LoadEffectResource("Microsoft.Xna.Framework.Graphics.Effect.Resources.BasicEffect_VSBasicTxNoFog.sb");
                writer.Write((int)VSBasicTxNoFog.Length); // shader length
                writer.Write(VSBasicTxNoFog);
                writer.Write((byte)0); // sampler count
                writer.Write((byte)1); // constant buffer count
                {
                    writer.Write((byte)0); // constant buffer index
                }

                writer.Write(false); // is vertex shader
                var PSBasicTxNoFog = LoadEffectResource("Microsoft.Xna.Framework.Graphics.Effect.Resources.BasicEffect_PSBasicTxNoFog.sb");
                writer.Write((int)PSBasicTxNoFog.Length); // shader length
                writer.Write(PSBasicTxNoFog);
                writer.Write((byte)1); // sampler count
                {
                    writer.Write((byte)SamplerType.Sampler2D);
                    writer.Write((byte)0); // texture slot
                    writer.Write((byte)0); // sampler slot
                    writer.Write(false); // sampler state
                    writer.Write((byte)5); // parameter index
                }
                writer.Write((byte)0); // constant buffer count
            }

            // Parameters
            writer.Write((byte)6);
            {
                writer.Write((byte)EffectParameterClass.Vector);
                writer.Write((byte)EffectParameterType.Single);
                writer.Write("SpecularColor");
                writer.Write(""); // semantic
                writer.Write((byte)0); // annotations
                writer.Write((byte)1); // rowCount
                writer.Write((byte)3); // columnCount
                writer.Write((byte)0); // elements
                writer.Write((byte)0); // struct members
                writer.Write((float)0); writer.Write((float)0); writer.Write((float)0); // data

                writer.Write((byte)EffectParameterClass.Vector);
                writer.Write((byte)EffectParameterType.Single);
                writer.Write("SpecularPower");
                writer.Write(""); // semantic
                writer.Write((byte)0); // annotations
                writer.Write((byte)1); // rowCount
                writer.Write((byte)1); // columnCount
                writer.Write((byte)0); // elements
                writer.Write((byte)0); // struct members
                writer.Write((float)0); // data

                writer.Write((byte)EffectParameterClass.Vector);
                writer.Write((byte)EffectParameterType.Single);
                writer.Write("FogVector");
                writer.Write(""); // semantic
                writer.Write((byte)0); // annotations
                writer.Write((byte)1); // rowCount
                writer.Write((byte)4); // columnCount
                writer.Write((byte)0); // elements
                writer.Write((byte)0); // struct members
                writer.Write((float)0); writer.Write((float)0); writer.Write((float)0); writer.Write((float)0);

                writer.Write((byte)EffectParameterClass.Vector);
                writer.Write((byte)EffectParameterType.Single);
                writer.Write("DiffuseColor");
                writer.Write(""); // semantic
                writer.Write((byte)0); // annotations
                writer.Write((byte)1); // rowCount
                writer.Write((byte)4); // columnCount
                writer.Write((byte)0); // elements
                writer.Write((byte)0); // struct members
                writer.Write((float)0); writer.Write((float)0); writer.Write((float)0); writer.Write((float)0);

                writer.Write((byte)EffectParameterClass.Matrix);
                writer.Write((byte)EffectParameterType.Single);
                writer.Write("WorldViewProj");
                writer.Write(""); // semantic
                writer.Write((byte)0); // annotations
                writer.Write((byte)4); // rowCount
                writer.Write((byte)4); // columnCount
                writer.Write((byte)0); // elements
                writer.Write((byte)0); // struct members
                writer.Write((float)0); writer.Write((float)0); writer.Write((float)0); writer.Write((float)0);
                writer.Write((float)0); writer.Write((float)0); writer.Write((float)0); writer.Write((float)0);
                writer.Write((float)0); writer.Write((float)0); writer.Write((float)0); writer.Write((float)0);
                writer.Write((float)0); writer.Write((float)0); writer.Write((float)0); writer.Write((float)0);

                writer.Write((byte)EffectParameterClass.Object);
                writer.Write((byte)EffectParameterType.Texture2D);
                writer.Write("Texture");
                writer.Write(""); // semantic
                writer.Write((byte)0); // annotations
                writer.Write((byte)0); // rowCount
                writer.Write((byte)0); // columnCount
                writer.Write((byte)0); // elements
                writer.Write((byte)0); // struct members
            }

            // Techniques
            writer.Write((byte)6);
            {
                writer.Write("BasicEffect");
                writer.Write((byte)0); // annotations
                writer.Write((byte)1); // passes
                writer.Write("");
                writer.Write((byte)0); // annotations
                writer.Write((byte)255); // vertex shader index
                writer.Write((byte)255); // pixel shader index
                writer.Write(false); // blend state
                writer.Write(false); // depth stencil state
                writer.Write(false); // rasterizer state

                writer.Write("BasicEffect_NoFog");
                writer.Write((byte)0); // annotations
                writer.Write((byte)1); // passes
                writer.Write("");
                writer.Write((byte)0); // annotations
                writer.Write((byte)0); // vertex shader index
                writer.Write((byte)1); // pixel shader index
                writer.Write(false); // blend state
                writer.Write(false); // depth stencil state
                writer.Write(false); // rasterizer state

                writer.Write("BasicEffect_VertexColor");
                writer.Write((byte)0); // annotations
                writer.Write((byte)1); // passes
                writer.Write("");
                writer.Write((byte)0); // annotations
                writer.Write((byte)255); // vertex shader index
                writer.Write((byte)255); // pixel shader index
                writer.Write(false); // blend state
                writer.Write(false); // depth stencil state
                writer.Write(false); // rasterizer state

                writer.Write("BasicEffect_VertexColor_NoFog");
                writer.Write((byte)0); // annotations
                writer.Write((byte)1); // passes
                writer.Write("");
                writer.Write((byte)0); // annotations
                writer.Write((byte)2); // vertex shader index
                writer.Write((byte)1); // pixel shader index
                writer.Write(false); // blend state
                writer.Write(false); // depth stencil state
                writer.Write(false); // rasterizer state

                writer.Write("BasicEffect_Texture");
                writer.Write((byte)0); // annotations
                writer.Write((byte)1); // passes
                writer.Write("");
                writer.Write((byte)0); // annotations
                writer.Write((byte)255); // vertex shader index
                writer.Write((byte)255); // pixel shader index
                writer.Write(false); // blend state
                writer.Write(false); // depth stencil state
                writer.Write(false); // rasterizer state                

                writer.Write("BasicEffect_Texture_NoFog");
                writer.Write((byte)0); // annotations
                writer.Write((byte)1); // passes
                writer.Write("");
                writer.Write((byte)0); // annotations
                writer.Write((byte)3); // vertex shader index
                writer.Write((byte)4); // pixel shader index
                writer.Write(false); // blend state
                writer.Write(false); // depth stencil state
                writer.Write(false); // rasterizer state   
            }

            return stream.ToArray();
        }
#endif
    }
}
