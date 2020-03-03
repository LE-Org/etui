#include <stdio.h>
#include <time.h>

#define NSEC_PER_SEC (1000000000)

void
swallow_stderr(void)
{
	freopen("/dev/null", "w", stderr);
}

void
wait_tick(unsigned long long period_ns)
{
	static struct timespec deadline = {0};
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL);
	clock_gettime(CLOCK_MONOTONIC, &deadline);
	deadline.tv_sec += period_ns / NSEC_PER_SEC;
	deadline.tv_nsec += period_ns % NSEC_PER_SEC;
	if (deadline.tv_nsec >= NSEC_PER_SEC) {
		deadline.tv_sec++;
		deadline.tv_nsec -= NSEC_PER_SEC;
	}
}
