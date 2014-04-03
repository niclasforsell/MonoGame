/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2012 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <assert.h>
#include "file.h"

using namespace Media;

File::File(const char *path, const char *mode) : m_fp(0), m_path(0), m_size(0)
{
	if (path && mode) {
		open(path, mode);
	}
}

int File::open(const char *path, const char *mode)
{
	assert(path);
	assert(mode);
	assert(!m_fp);
	assert(!m_path);

	// open a file
	m_fp = fopen(path, mode);
	if (!m_fp) {
		printf("error: fopen() failed: %s\n", path);
		return -1;
	}

	// get the file size
	fseek(m_fp, 0, SEEK_END);
	m_size = (uint32_t)ftell(m_fp);
	fseek(m_fp, 0, SEEK_SET);
	return 0;
}

File::~File(void)
{
	close();
}

int File::close(void)
{
	if (m_fp) {
		// close a file
		fclose(m_fp);
		m_fp = 0;
	}

	return 0;
}

int File::read(void *p, uint32_t size, off_t offset, int32_t whence)
{
	int ret = 0;

	assert(p);
	assert(m_fp);

	if (offset || whence != SEEK_CUR) {
		// seek a specified position
		ret = fseek(m_fp, offset, whence);
		if (ret) {
			printf("error: fseek() failed: 0x%08X\n", ret);
			goto term;
		}
	}

	{
		// read the file
		const size_t readsize = fread(p, 1, size, m_fp);
		if (size < readsize) {
			ret = -1;
			goto term;
		}
		ret = (int)readsize;
	}

term:
	return ret;
}

int File::write(const void *p, uint32_t size, off_t offset, int32_t whence)
{
	int ret = 0;

	assert(p);
	assert(m_fp);

	if (offset || whence != SEEK_CUR) {
		// seek a specified position
		ret = fseek(m_fp, offset, whence);
		if (ret) {
			printf("error: fseek() failed: 0x%08X\n", ret);
			goto term;
		}
	}

	{
		// write the file
		const size_t writesize = fwrite(p, 1, size, m_fp);
		if (size < writesize) {
			ret = -1;
			goto term;
		}
		ret = writesize;
	}

term:
	return ret;
}
