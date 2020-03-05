TOP = ..
include $(TOP)/configure/CONFIG

PROD = etui
PROD_LIBS += ca

etui_USR_CFLAGS += -pedantic -Wall -Wextra

etui_SYS_LIBS += ncurses
etui_SYS_LIBS += menu

etui_SRCS += utils.c
etui_SRCS += tui.c
etui_SRCS += cahelper.c
etui_SRCS += main.c

include $(TOP)/configure/RULES
