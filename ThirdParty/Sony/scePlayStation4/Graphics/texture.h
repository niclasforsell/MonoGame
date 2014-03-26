#pragma once

#include "predecls.h"


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

	void SetData(unsigned char *data, uint32_t bytes);

	uint32_t getWidth();
	uint32_t getHeight();
};

