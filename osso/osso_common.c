#include "osso.h"

// IMPORTANT: osso_time_diff!
// TODO: osso_time_earlier_than?
// TODO: osso_time_later_than?
bool osso_time_later_than(OSSO_Time a, OSSO_Time b)
{
  return a.time > b.time;
}

char *osso_path_ncstring(DG_Arena *arena, OSSO_Path path, u32 *length)
{
  if (length != NULL) {
    *length = path.path.len;
  }
  return string8_copy_to_cstring(arena, path.path);
}

char *osso_path_cstring(DG_Arena *arena, OSSO_Path path)
{
  return osso_path_ncstring(arena, path, 0);
}
