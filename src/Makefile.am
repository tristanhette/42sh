# define the sub directories
SUBDIRS = ast \
	parser \
	lexer \
	io_backend \
	builtins \
	redirs \
	variables \
	fonctions

bin_PROGRAMS = 42sh

42sh_SOURCES = 42sh.c exec_main.c

42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

# define libs
42sh_LDADD =  \
       ast/libast.a \
       parser/libparser.a \
       lexer/liblexer.a \
       io_backend/libio_backend.a \
       builtins/libbuiltins.a \
       redirs/libredirs.a \
       variables/libvariables.a \
	   fonctions/libfonctions.a
