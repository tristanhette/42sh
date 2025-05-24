lib_LIBRARIES = liblexer.a

liblexer_a_SOURCES =  \
  lexer.c \
  lexer.h \
  token.c \
  token.h \
  utils_lexer.c \
  utils_lexer.h \
  process_lexer.c \
  process_lexer.h


liblexer_a_CPPFLAGS = -I$(top_srcdir)/src

liblexer_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = liblexer.a
