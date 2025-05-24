lib_LIBRARIES = libparser.a

libparser_a_SOURCES =  \
  parser.c \
  parser.h \
  grammar.c \
  grammar.h \
  grammar_shell.c \
  grammar_shell.h \
  grammar_case.h \
  grammar_case.c \
  utils.c \
  utils.h \
  utils_shell.c \
  utils_shell.h \
  utils_case.c \
  utils_case.h

libparser_a_CPPFLAGS = -I$(top_srcdir)/src

libparser_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libparser.a
