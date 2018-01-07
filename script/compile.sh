BINDIR="bin"
LIBDIR="src"
HEADERS="$LIBDIR/*.h"
SOURCES="$LIBDIR/*.c"
ARTIFACTS="$LIBDIR/*.o"
CHT_SOURCE="$BINDIR/cht.c"
CHT_BINARY="$BINDIR/cht"

LIBHIREDIS="./deps/hiredis/libhiredis.a"
LIBSODIUM="./deps/libsodium/src/libsodium/.libs/libsodium.a"

if [[ ! -f $LIBHIREDIS ]] || [[ ! -f $LIBSODIUM ]]; then
    >&2 echo ""
    >&2 echo "Please consult the README.md for instructions on building"
    >&2 echo "and configuring the dependencies!"
    exit 1
fi

rm -r $ARTIFACTS $CHT_BINARY
C_OPTS="-std=c99 -Wall -O3 $LIBHIREDIS $LIBSODIUM"

for F in `echo $SOURCES`; do
    gcc $C_OPTS -c -o ${F%.c}.o $F
done

gcc $C_OPTS -o $CHT_BINARY -I$LIBDIR `echo $ARTIFACTS` $CHT_SOURCE