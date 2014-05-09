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

                // Setup default value.
                var size = param.columns * param.rows * 4;
                var data = new byte[size];
                if (elem.DefaultValue != null)
                    Array.Copy(elem.DefaultValue, data, data.Length);

                // Set the data.
                param.data = data;

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
                case ElementType.Float2x1:                     
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.VECTOR;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 1;
                    param.columns = 2;
                    break;

                case ElementType.Float3:
                case ElementType.Float3x1:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.VECTOR;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 1;
                    param.columns = 3;
                    break;

		        case ElementType.Float4:
                case ElementType.Float4x1:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.VECTOR;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 1;
                    param.columns = 4;
                    break;
             
		        case ElementType.Float1x2:
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
                    param.rows = 2;
                    param.columns = 3;
                    break;

		        case ElementType.Float4x2:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.MATRIX_COLUMNS;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 2;
                    param.columns = 4;
                    break;

		        case ElementType.Float1x3:
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
                    param.rows = 2;
                    param.columns = 4;
                    break;

		        case ElementType.Float1x4:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.MATRIX_COLUMNS;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 4;
                    param.columns = 1;
                    break;
       
		        case ElementType.Float2x4:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.MATRIX_COLUMNS;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 4;
                    param.columns = 3;
                    break;

		        case ElementType.Float3x4:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.MATRIX_COLUMNS;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 4;
                    param.columns = 3;
                    break;

		        case ElementType.Float4x4:
                    param.class_ = EffectObject.D3DXPARAMETER_CLASS.MATRIX_COLUMNS;
                    param.type = EffectObject.D3DXPARAMETER_TYPE.FLOAT;
                    param.rows = 4;
                    param.columns = 4;
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
            
            //param.member_count = (uint)type.Description.MemberCount;
            //param.element_count = (uint)type.Description.ElementCount;

            /*
            if (param.member_count > 0)
            {
                param.member_handles = new EffectObject.d3dx_parameter[param.member_count];
                for (var i = 0; i < param.member_count; i++)
                {
                    var mparam = GetParameterFromType(type.GetMemberType(i));
                    mparam.name = type.GetMemberTypeName(i) ?? string.Empty;
                    param.member_handles[i] = mparam;
                }
            }
            else
            {
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
                    mparam.data = new byte[param.columns * param.rows * 4];

                    param.member_handles[i] = mparam;
                }
            }
            */

            return param;
        }
    }
}