#include <libcdg/libcdg.h>

/*
 * Manifest is a platform for:
 * - opening windows
 * - getting a opengl context
 * - gathering input
 * - outputting sound
 * */


#ifndef MANIFEST_INTERNAL
typedef void Manifest_Platform_Context;
#endif

typedef struct {
  Manifest_Platform_Context *platform_context;
} Manifest_Context;

