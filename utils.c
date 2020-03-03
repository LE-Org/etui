#include <stdio.h>

void
swallow_stderr()
{
	freopen("/dev/null", "w", stderr);
}
