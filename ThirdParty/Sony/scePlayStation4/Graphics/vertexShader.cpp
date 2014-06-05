#include "vertexShader.h"

#include "..\allocator.h"

#include <gnm.h>
#include <gnmx.h>
#include <gnmx/shader_parser.h>
#include <assert.h>

using namespace sce;
using namespace sce::Gnmx;
using namespace Graphics;


VertexShader::VertexShader(const void *data)
{
	Gnmx::ShaderInfo shaderInfo;
	Gnmx::parseShader(&shaderInfo, data, Gnmx::kVertexShader);

	_binary = Allocator::Get()->allocate(shaderInfo.m_gpuShaderCodeSize, Gnm::kAlignmentOfShaderInBytes, SCE_KERNEL_WC_GARLIC);
	auto shaderHeader = Allocator::Get()->allocate(shaderInfo.m_vsShader->computeSize(), Gnm::kAlignmentOfBufferInBytes);

	memcpy(_binary, shaderInfo.m_gpuShaderCode, shaderInfo.m_gpuShaderCodeSize);
	memcpy(shaderHeader, shaderInfo.m_vsShader, shaderInfo.m_vsShader->computeSize());

	_shader = static_cast<Gnmx::VsShader*>(shaderHeader);
	_shader->patchShaderGpuAddress(_binary);
}

VertexShader::~VertexShader()
{
	Allocator::Get()->release(_binary);
	Allocator::Get()->release(_shader);
}
