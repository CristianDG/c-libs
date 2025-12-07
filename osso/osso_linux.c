#include "osso.h"

// NOTE: temporary
#include <stdio.h>
#include <sys/stat.h>

char *osso_fd_read_entire_file(DG_Arena *a, FILE *file, usize *size)
{
  char *result = 0;

  fseek(file, 0L, SEEK_END);
  usize file_size = ftell(file);
  fseek(file, 0L, SEEK_SET);

  //TODO: error checking
  result = dg_arena_alloc_size(a, sizeof(char) * (file_size + 1));

  //TODO: error checking
  usize bytes_read = fread(result, sizeof(char), file_size, file);

  DG_ASSERT(bytes_read <= file_size);

  if (size) {
    *size = bytes_read;
  }
  result[bytes_read] = 0;

  return result;
}


char *osso_path_read_entire_file(DG_Arena *a, char *path, usize *size)
{
  char *result = 0;

  //TODO: error checking
  FILE *file = fopen(path, "r");

  result = osso_fd_read_entire_file(a, file, size);

  fclose(file);

  return result;
}

void osso_path_time_get(char *path, OSSO_Time *created, OSSO_Time *modified, OSSO_Time *accessed)
{
  struct stat s;
  stat(path, &s);

  if (created) {
    DG_WIP;
  }

  if (modified) {
    DG_MEMSET(modified, 0, sizeof(*modified));
    modified->time = s.st_mtime;
  }

  if (accessed) {
    DG_MEMSET(accessed, 0, sizeof(*accessed));
    accessed->time = s.st_atime;
  }

}

// IMPORTANT: osso_time_diff!
// TODO: osso_time_earlier_than?
// TODO: osso_time_later_than?
bool osso_time_later_than(OSSO_Time a, OSSO_Time b)
{
  return a.time > b.time;
}

void *osso_platform_specific_lib_load(char *path);
void osso_platform_specific_lib_unload(void *handle);
void *osso_platform_specific_lib_function_load(void *handle, char *function_name);
// TODO: return error
void osso_path_file_copy(char *from_path, char *to_path);
// TODO: return error
void osso_path_file_delete(char *path);

// =========== specific to platform ===========

