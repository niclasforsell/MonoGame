#include "stdafx.h"
#include "ShaderCompiler.h"

#include <shader/compiler.h>
#include <shader/binary.h>
#include <shader/shader_parser.h>


using namespace System::Runtime::InteropServices;

using namespace sce::Shader;
using namespace sce::Shader::Binary;

namespace sce { namespace PlayStation4 { namespace Tools {

ShaderCompiler::ShaderCompiler()
{
	_bytecode = nullptr;
	_program = NULL;
}

void ShaderCompiler::_reset()
{
	if (_bytecode != nullptr)
	{
		delete _bytecode;
		_bytecode = nullptr;
	}

	if (_program != NULL)
	{
		delete _program;
		_program = NULL;
	}
}

Compiler::SourceFile* OpenFileCallback(
	const char *fileName,
	const Compiler::SourceLocation *includedFrom,
	const Compiler::Options *compileOptions,
	const char **errorString)
{
	auto sourceFile = (Compiler::SourceFile*)compileOptions->userData;
	return sourceFile;
}

bool ShaderCompiler::Compile(String^ code, String^ entryPoint, String^ profile)
{
	_reset();

	auto fileName = "main_source_file.fx";

	Compiler::Options options;
	Compiler::initializeOptions(&options);

	if (profile->Equals("sce_vs_vs_orbis", System::StringComparison::InvariantCultureIgnoreCase))
		options.targetProfile = Compiler::TargetProfile::kTargetProfileVsVs;
	else if (profile->Equals("sce_ps_orbis", System::StringComparison::InvariantCultureIgnoreCase))
		options.targetProfile = Compiler::TargetProfile::kTargetProfilePs;
	else
	{
		// Throw and exception?
	}
	
	options.entryFunctionName = (const char*)(Marshal::StringToHGlobalAnsi(entryPoint).ToPointer());
	options.mainSourceFile = fileName;
	options.sourceLanguage = Compiler::SourceLanguage::kSourceLanguagePssl;

	Compiler::SourceFile sourceFile;
	sourceFile.fileName = fileName;
	sourceFile.text = (const char*)(Marshal::StringToHGlobalAnsi(code).ToPointer());
	sourceFile.size = _mbstrlen(sourceFile.text);
	options.userData = &sourceFile;

	Compiler::CallbackList callbacks;
	Compiler::initializeCallbackList(&callbacks, Compiler::CallbackDefaults::kCallbackDefaultsTrivial);
	callbacks.openFile = OpenFileCallback;

	auto output = Compiler::run(&options, &callbacks);
	if (output == NULL) 
	{
		// Can i throw an exception?
		return false;
	}

	for (auto i=0; i < output->diagnosticCount; i++)
	{
		auto message = output->diagnostics[i].message;
		auto level = output->diagnostics[i].level;
		auto code = output->diagnostics[i].code;
		auto location = output->diagnostics[i].location;
	}

	if (output->programData == NULL || output->programSize == 0)
	{
		// Throw an exception.
		return false;
	}

	_bytecode = gcnew array<Byte>(output->programSize);
	Marshal::Copy((IntPtr)(void*)output->programData, _bytecode, 0, output->programSize);

	Compiler::destroyOutput(output);

	return true;
}

bool ShaderCompiler::Reflect(array<Byte>^ bytecode)
{
	_reset();

	_bytecode = bytecode;
	pin_ptr<System::Byte> pinned = &_bytecode[0];

	_program = new Program();
	auto status = _program->loadFromMemory((void*)pinned, _bytecode->Length);

	if (status != PsslStatus::kStatusOk)
		return false;
	
	return true;
}

BufferDesc^ ShaderCompiler::GetBufferDesc(int index)
{
	auto psBuffer = _program->m_buffers+index;
	auto name = psBuffer->getName();

	auto buffer = gcnew BufferDesc();
	buffer->Type = (BufferType)psBuffer->m_langType;
	buffer->Name = gcnew String(name);
	buffer->Index = psBuffer->m_resourceIndex;
	buffer->ElementCount = psBuffer->m_numElements;
	buffer->StrideSize = psBuffer->m_strideSize;

	Element *elements = NULL;
	if (psBuffer->m_numElements > 0)
		elements = _program->m_elements + psBuffer->m_elementOffset;
	buffer->_elements = elements;

	return buffer;
}

SamplerDesc^ ShaderCompiler::GetSamplerDesc(int index)
{
	if (_program == NULL || index < 0 || index >= _program->m_numSamplerStates)
		return nullptr;

	auto psSampler = _program->m_samplerStates + index;
	auto stringTable = (const char*)(_program->m_stringTable);
	auto name = psSampler->getName();

	auto sampler = gcnew SamplerDesc();
	sampler->Name = gcnew String(name);
	sampler->Index = psSampler->m_resourceIndex;

	return sampler;
}

ElementDesc^ BufferDesc::GetElement(int index)
{
	if (_elements == NULL || index < 0 || index >= ElementCount)
		return nullptr;

	auto psElement = ((Element*)_elements)+index;
	auto name = psElement->getName();
	auto typeName = psElement->getTypeName();

	auto element = gcnew ElementDesc();
	element->Name = gcnew String(name);
	element->Type = (ElementType)psElement->m_type;
	element->OffsetInBytes = psElement->m_byteOffset;
	element->SizeInBytes = psElement->m_size;

	return element;
}

array<Byte>^ ShaderCompiler::Strip()
{
	if (_program == NULL)
		return nullptr;

	// TODO: Remove stuff we don't need at runtime.

	auto size = _program->calculateSize();
	array<Byte>^ stripped = gcnew array<Byte>(size);
	pin_ptr<System::Byte> pinned = &stripped[0];
	_program->saveToMemory((void*)pinned, size);

	return stripped;
}

}}}