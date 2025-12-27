#ifndef OSSO_H
#define OSSO_H

#include "../libcdg/libcdg.h"
#include "../string/string.h"
#include <time.h>

// TODO: make multi platform
typedef struct {
  time_t time;
} OSSO_Time;

typedef struct {
  void *handle;
  OSSO_Time time_since_last_load;
  bool _swap;
} OSSO_Hot_Reloaded_Library;

typedef struct {
  String8 path;
} OSSO_Path;


char *osso_fd_read_entire_file(DG_Arena *a, FILE *file, usize *size);
char *osso_path_cstring_read_entire_file(DG_Arena *a, char *path, usize *size);
void osso_path_cstring_time_get(char *path, OSSO_Time *created, OSSO_Time *modified, OSSO_Time *accessed);


void osso_lib_hot_reload(OSSO_Hot_Reloaded_Library *lib, char *path, char *tmp_lib_folder, bool *ok_ptr, bool *new_ptr);
void osso_path_cstring_file_copy(char *from_path, char *to_path);
void osso_path_cstring_file_delete(char *path);

// IMPORTANT: osso_time_diff!
// TODO: osso_time_earlier_than?
// TODO: osso_time_later_than?
bool osso_time_later_than(OSSO_Time a, OSSO_Time b);


// TODO: make platform independent...?
void *osso_platform_specific_lib_function_load(void *handle, char *function_name);

#endif
