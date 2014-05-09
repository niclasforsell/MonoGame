#include "fetchShader.h"

#include "vertexShader.h"
#include "..\allocator.h"

#include <gnm.h>
#include <gnmx.h>
#include <gnmx/shader_parser.h>
#include <assert.h>

using namespace sce;
using namespace sce::Gnmx;
using namespace Graphics;


FetchShader::FetchShader(VertexShader *shader)
{
	_fsMem = Allocator::Get()->allocate(
		Gnmx::computeVsFetchShaderSize(shader->_shader),
		Gnm::kAlignmentOfFetchShaderInBytes,
		SCE_KERNEL_WC_GARLIC);

	Gnmx::generateVsFetchShader(_fsMem, &_shaderModifier, shader->_shader, NULL);
}

FetchShader::~FetchShader()
{
	Allocator::Get()->release(_fsMem);
}
