lib_LIBRARIES = libast.a

libast_a_SOURCES =  \
  ast.c \
  ast.h \
  ast_exec.c \
  ast_exec.h \
  command.c \
  command.h

libast_a_CPPFLAGS = -I$(top_srcdir)/src

libast_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libast.a
