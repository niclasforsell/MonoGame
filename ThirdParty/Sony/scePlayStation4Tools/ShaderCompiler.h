#pragma once

#include <shader/binary.h>

using namespace System;
using namespace System::Runtime::InteropServices;

namespace sce { namespace PlayStation4 { namespace Tools {

	public enum struct BufferType
	{
		DataBuffer,
		Texture1d,
		Texture2d,
		Texture3d,
		TextureCube,
		Texture1dArray,
		Texture2dArray,
		TextureCubeArray,
		MsTexture2d,
		MsTexture2dArray,
		RegularBuffer,
		ByteBuffer,
		RwDataBuffer,
		RwTexture1d,
		RwTexture2d,
		RwTexture3d,
		RwTexture1dArray,
		RwTexture2dArray,
		RwRegularBuffer,
		RwByteBuffer,
		AppendBuffer,
		ConsumeBuffer,
		ConstantBuffer,
		TextureBuffer,
		PointBuffer,
		LineBuffer,
		TriangleBuffer,
		SRTBuffer,
		BufferTypeLast 
	};

	public enum struct ElementType
	{
		Float1,
		Float2,
		Float3,
		Float4,
		Half1,
		Half2,
		Half3,
		Half4,
		Int1,
		Int2,
		Int3,
		Int4,
		Uint1,
		Uint2,
		Uint3,
		Uint4,
		Double1,
		Double2,
		Double3,
		Double4,
		Float1x1,
		Float2x1,
		Float3x1,
		Float4x1,
		Float1x2,
		Float2x2,
		Float3x2,
		Float4x2,
		Float1x3,
		Float2x3,
		Float3x3,
		Float4x3,
		Float1x4,
		Float2x4,
		Float3x4,
		Float4x4,
		Half1x1,
		Half2x1,
		Half3x1,
		Half4x1,
		Half1x2,
		Half2x2,
		Half3x2,
		Half4x2,
		Half1x3,
		Half2x3,
		Half3x3,
		Half4x3,
		Half1x4,
		Half2x4,
		Half3x4,
		Half4x4,
		Int1x1,
		Int2x1,
		Int3x1,
		Int4x1,
		Int1x2,
		Int2x2,
		Int3x2,
		Int4x2,
		Int1x3,
		Int2x3,
		Int3x3,
		Int4x3,
		Int1x4,
		Int2x4,
		Int3x4,
		Int4x4,
		Uint1x1,
		Uint2x1,
		Uint3x1,
		Uint4x1,
		Uint1x2,
		Uint2x2,
		Uint3x2,
		Uint4x2,
		Uint1x3,
		Uint2x3,
		Uint3x3,
		Uint4x3,
		Uint1x4,
		Uint2x4,
		Uint3x4,
		Uint4x4,
		Double1x1,
		Double2x1,
		Double3x1,
		Double4x1,
		Double1x2,
		Double2x2,
		Double3x2,
		Double4x2,
		Double1x3,
		Double2x3,
		Double3x3,
		Double4x3,
		Double1x4,
		Double2x4,
		Double3x4,
		Double4x4,
		Point,
		Line,
		Triangle,
		Adjacentline,
		Adjacenttriangle,
		Patch,
		Structure,
		Long1,
		Long2,
		Long3,
		Long4,
		Ulong1,
		Ulong2,
		Ulong3,
		Ulong4,
		Long1x1,
		Long2x1,
		Long3x1,
		Long4x1,
		Long1x2,
		Long2x2,
		Long3x2,
		Long4x2,
		Long1x3,
		Long2x3,
		Long3x3,
		Long4x3,
		Long1x4,
		Long2x4,
		Long3x4,
		Long4x4,
		Ulong1x1,
		Ulong2x1,
		Ulong3x1,
		Ulong4x1,
		Ulong1x2,
		Ulong2x2,
		Ulong3x2,
		Ulong4x2,
		Ulong1x3,
		Ulong2x3,
		Ulong3x3,
		Ulong4x3,
		Ulong1x4,
		Ulong2x4,
		Ulong3x4,
		Ulong4x4,
		Bool1,
		Bool2,
		Bool3,
		Bool4,
		Texture,
		SamplerState,
		Buffer,
		TypeEnd
	};

	public enum struct SemanticUsage
	{
		Position,
		Normal,
		Color,
		Binormal,
		Tangent,
		Texcoord0,
		Texcoord1,
		Texcoord2,
		Texcoord3,
		Texcoord4,
		Texcoord5,
		Texcoord6,
		Texcoord7,
		Texcoord8,
		Texcoord9,
		TexcoordEnd,
		Implicit,
		Nonreferencable,
		Clip,
		Fog,
		Pointsize,
		Fragcoord,
		Target0,
		Target1,
		Target2,
		Target3,
		Target4,
		Target5,
		Target6,
		Target7,
		Target8,
		Target9,
		Target10,
		Target11,
		Depth,
		Lastcg,
		UserDefined,
		SClipDistance,
		SCullDistance,
		SCoverage,
		SDepthOutput,
		SDispatchthreadId,
		SDomainLocation,
		SGroupId,
		SGroupIndex,
		SGroupThreadId,
		SPosition,
		SVertexId,
		SInstanceId,
		SSampleIndex,
		SPrimitiveId,
		SGsinstanceId,
		SOutputControlPointId,
		SFrontFace,
		SRenderTargetIndex,
		SViewportIndex,
		STargetOutput,
		SEdgeTessFactor,
		SInsideTessFactor,
		SpriteCoord,
		SDepthGEOutput,
		SDepthLEOutput,
		SemanticEnd 
	};


	public ref class ElementDesc
	{
	public:
		String^ Name;
		ElementType Type;
		array<Byte>^ DefaultValue;
		int ArraySize;
		int OffsetInBytes;
		int SizeInBytes;
	};

	public ref class BufferDesc
	{
	internal:
		sce::Shader::Binary::Element* _elements;
		Byte* _defaults;

	public:
		BufferType Type;
		String^ Name;
		int Index;
		int ElementCount;
		int StrideSize;
		
		ElementDesc^ GetElement(int index);
	};

	public ref class SamplerDesc
	{
	public:
		String^ Name;
		int Index;
	};


	public ref class AttributeDesc
	{
	public:
		String^ Name;
		String^ SemanticName;
		int SemanticIndex;
		SemanticUsage Usage;
	};

	public ref class ShaderCompiler
	{
	private:

		sce::Shader::Binary::Program *_program;

		array<Byte>^ _bytecode;

		void _reset();

	public:
		ShaderCompiler();

		bool Compile(String^ code, String^ entryPoint, String^ profile, [Out]String^ %buildOutput);

		bool Reflect(array<Byte>^ bytecode);

		array<Byte>^ Strip();

		BufferDesc^ GetBufferDesc(int index);

		SamplerDesc^ GetSamplerDesc(int index);

		AttributeDesc^ GetAttributeDesc(int index);

		property int AttributeCount
		{
			int get() { return _program->m_numInputAttributes; }
		}

		property int BufferCount
		{
			int get() { return _program->m_numBuffers; }
		}

		property array<Byte>^ Bytecode 
		{ 
			array<Byte>^ get() { return _bytecode; }
		}

	};

}}}