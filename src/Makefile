TOP = ..
include $(TOP)/configure/CONFIG

PROD = nccamon
PROD_LIBS += ca

nccamon_USR_CFLAGS += -pedantic -Wall -Wextra

nccamon_SYS_LIBS += ncurses
nccamon_SYS_LIBS += menu

nccamon_SRCS += utils.c
nccamon_SRCS += tui.c
nccamon_SRCS += cahelper.c
nccamon_SRCS += main.c

include $(TOP)/configure/RULES
