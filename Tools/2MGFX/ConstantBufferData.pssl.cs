using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using sce.PlayStation4.Tools;

namespace TwoMGFX
{
    internal partial class ConstantBufferData
    {
        public ConstantBufferData(BufferDesc cb)
        {
            Name = cb.Name ?? string.Empty;            
            Size = cb.StrideSize;

            ParameterIndex = new List<int>();

            var parameters = new List<EffectObject.d3dx_parameter>();

            // Gather all the parameters.
            for (var i = 0; i < cb.ElementCount; i++)
            {
                var elem = cb.GetElement(i);

                var param = GetParameterElem(elem);
                parameters.Add(param);
            }

            // Sort them by the offset for some consistent results.
            Parameters = parameters.OrderBy(e => e.bufferOffset).ToList();

            // Store the parameter offsets.
            ParameterOffset = new List<int>();
            foreach (var param in Parameters)
                ParameterOffset.Add(param.bufferOffset);
        }

        private static EffectObject.d3dx_parameter GetParameterElem(ElementDesc elem)
        {
            var param = new EffectObject.d3dx_parameter();

            param.name = elem.Name ?? string.Empty;
            param.semantic = string.Empty;
            param.bufferOffset = elem.OffsetInBytes;

            switch (elem.Type)
            {
                default:
                    throw new Exception("Unsupported parameter type!");

                case ElementType.Float1:
                case ElementType.Float1x1:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.SCALAR;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 1;
                    param.columns = 1;
                    break;

                case ElementType.Float2: 
		        case ElementType.Float1x2:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.VECTOR;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 1;
                    param.columns = 2;
                    break;

                case ElementType.Float3:
		        case ElementType.Float1x3:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.VECTOR;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 1;
                    param.columns = 3;
                    break;

		        case ElementType.Float4:
		        case ElementType.Float1x4:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.VECTOR;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 1;
                    param.columns = 4;
                    break;
             
                case ElementType.Float2x1:                     
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.MATRIX_COLUMNS;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 2;
                    param.columns = 1;
                    break;
       
		        case ElementType.Float2x2:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.MATRIX_COLUMNS;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 2;
                    param.columns = 2;
                    break;

		        case ElementType.Float3x2:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.MATRIX_COLUMNS;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 3;
                    param.columns = 2;
                    break;

		        case ElementType.Float4x2:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.MATRIX_COLUMNS;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 4;
                    param.columns = 2;
                    break;

                case ElementType.Float3x1:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.MATRIX_COLUMNS;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 3;
                    param.columns = 1;
                    break;
       
		        case ElementType.Float2x3:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.MATRIX_COLUMNS;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 2;
                    param.columns = 3;
                    break;

		        case ElementType.Float3x3:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.MATRIX_COLUMNS;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 3;
                    param.columns = 3;
                    break;

		        case ElementType.Float4x3:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.MATRIX_COLUMNS;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 4;
                    param.columns = 3;
                    break;

                case ElementType.Float4x1:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.MATRIX_COLUMNS;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 4;
                    param.columns = 1;
                    break;
       
		        case ElementType.Float2x4:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.MATRIX_COLUMNS;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 2;
                    param.columns = 4;
                    break;

		        case ElementType.Float3x4:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.MATRIX_COLUMNS;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 3;
                    param.columns = 4;
                    break;

		        case ElementType.Float4x4:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.MATRIX_COLUMNS;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 4;
                    param.columns = 4;
                    break;

                case ElementType.Int1:
                case ElementType.Uint1:
                case ElementType.Int1x1:
                case ElementType.Uint1x1:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.SCALAR;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.INT;
                    param.rows = 1;
                    param.columns = 1;
                    break;

                    /*
		                Int1,
		                Int1x1,
		                Uint1x1,

                        Int2,
		                Int2x1,
		                Uint2x1,

                        Int3,
		                Int3x1,
		                Uint3x1,

                        Int4,
		                Int4x1,
		                Uint4x1,

		                //Structure,

		                Bool1,
		                Bool2,
		                Bool3,
		                Bool4,

		                //Texture,
		                //SamplerState,
                     */
            }

            param.member_count = 0; // (uint)type.Description.MemberCount;
            param.element_count = (uint)elem.ArraySize;

            if (param.member_count > 0)
            {
                throw new NotImplementedException("Struct uniforms are not implemented!");
            }
            else if (param.element_count > 0)
            {
                var defaultOffset = 0;

                param.member_handles = new EffectObject.d3dx_parameter[param.element_count];
                for (var i = 0; i < param.element_count; i++)
                {
                    var mparam = new EffectObject.d3dx_parameter();

                    mparam.name = string.Empty;
                    mparam.semantic = string.Empty;
                    mparam.type = param.type;
                    mparam.class_ = param.class_;
                    mparam.rows = param.rows;
                    mparam.columns = param.columns;

                    var size = (int)(((param.rows-1) * 16) + (param.columns * 4));
                    var data = new byte[size];

                    if (elem.DefaultValue != null)
                        Array.Copy(elem.DefaultValue, defaultOffset, data, 0, size);

                    defaultOffset += (int)param.rows * 16;
                    mparam.data = data;
                    param.member_handles[i] = mparam;
                }
            }
            else
            {
                var size = param.columns * param.rows * 4;
                var data = new byte[size];

                // Set the default value.
                if (elem.DefaultValue != null)
                    Array.Copy(elem.DefaultValue, data, size);

                // Set the data.
                param.data = data;
            }

            return param;
        }
    }
}