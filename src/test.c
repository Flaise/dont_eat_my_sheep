#ifdef TEST

#ifdef _WIN32a
#include "SDL.h"
#else
#include "SDL2/SDL.h"
#endif
#include "SDL_image.h"

#include "logging.h"
#include "constants.h"
#include "state.h"
#include "icon.h"
#include "event.h"
#include "draw.h"
#include "terrain.h"

#include "minunit.h"

typedef struct {
    Entity entity;
} AbstractComp;

typedef struct {
    void* mem;
    size_t alive;
    size_t total;
    size_t compsize;
} CompGroup;

CompGroup compgroup_init(size_t total, size_t compsize) {
    if (total == 0 || compsize == 0) {
        ERROR("Invalid total or component size.");
    }
    CompGroup result;
    result.mem = calloc(total, compsize);
    result.alive = 0;
    result.total = total;
    result.compsize = compsize;
    return result;
}

typedef struct {
    Entity entity;
    int val;
} CompInt;

typedef struct {
    Entity entity;
    double val;
} CompDouble;



Entity entity_next(State* state) {
    
}

int component_init(State* state, Entity entity) {
    
}





// void entity_end(State* state, Entity entity) {
    // bool found = false;
    // for (int i = 0; i < MAX_COMPONENTS; i += 1) {
        // Avatar* comp = state->avatars[i];
        // if (found) {
            // /* Safe because found will never == true on first iteration. */
            // state->avatars[i - 1] = *comp;
            // if (comp->entity == 0) {
                // break;
            // }
            // comp->entity = 0;
        // } else {
            // if (comp->entity == 0) {
                // break;
            // }
            // if (comp->entity == entity) {
                // found = true;
            // }
        // }
    // }
// }


AbstractComp* component_at(void* mem, size_t compsize, size_t index) {
    return (AbstractComp*)(mem + (index * compsize));
}

void component_end(CompGroup* components, Entity entity) {
    bool found = false;
    for (size_t index = 0; index < components->alive; index += 1) {
        AbstractComp* comp = component_at(components->mem, components->compsize, index);
        if (found) {
            /* memcpy is safe because found will never == true on first iteration. */
            void* dest = components->mem + (index - 1) * components->compsize;
            void* source = components->mem + (index * components->compsize);
            memcpy(dest, source, components->compsize);
            
            if (comp->entity == 0) {
                break;
            }
            comp->entity = 0; /* Necessary to clear component on last iteration. */
        } else {
            if (comp->entity == 0) {
                break;
            }
            if (comp->entity == entity) {
                found = true;
            }
        }
    }
    if (found) {
        components->alive -= 1;
    }
}

// void draw(state) {
    // for (int i = 0; i < MAX_COMPONENTS; i += 1) {
        // // check if entity alive
        // CPosition* cposition = ...
        // CIcon* cicon = ...
// 
        // draw(cposition, cicon);
    // }
// }


int tests_run = 0;
int tests_failed = 0;

static char* test_compbgone() {
    CompGroup groupint = compgroup_init(3, sizeof(CompInt));
    groupint.alive = 3;
    CompInt* comps = (CompInt*)groupint.mem;
    
    comps[0].entity = 1;
    comps[0].val = 6;
    
    comps[1].entity = 2;
    comps[1].val = 5;
    
    comps[2].entity = 3;
    comps[2].val = 4;

    component_end(&groupint, 1);
    
    mu_assert(comps[0].entity == 2, "");
    mu_assert(comps[0].val == 5, "");
    mu_assert(comps[1].entity == 3, "");
    mu_assert(comps[1].val == 4, "");
    mu_assert(comps[2].entity == 0, "");
    mu_assert(groupint.alive == 2, "");
    return 0;
}

int main(int argc, char **argv) {
    mu_run_test(test_compbgone);

    printf("Passed: %d Failed: %d\n", tests_run - tests_failed, tests_failed);

    return tests_failed;
}

#endif /* TEST */
