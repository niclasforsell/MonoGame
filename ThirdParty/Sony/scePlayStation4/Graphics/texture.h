#pragma once

#include "predecls.h"

namespace Graphics {

class __declspec(dllexport) Texture
{
	friend class GraphicsSystem;

private:

	sce::Gnm::Texture *_texture;

	// No default or copy constructors.
	Texture() { }
	Texture( const Texture & ) { }

protected:
	Texture(sce::Gnm::Texture *texture);

public:
	virtual ~Texture();

	void SetData(uint32_t level, unsigned char *data, uint32_t bytes);

};

} // namespace Graphics