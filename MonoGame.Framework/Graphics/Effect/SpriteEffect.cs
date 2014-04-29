#region File Description
//-----------------------------------------------------------------------------
// SpriteEffect.cs
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
    /// The default effect used by SpriteBatch.
    /// </summary>
    public class SpriteEffect : Effect
    {
        #region Effect Parameters

        EffectParameter matrixParam;

        #endregion

        static internal readonly byte[] Bytecode = LoadEffectResource(
#if DIRECTX
            "Microsoft.Xna.Framework.Graphics.Effect.Resources.SpriteEffect.dx11.mgfxo"
#elif PSM
            "MonoGame.Framework.PSMobile.PSSuite.Graphics.Resources.SpriteEffect.cgx" //FIXME: This shader is totally incomplete
#elif OPENGL
            "Microsoft.Xna.Framework.Graphics.Effect.Resources.SpriteEffect.ogl.mgfxo"
#elif PLAYSTATION4
            "Microsoft.Xna.Framework.Graphics.Effect.Resources.SpriteEffect.ps4.mgfxo"
#endif
        );

        #region Methods

        /// <summary>
        /// Creates a new SpriteEffect.
        /// </summary>
        public SpriteEffect(GraphicsDevice device)
            : base(device, Bytecode)
        {
            CacheEffectParameters();
        }

        /// <summary>
        /// Creates a new SpriteEffect by cloning parameter settings from an existing instance.
        /// </summary>
        protected SpriteEffect(SpriteEffect cloneSource)
            : base(cloneSource)
        {
            CacheEffectParameters();
        }


        /// <summary>
        /// Creates a clone of the current SpriteEffect instance.
        /// </summary>
        public override Effect Clone()
        {
            return new SpriteEffect(this);
        }


        /// <summary>
        /// Looks up shortcut references to our effect parameters.
        /// </summary>
        void CacheEffectParameters()
        {
            matrixParam = Parameters["MatrixTransform"];
        }

        /// <summary>
        /// Lazily computes derived parameter values immediately before applying the effect.
        /// </summary>
        protected internal override bool OnApply()
        {
            var viewport = GraphicsDevice.Viewport;

            var projection = Matrix.CreateOrthographicOffCenter(0, viewport.Width, viewport.Height, 0, 0, 1);
            var halfPixelOffset = Matrix.CreateTranslation(-0.5f, -0.5f, 0);

            matrixParam.SetValue(halfPixelOffset * projection);

            return false;
        }


        #endregion

#if PLAYSTATION4

        static byte[] GenerateSpriteEffect()
        {
            var stream = new System.IO.MemoryStream();
            var writer = new System.IO.BinaryWriter(stream);

            writer.Write("MGFX".ToCharArray());
            writer.Write((byte)5); // version
            writer.Write((byte)2); // profile

            // Constant Buffers
            writer.Write((byte)1);
            {
                // SpriteEffect_vv
                writer.Write((short)(4 * 4 * 4)); // size in bytes
                writer.Write((byte)1); // parameter count
                {
                    writer.Write((byte)1); // parameter index
                    writer.Write((ushort)0); // parameter offset
                }
            }

            // Shaders
            writer.Write((byte)2);
            {
                writer.Write(true); // is vertex shader
                var SpriteEffect_vv = LoadEffectResource("Microsoft.Xna.Framework.Graphics.Effect.Resources.SpriteEffect_vv.sb");
                writer.Write((int)SpriteEffect_vv.Length); // shader length
                writer.Write(SpriteEffect_vv);
                writer.Write((byte)0); // sampler count
                writer.Write((byte)1); // constant buffer count
                {
                    writer.Write((byte)0); // constant buffer index
                }

                writer.Write(false); // is vertex shader
                var SpriteEffect_p = LoadEffectResource("Microsoft.Xna.Framework.Graphics.Effect.Resources.SpriteEffect_p.sb");
                writer.Write((int)SpriteEffect_p.Length); // shader length
                writer.Write(SpriteEffect_p);
                writer.Write((byte)1); // sampler count
                {
                    writer.Write((byte)SamplerType.Sampler2D);
                    writer.Write((byte)0); // texture slot
                    writer.Write((byte)0); // sampler slot
                    writer.Write(false); // sampler state
                    writer.Write((byte)0); // parameter index
                }
                writer.Write((byte)0); // constant buffer count
            }

            // Parameters
            writer.Write((byte)2);
            {
                writer.Write((byte)EffectParameterClass.Object);
                writer.Write((byte)EffectParameterType.Texture2D);
                writer.Write("Texture");
                writer.Write(""); // semantic
                writer.Write((byte)0); // annotations
                writer.Write((byte)0); // rowCount
                writer.Write((byte)0); // columnCount
                writer.Write((byte)0); // elements
                writer.Write((byte)0); // struct members

                writer.Write((byte)EffectParameterClass.Matrix);
                writer.Write((byte)EffectParameterType.Single);
                writer.Write("MatrixTransform");
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
            }

            // Techniques
            writer.Write((byte)1);
            {
                writer.Write("SpriteBatch");
                writer.Write((byte)0); // annotations
                writer.Write((byte)1); // passes
                writer.Write("");
                writer.Write((byte)0); // annotations
                writer.Write((byte)0); // vertex shader index
                writer.Write((byte)1); // pixel shader index
                writer.Write(false); // blend state
                writer.Write(false); // depth stencil state
                writer.Write(false); // rasterizer state
            }

            return stream.ToArray();
        }
#endif
    }
}
