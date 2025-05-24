
lib_LIBRARIES = libbuiltins.a

libbuiltins_a_SOURCES =  \
  builtins.c \
  builtins.h \
  builtins_next.c \
  builtins_next.h \
  builtins_next2.c \
  builtins_next2.h \
  utils.c \
  utils.h

builtins_CPPFLAGS = -I$(top_srcdir)/src

builtins_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libbuiltins.a
