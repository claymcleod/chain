set -e

git submodule update --init --recursive

cd deps/libsodium/
./autogen.sh
./configure
make

cd ../hiredis/
make