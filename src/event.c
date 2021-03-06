#include "SDL.h"
#include "logging.h"
#include "entity.h"
#include "constants.h"
#include "component.h"
#include "state.h"
#include "draw.h"
#include "select.h"
#include "board.h"

void size_changed(State* state, uint32_t width, uint32_t height) {
    /* Letterboxing is done automatically with SDL_RenderSetLogicalSize. */
    redraw(state);
}

void mouse_motion(State* state, int32_t x, int32_t y) {
    select_mouse_move(state, x, y);
    redraw(state);
}

void mouse_leave(State* state) {
    select_mouse_leave(state);
    redraw(state);
}

void mouse_button(State* state, uint8_t button, int32_t x, int32_t y) {
    select_mouse_press(state, button, x, y);
    redraw(state);
}

void key_down(State* state, SDL_Keycode code) {
    if (code == SDLK_r) {
        /* R = restart */
        level_restart(state);
    } else if (code == SDLK_ESCAPE) {
        /* Esc = quit */
        state->exiting = true;
    } else if (code == SDLK_LEFTBRACKET) {
        /* [ = previous level */
        level_prev(state);
    } else if (code == SDLK_RIGHTBRACKET) {
        /* ] = next level */
        level_next(state);
    }
}

int events_pending(State* state) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                state->exiting = true;
                return 0;

            /* on Ubuntu, drag-resize = SizeChanged
               double click to unmaximize or click unmaximize button = SizeChanged->Restored
               click on task bar to unminimize = Restored */
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    int32_t width = event.window.data1;
                    int32_t height = event.window.data2;
                    size_changed(state, width, height);
                } else if (event.window.event == SDL_WINDOWEVENT_LEAVE) {
                    mouse_leave(state);
                }
                continue;

            case SDL_MOUSEMOTION:
                /* SDL_GetMouseState doesn't report transformed positions so pass event data. */
                mouse_motion(state, event.motion.x, event.motion.y);
                continue;

            case SDL_MOUSEBUTTONDOWN:
                mouse_button(state, event.button.button, event.button.x, event.button.y);
                continue;

            case SDL_KEYDOWN:
                key_down(state, event.key.keysym.sym);
                continue;
        }
    }
    return 0;
}

int events_all(State* state) {
    level_init(state);
    
    if (draw_now(state) != 0) {
        return 1;
    }
    
    while (true) {
        /* Process all available events. */
        if (events_pending(state) != 0) {
            return 1;
        }

        if (state->exiting) {
            return 0;
        }

        /* Redraw. */
        /* TODO: Remove finished tweens to avoid unnecessary redrawing and other computations. */
        if (state->needs_redraw || state->components.compgroups[COMPTYPE_TWEEN].alive > 0) {
            if (draw_now(state) != 0) {
                return 1;
            }

            /* TODO: Framerate cap when vsync isn't available. */
        } else {
            /* Delay to avoid hogging CPU when there's nothing to draw. */
            SDL_Delay(10);
        }
    }
}
