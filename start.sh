
mkdir -p ./out/ || exit 1

cd src || exit 1 # removes extraneous folder name from log messages
/usr/bin/time -f "completed in %es" \
    c99 -o ../out/cgame *.c `pkg-config --cflags --libs sdl2 SDL2_image` || exit 1
cd .. || exit 1

./out/cgame
