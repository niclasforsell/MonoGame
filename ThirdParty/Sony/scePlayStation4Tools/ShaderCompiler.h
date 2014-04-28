#pragma once

using namespace System;

namespace sce { namespace PlayStation4 { namespace Tools {

	public ref class ShaderCompiler
	{
	private:

	public:
		ShaderCompiler();

		bool Compile(String^ code, String^ entryPoint, String^ profile);

		property array<Byte>^ Bytecode 
		{ 
			array<Byte>^ get() { return nullptr; }
		}

	};

}}}