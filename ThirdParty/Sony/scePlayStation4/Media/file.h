/* SCE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 01.600.051
 * Copyright (C) 2012 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef _FILE_H
#define _FILE_H

#include <stdint.h>
#include <stdio.h>

namespace Media {

class File
{
public:
	File(const char *path = 0, const char *mode = 0);
	~File(void);
	int open(const char *path, const char *mode);
	int close(void);
	int read(void *p, uint32_t size, off_t offset = 0, int32_t whence = SEEK_CUR);
	int write(const void *p, uint32_t size, off_t offset = 0, int32_t whence = SEEK_CUR);
	uint32_t size(void) { return m_size; }
	const char *name(void) { return m_path; }
private:
	FILE *m_fp;
	const char *m_path;
	uint32_t m_size;
};

} // namespace Media

#endif /* _FILE_H */
