/* romfs.c ROM file access functions. */
#include <string.h>
#include "romfs.h"

/* find and return a pointer to the null-terminated string containing the contents
 * of the file matching the given path.
 * return zero if the file is not found.
 */
const char* extract_rom_file(const char *romfs, const char *path)
{
  unsigned long file_size, offset;
  unsigned int path_len;
  const char *content;

  if (!romfs || !path)
    return 0;

  for (content = 0, file_size = 1, offset = 0;
      file_size != 0 && !content;
      offset += file_size + path_len)
  {
    file_size = (unsigned char)romfs[offset++];
    file_size <<= 8;
    file_size += (unsigned char)romfs[offset++];
    file_size <<= 8;
    file_size += (unsigned char)romfs[offset++];
    file_size <<= 8;
    file_size += (unsigned char)romfs[offset++];
    path_len = (unsigned char)romfs[offset++];
    if (path_len && strcmp(romfs + offset, path) == 0)
      content = romfs + offset + path_len;
  }

  return content;
}

