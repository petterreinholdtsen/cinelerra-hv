// Compiler inlines assemply language which defines symbols.

#if ARCH_X86
#include "mmx.h"
#endif


int mmx_ok_workaround()
{
	return mmx_ok();
}
