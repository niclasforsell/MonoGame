#include "renderTarget.h"

#include "..\allocator.h"
#include <gnm.h>

using namespace Graphics;


RenderTarget::RenderTarget(sce::Gnm::RenderTarget *renderTarget)
{
	_renderTarget = renderTarget;

	_texture = new sce::Gnm::Texture();
	_texture->initFromRenderTarget(_renderTarget, false);
	_texture->setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);
}

RenderTarget::~RenderTarget()
{
	Allocator::Get()->release(_renderTarget->getBaseAddress());
	delete _renderTarget;
	delete _texture;
}

uint32_t RenderTarget::getWidth()
{ 
	return _renderTarget->getWidth();
}

uint32_t RenderTarget::getHeight() 
{ 
	return _renderTarget->getHeight();
}
