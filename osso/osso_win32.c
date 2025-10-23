#include <libcdg/libcdg.h>

// NOTE: temporary
#include <stdlib.h>

#include <sys/stat.h>

#if DG_OS_WINDOWS
// FIXME: colocar em cima
# include <windows.h>
#elif DG_OS_LINUX
// FIXME: remover
void fopen_s(FILE **file, char *path, char *mode){
  *file = fopen(file, path, mode);
}
#endif

char *osso_fd_read_entire_file(DG_Arena *a, FILE *file, usize *size)
{
  char *result = 0;

  fseek(file, 0L, SEEK_END);
  usize file_size = ftell(file);
  fseek(file, 0L, SEEK_SET);

  //TODO: error checking
  result = dg_arena_alloc(a, sizeof(char) * (file_size + 1));

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

  FILE *file;
  //TODO: error checking
  fopen_s(&file, path, "r");

  result = osso_fd_read_entire_file(a, file, size);

  fclose(file);

  return result;
}

// TODO: make multi platform
typedef struct {
  time_t time;
} OSSO_Time;

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

typedef struct {
  void *handle;
  OSSO_Time time_since_last_load;
  bool _swap;
} OSSO_Hot_Reloaded_Library;

void *osso_platform_specific_lib_load(char *path);
void osso_platform_specific_lib_unload(void *handle);
void *osso_platform_specific_lib_function_load(void *handle, char *function_name);
// TODO: return error
void osso_path_file_copy(char *from_path, char *to_path);
// TODO: return error
void osso_path_file_delete(char *path);

inline bool dg_ptr_valid(void *ptr) {
  return !((ptr == 0) || (ptr == NULL));
}

void osso_lib_hot_reload(OSSO_Hot_Reloaded_Library *lib, char *path, char *tmp_lib_folder, bool *ok_ptr, bool *new_ptr)
{
  DG_ASSERT(lib);

  OSSO_Time modified;
  // TODO: osso_handle_time_get
  osso_path_time_get(path, 0, &modified, 0);

  bool first_load = !dg_ptr_valid(lib->handle);
  bool lib_is_old = osso_time_later_than(modified, lib->time_since_last_load);

  bool ok = !first_load;
  bool new = false;

  if (first_load || lib_is_old) {
    // TODO: osso_platform_specific_switch_libs

    if (tmp_lib_folder) {
      DG_WIP;
    }

    char tmp_lib_path[1024] = {0};

    // FIXME: file_name_get + file_extension_get
    // FIXME: string builder implementation
    sprintf_s(tmp_lib_path, 1024, "tmp%d_%s", (i32)lib->_swap, path);
    lib->_swap = !lib->_swap;

    osso_path_file_delete(tmp_lib_path);
    osso_path_file_copy(path, tmp_lib_path);

    void *handle = osso_platform_specific_lib_load(tmp_lib_path);
    if (dg_ptr_valid(handle)) {
      osso_platform_specific_lib_unload(lib->handle);

      lib->handle = handle;
      lib->time_since_last_load = modified;

      ok = true;
      new = true;
      goto r;
    }

    ok = true;
    new = false;
    goto r;

  } else {
    if (first_load) {
      DG_LOG_ERROR("could not initialize symbols from lib %s\n", path);
    }

    ok = !first_load;
    new = false;
    goto r;
  }

r:
  if (dg_ptr_valid(ok_ptr)) *ok_ptr = ok;
  if (dg_ptr_valid(new_ptr)) *new_ptr = new;
  return;
}

// =========== specific to win32 ===========

// TODO: error checking
void osso_path_file_copy(char *from_path, char *to_path)
{
  BOOL _val = CopyFileA(from_path, to_path, FALSE);
}

void osso_path_file_delete(char *path)
{
  // TODO: possible errors:
  // - file busy
  // - file not found
  // - not enough permissions
  BOOL _val = DeleteFileA(path);
}

void *osso_platform_specific_lib_function_load(void *handle, char *function_name)
{
  void *result = (void *)GetProcAddress(handle, function_name);
  DG_ASSERT_MSG(result, "function exists");
  return result;
}

// TODO: trocar path por handle...?
void *osso_platform_specific_lib_load(char *lib_path)
{
  HMODULE result = LoadLibraryA(lib_path);
  // DG_ASSERT_MSG(result, "library exists");
  return result;
}

void osso_platform_specific_lib_unload(void *handle)
{
  BOOL _val = FreeLibrary(handle);
}
