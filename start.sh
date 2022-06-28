
# CONVERT PNG TO C

if [[ $1 != 'test' ]]; then
    mkdir -p ./src/res || exit 1
    # xxd --include "./res/Floor - Grass 1 64x64.png" ./src/res/floor.h
    # xxd --include "./res/Floor - Dirt 2 64x64.png" ./src/res/floor.h
    # xxd --include "./res/Wall - Brick 1 64x64.png" ./src/res/wall.h
    # xxd --include "./res/Wall - Brick 3 64x64.png" ./src/res/wall.h
    xxd --include "./res/Tiny Top Down 32x32.png" ./src/res/terrain.h
    xxd --include "./res/dragon.png" ./src/res/dragon.h
    xxd --include "./res/knight.png" ./src/res/knight.h
    xxd --include "./res/sheep.png" ./src/res/sheep.h
fi


# COMPILE C

if [[ $1 == 'test' ]]; then
    DEFINE="-D TEST"
    BIN="bin/cgame_test"
else
    DEFINE=""
    BIN="bin/cgame"
fi

mkdir -p ./bin/ || exit 1

cd src || exit 1 # removes extraneous folder name from log messages
/usr/bin/time -f "compilation: %es" \
    c99 -o ../${BIN} *.c `pkg-config --cflags --libs sdl2 SDL2_image` ${DEFINE} || exit 1
cd .. || exit 1


# EXECUTE

./${BIN}
