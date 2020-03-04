#ifndef UTILS_H
#define UTILS_H

void swallow_stderr(void);
void wait_ns(unsigned long long ns);
char **process_input_file(const char *path);

#endif
