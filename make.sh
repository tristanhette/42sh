autoreconf --install
./configure CFLAGS="-std=c99 -Werror -Wall -Wextra -Wvla -pedantic -O0 -g" # -fsanitize=address"
make
