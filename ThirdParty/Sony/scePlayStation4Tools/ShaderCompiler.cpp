#include "stdafx.h"
#include "ShaderCompiler.h"

#include <shader/compiler.h>

using namespace sce::Shader;

namespace sce { namespace PlayStation4 { namespace Tools {

ShaderCompiler::ShaderCompiler()
{
}

bool ShaderCompiler::Compile(String^ code, String^ entryPoint, String^ profile)
{
	Compiler::Options options;
	options.sourceLanguage = Compiler::SourceLanguage::kSourceLanguagePssl;
	auto output = Compiler::run(&options, 0);

	return true;
}

}}}