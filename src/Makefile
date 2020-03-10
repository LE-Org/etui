TOP = ..
include $(TOP)/configure/CONFIG

PROD = etui
PROD_LIBS += ca

etui_USR_CFLAGS += -pedantic -Wall -Wextra

etui_SYS_LIBS += ncurses
etui_SYS_LIBS += menu

etui_SRCS += utils.c
etui_SRCS += win.c
etui_SRCS += win_data.c
etui_SRCS += win_registrar.c
etui_SRCS += win_menu.c
etui_SRCS += win_fields.c
etui_SRCS += win_main.c
etui_SRCS += win_cmds.c
etui_SRCS += win_stats.c
etui_SRCS += tui.c
etui_SRCS += cahelper.c
etui_SRCS += main.c

include $(TOP)/configure/RULES
