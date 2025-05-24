#!/bin/sh

# Used to clear all autoreconf --install files

autoreconf="aclocal.m4 ar-lib autom4te.cache/ compile config.log config.status configure configure~ depcomp install-sh missing"

# Makefiles from modules
lexer="src/lexer/Makefile src/lexer/Makefile.in src/lexer/.deps"
parser="src/parser/Makefile src/parser/Makefile.in src/parser/.deps"
ast="src/ast/Makefile src/ast/Makefile.in src/ast/.deps"
io_backend="src/io_backend/Makefile src/io_backend/Makefile.in src/io_backend/.deps"
builtins="src/builtins/Makefile src/builtins/Makefile.in src/builtins/.deps"
variables="src/variables/Makefile src/variables/Makefile.in src/variables/.deps"
src="src/Makefile src/Makefile.in src/.deps/"
redirs="src/redirs/Makefile src/redirs/Makefile.in src/redirs/.deps/"
fonctions="src/fonctions/Makefile src/fonctions/Makefile.in src/fonctions/.deps/"

tests="tests/__pycache__/ tests/.output.err tests/.output.out tests/stderr/stderr.txt tests/stderr/stdout.txt tests/scripts/input tests/scripts/temp test tests/test"

makes="Makefile Makefile.in $src $lexer $parser $ast $io_backend $builtins $variables $tests $redirs $fonctions"

make clean
make distclean
rm -rf $autoreconf $deps $makes
