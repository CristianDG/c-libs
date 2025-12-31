#include "osso.h"

#include "osso_common.c"

#if DG_OS_WINDOWS
# include "osso_win32.c"
#elif DG_OS_LINUX
# include "osso_linux.c"
#endif
