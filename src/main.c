#ifdef _WIN32a
#include "SDL.h"
#else
#include "SDL2/SDL.h"
#endif

#include "logging.h"


int show_window() {
    SDL_Window* win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        ERROR("Unable to create window.");
    	return 1;
    }

    ERROR("Test error message.");
    INFO("It works?");

    SDL_DestroyWindow(win);
    return 0;
}


int main(int argc, char* argv[]) {
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        ERROR("Unable to initialize SDL.");
        return 1;
    }

    int status = show_window();
    
    SDL_Quit();
    return status;
}
