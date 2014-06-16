#include "pixelShader.h"

#include "..\allocator.h"

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
	Gnmx::parseShader(&shaderInfo, data, Gnmx::kPixelShader);

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
