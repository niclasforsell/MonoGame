#include "pixelShader.h"

#include "..\allocator.h"

#include <sdk_version.h>
#include <gnm.h>
#include <gnmx.h>
#include <gnmx/shader_parser.h>
#include <assert.h>

using namespace sce;
using namespace sce::Gnmx;
using namespace Graphics;


PixelShader::PixelShader(const void *data)
{
	Gnmx::ShaderInfo shaderInfo;
#if SCE_ORBIS_SDK_VERSION >= 0x02000071u // SDK Version 2.0
	Gnmx::parseShader(&shaderInfo, data);
#else
	Gnmx::parseShader(&shaderInfo, data, Gnmx::kPixelShader);
#endif

	_binary = mem::allocShared(shaderInfo.m_gpuShaderCodeSize, Gnm::kAlignmentOfShaderInBytes);
	auto shaderHeader = mem::alloc(shaderInfo.m_psShader->computeSize(), Gnm::kAlignmentOfBufferInBytes);

	memcpy(_binary, shaderInfo.m_gpuShaderCode, shaderInfo.m_gpuShaderCodeSize);
	memcpy(shaderHeader, shaderInfo.m_psShader, shaderInfo.m_psShader->computeSize());

	_shader = static_cast<Gnmx::PsShader*>(shaderHeader);
	_shader->patchShaderGpuAddress(_binary);
}

PixelShader::~PixelShader()
{
	mem::freeShared(_binary);
	mem::free(_shader);
}
