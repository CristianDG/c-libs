#include "osso.h"

// NOTE: temporary
#include <stdio.h>
#include <sys/stat.h>
#include <sys/syscall.h>

long syscall(long number, ...);

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


char *osso_path_cstring_read_entire_file(DG_Arena *a, char *path, usize *size)
{
  char *result = 0;

  //TODO: error checking
  FILE *file = fopen(path, "r");

  result = osso_fd_read_entire_file(a, file, size);

  fclose(file);

  return result;
}

void osso_path_cstring_time_get(char *path, OSSO_Time *created, OSSO_Time *modified, OSSO_Time *accessed)
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

OSSO_Path osso_self_path(DG_Arena *arena)
{
  OSSO_Path result = {0};
  const u32 buf_size = 1024;
  char *buf = dg_arena_alloc_arr(arena, char, buf_size);
  syscall(SYS_readlink, "/proc/self/exe", buf, buf_size);

  result.path.data = (u8*) buf;

  // FIXME: strlen?
  result.path.len = CLAMP_TOP(buf_size, strlen(buf));

  return result;
}

b32 osso_path_is_folder(OSSO_Path path)
{
  b32 result = 0;

  DG_Scratch scratch = dg_scratch_get(0);

  struct stat s;
  stat(string8_copy_to_cstring(scratch.arena, path.path), &s);

  const u32 is_file_flag = 0170000;
  const u32 is_directory_flag = 0040000;

  if ((s.st_mode & is_file_flag) == is_directory_flag) {
    result = true;
  }

  dg_scratch_release(scratch);

  return result;
}

OSSO_Path osso_self_folder_path(DG_Arena *arena)
{
  OSSO_Path result = {0};
  DG_Scratch scratch = dg_scratch_get(arena);

  OSSO_Path self_path = osso_self_path(scratch.arena);

  for (u32 i = self_path.path.len - 1; i >= 0; --i) {
    u8 c = self_path.path.data[i];
    if (c == '/') {
      result.path = string8_copy(arena, string8_substring_alias(self_path.path, 0, i));
      break;
    }
  }

  dg_scratch_release(scratch);
  return result;
}

OSSO_Path osso_self_calling_location_path(DG_Arena *arena)
{
  OSSO_Path result = {0};
  DG_Scratch scratch = dg_scratch_get(arena);

  char *buf = dg_arena_alloc_arr(arena, char, 1024);

  syscall(SYS_getcwd, buf, 1024);
  result.path = string8_copy(arena, string8_from_cstring_clamped(buf, 1024));

  dg_scratch_release(scratch);

  return result;
}

