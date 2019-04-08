#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

typedef struct _Archive
{
  enum {
    BinaryArchive = 0,
    CArchive
  }
    type;

  FILE *output;
  int line_length;
}
  Archive;

/* create and open the archive file and write the file header. */
static int open_archive(Archive *archive, const char *file, const char *var)
{
  if (!archive || !file || !var)
  {
    printf("open_archive: internal error.\n");
    return 0;
  }

  archive->output = fopen(file, "w");
  if (!archive->output)
  {
    perror("open_archive: error opening archive file");
    return 0;
  }

  if (archive->type == CArchive)
    fprintf(archive->output,
      "/* Auto-generated ROM file, created by mkrom. */\n\n"
      "const char %s[] = ", var);

  return 1;
}

/* write the null entry at the end of the archive and close it. */
static void close_archive(Archive *archive)
{
  if (archive->type == CArchive)
    fwrite("\"\\0\\0\\0\\0\\0\";", 13, 1, archive->output);
  else
    fwrite("\0\0\0\0\0", 5, 1, archive->output);
  fclose(archive->output);
}

static int c_encode_file(Archive *archive, int fd, unsigned long long file_size, const char *path, unsigned int path_len, unsigned int prefix_len)
{
  unsigned char buf[512];
  int i, buf_len, last_was_hex;

  /* adjust the path and path_len to ignore the prefix. */
  path += prefix_len;
  path_len -= prefix_len;

  /* adjust the lengths to include the null terminators and write the header. */
  ++file_size;
  ++path_len;
  fprintf(archive->output, "\n/* File: %s (%llu bytes) */", path, file_size);
  fprintf(archive->output, "\n\"\\x%02X\\x%02X\\x%02X\\x%02X\\x%02X\"\"%s\\0\"",
    (unsigned int)(file_size >> 24) & 0x000000FF,
    (unsigned int)(file_size >> 16) & 0x000000FF,
    (unsigned int)(file_size >>  8) & 0x000000FF,
    (unsigned int) file_size        & 0x000000FF,
     path_len & 0x00FF,
     path);

  last_was_hex = 0;
  archive->line_length = 0;
  while ((buf_len = read(fd, buf, sizeof(buf))) > 0)
  {
    for (i = 0; i != buf_len; ++i)
    {
      if (archive->line_length == 0)
      {
        fwrite("\n\"", 2, 1, archive->output);
        archive->line_length = 1;
      }

      if (buf[i] == '\\')
      {
        fwrite("\\\\", 2, 1, archive->output);
        archive->line_length += 2;
        last_was_hex = 0;
      }
      else if (buf[i] == '\t')
      {
        fwrite("\\t", 2, 1, archive->output);
        archive->line_length += 2;
        last_was_hex = 0;
      }
      else if (buf[i] == '"')
      {
        fwrite("\\\"", 2, 1, archive->output);
        archive->line_length += 2;
        last_was_hex = 0;
      }
      else if (isprint(buf[i]))
      {
        if (last_was_hex)
        {
          fwrite("\"\"", 2, 1, archive->output);
          archive->line_length += 2;
        }
        fwrite(buf + i, 1, 1, archive->output);
        ++archive->line_length;
        last_was_hex = 0;
      }
      else
      {
        fprintf(archive->output, "\\x%02X", buf[i]);
        archive->line_length += 4;
        last_was_hex = 1;
      }

      if (archive->line_length >= 79)
      {
        fwrite("\"", 1, 1, archive->output);
        archive->line_length = 0;
        last_was_hex = 0;
      }
    }
  }

  if (buf_len < 0)
  {
    perror("c_encode_file: error reading file");
    return 0;
  }

  /* terminate the file. */
  if (archive->line_length == 0)
    fwrite("\"", 1, 1, archive->output);
  fwrite("\\0\"", 3, 1, archive->output);

  return 1;
}

static int binary_encode_file(Archive *archive, int fd, unsigned long long file_size, const char *path, unsigned int path_len, unsigned int prefix_len)
{
  unsigned char buf[512];
  int buf_len;

  /* adjust the path and path_len to ignore the prefix. */
  path += prefix_len;
  path_len -= prefix_len;

  /* adjust the lengths to include the null terminators and write the header. */
  ++file_size;
  ++path_len;

  buf[0] = (file_size >> 24) & 0x000000FF;
  buf[1] = (file_size >> 16) & 0x000000FF;
  buf[2] = (file_size >>  8) & 0x000000FF;
  buf[3] =  file_size        & 0x000000FF;
  buf[4] =  path_len & 0x00FF;
  fwrite(buf, 5, 1, archive->output);

  fprintf(archive->output, "%s", path);
  buf[0] = 0;
  fwrite(buf, 1, 1, archive->output);

  while ((buf_len = read(fd, buf, sizeof(buf))) > 0)
    fwrite(buf, buf_len, 1, archive->output);

  if (buf_len < 0)
  {
    perror("binary_encode_file: error reading file");
    return 0;
  }

  /* terminate the file. */
  buf[0] = 0;
  fwrite(buf, 1, 1, archive->output);

  return 1;
}

static int archive_file(Archive *archive, char *path, int prefix_len)
{
  struct stat st;
  int fd;

  fd = open(path, O_RDONLY);
  if (fd == -1)
  {
    perror("archive_file: error opening file");
    return 0;
  }

  if (fstat(fd, &st) == -1)
  {
    perror("archive_file: unable to stat file");
    return 0;
  }

  if (st.st_size > 0x0FFFFFFFFUL)
  {
    printf("archive_file: file %s too big\n", path);
    return 0;
  }

  printf("Archiving file: %s as %s (%llu bytes)\n", path, path + prefix_len, (unsigned long long)st.st_size);
  if (archive->type == CArchive)
    c_encode_file(archive, fd, st.st_size, path, strlen(path), prefix_len);
  else
    binary_encode_file(archive, fd, st.st_size, path, strlen(path), prefix_len);

  close(fd);

  return 1;
}

static int archive_dir(Archive *archive, char *root, unsigned int prefix_len)
{
  char path[PATH_MAX];
  size_t path_len;
  DIR *dir;
  struct dirent *ent;
  int ok;

  ok = 1;
  dir = opendir(root);
  if (!dir)
  {
    printf("Error: unable to open directory %s\n", root);
    ok = 0;
  }
  printf("Archiving directory: %s\n", root);

  while (ok && (ent = readdir(dir)))
  {
    if ((strcmp(ent->d_name, ".") == 0) || (strcmp(ent->d_name, "..") == 0))
      continue;

    /* generate the path of the directory entry and archive it. */
    path_len = snprintf(path, sizeof(path), "%s/%s", root, ent->d_name);
    if (path_len >= sizeof(path))
    {
      printf("Error: path of %s/%s too long.\n", root, ent->d_name);
      ok = 0;
    }
    else if (ent->d_type == DT_DIR)
      ok = archive_dir(archive, path, prefix_len);
    else if (ent->d_type == DT_REG)
      ok = archive_file(archive, path, prefix_len);
  }

  if (dir)
    closedir(dir);

  return ok;
}

int main(int argc, char **argv)
{
  Archive archive;
  unsigned int dir_len, prefix_len;

  if (argc != 5 && argc != 6)
  {
    printf("%s <bin | c> <source_dir> <var_name> <output_file> [strip_prefix]\nArchive the contents of source_dir as a rom file and encode it "
        "as a raw binary file, or as a C source file containing a constant array var_name.\n", argv[0]);
    return 1;
  }

  archive.type = BinaryArchive;
  if (argv[1][0] == 'c')
    archive.type = CArchive;

  dir_len = strlen(argv[2]);
  prefix_len = 0;
  if (argc == 6)
  {
    prefix_len = strlen(argv[5]);
    if (prefix_len > dir_len)
    {
      printf("Error: prefix (%s) is longer than source directory(%s)\n", argv[5], argv[2]);
      return 1;
    }
  }

  if (!open_archive(&archive, argv[4], argv[3]))
    return 1;

  /* trim a trailing separator from the directory name. */
  if (argv[2][dir_len - 1] == '/')
    argv[2][dir_len - 1] = 0;

  archive_dir(&archive, argv[2], prefix_len);
  close_archive(&archive);
}

