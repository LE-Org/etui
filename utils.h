#ifndef UTILS_H
#define UTILS_H

void swallow_stderr(void);
void wait_tick(unsigned long long period_ns);
char **process_input_file(const char *path);

#endif
