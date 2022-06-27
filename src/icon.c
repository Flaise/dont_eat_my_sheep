#include <stdbool.h>

#ifdef _WIN32a
#include "SDL.h"
#else
#include "SDL2/SDL.h"
#endif

#include "logging.h"
#include "constants.h"
#include "icon.h"

Icon icon_init(TexID texture_id, SDL_Rect source_rect) {
    Icon result = {
        .source_rect = source_rect,
        .texture_id = texture_id,
    };
    return result;
}

Icon icon_tile_init(TexID texture_id, int tile_size, int x, int y) {
    SDL_Rect source_rect = {
        .x = tile_size * x,
        .y = tile_size * y,
        .w = tile_size,
        .h = tile_size,
    };
    return icon_init(texture_id, source_rect);
}

void icon_draw(State* state, Icon* icon, SDL_Rect* dest_rect) {
    SDL_Texture* texture = state->textures[icon->texture_id];
    if (texture == NULL) {
        WARN("icon texture not found");
        return;
    }
    if (SDL_RenderCopy(state->renderer, texture, &icon->source_rect, dest_rect) != 0) {
        WARN("SDL_RenderCopy (icon)");
        return;
    }
}