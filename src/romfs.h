/* romfs.h ROM file access functions. */
#ifndef ROMFS_H
#define ROMFS_H

/* find and return a pointer to the null-terminated string containing the contents
 * of the file matching the given path.
 * return zero if the file is not found.
 */
const char* extract_rom_file(const char *romfs, const char *path);

#endif

