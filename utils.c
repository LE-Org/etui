#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NSEC_PER_SEC (1000000000)

void
swallow_stderr(void)
{
	freopen("/dev/null", "w", stderr);
}

void
wait_period(unsigned long long ns)
{
	static struct timespec deadline = {0};
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL);
	clock_gettime(CLOCK_MONOTONIC, &deadline);
	deadline.tv_sec += ns / NSEC_PER_SEC;
	deadline.tv_nsec += ns % NSEC_PER_SEC;
	if (deadline.tv_nsec >= NSEC_PER_SEC) {
		deadline.tv_sec++;
		deadline.tv_nsec -= NSEC_PER_SEC;
	}
}

char **
process_input_file(const char *path)
{
	FILE *file;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	char **pvs = NULL;
	int npvs = 0;
	char *buf;
	int i;

	file = fopen(path, "r");

	if (!file)
		return NULL;

	while ((nread = getline(&line, &len, file)) != -1) {
		for (i = 0; i < nread; ++i)
			if (line[i] == '\n' || line[i] == ' ')
				break;

		buf = malloc(i+1);
		strncpy(buf, line, i);
		buf[i] = '\0';

		if (buf == NULL)
			goto alloc_err;

		/* is that optimized in glibc? */
		pvs = realloc(pvs, (npvs + 1) * sizeof(char *));

		if (pvs == NULL)
			goto alloc_err;

		pvs[npvs++] = buf;
	}
	pvs[npvs] = NULL;
	free(line);
	fclose(file);

alloc_err: /* ? */

	return pvs;
}
