#include "SDL.h"
#include "SDL_image.h"
#include <stdbool.h>
#include <stddef.h>
#include "logging.h"
#include "entity.h"
#include "constants.h"
#include "component.h"

CompGroup compgroup_init(uint32_t total, size_t compsize) {
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

void compgroup_clear(CompGroup* group) {
    if (group == NULL) {
        ERROR("Component group can't be null.");
        return;
    }
    group->alive = 0;
}

static AbstractComp* component_at(void* mem, size_t compsize, uint32_t index) {
    return (AbstractComp*)(mem + (index * compsize));
}

void* component_init(CompGroup* group, Entity entity) {
    if (group == NULL) {
        return NULL;
    }
    if (group->alive >= group->total) {
        return NULL;
    }
    if (entity == 0) {
        WARN("Entity can't be 0.");
        return NULL;
    }

    uint32_t dest_index = group->alive;
    
    for (uint32_t r = group->alive; r > 0;) {
        /* Loop counter has to decrement first because the size_t can't go negative. */
        r -= 1;
        
        AbstractComp* other = component_at(group->mem, group->compsize, r);
        if (other == NULL) {
            return NULL; /* Shouldn't be possible. */
        }
        
        if (other->entity == entity) {
            return NULL;
        } else if (other->entity > entity) {
            dest_index -= 1;
        } else {
            break;
        }
    }
    void* source = group->mem + dest_index * group->compsize;
    void* dest = group->mem + (dest_index + 1) * group->compsize;
    memmove(dest, source, group->compsize * (group->alive - dest_index));
    
    AbstractComp* result = (AbstractComp*)source;
    result->entity = entity;
    
    group->alive += 1;
    return source;
}

void component_end(CompGroup* group, Entity entity) {
    bool found = false;
    for (uint32_t index = 0; index < group->alive; index += 1) {
        AbstractComp* comp = component_at(group->mem, group->compsize, index);
        if (found) {
            /* memcpy is safe because found will never == true on first iteration. */
            void* dest = group->mem + (index - 1) * group->compsize;
            void* source = group->mem + (index * group->compsize);
            memcpy(dest, source, group->compsize);
            
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
        group->alive -= 1;
    }
}

void* component_of(CompGroup* group, Entity entity) {
    if (group == NULL) {
        return NULL;
    }
    if (entity == 0) {
        return NULL;
    }

    for (uint32_t r = 0; r < group->alive; r += 1) {
        AbstractComp* other = component_at(group->mem, group->compsize, r);
        if (other->entity == entity) {
            return other;
        }
    }
    
    return NULL;
}

void compgroups_entity_end(CompGroup* group_arr, int8_t ngroups, Entity entity) {
    for (int8_t i = 0; i < ngroups ; i += 1) {
        CompGroup* group = &group_arr[i];
        component_end(group, entity);
    }
}

bool component_iterate(CompGroup** groups, void** comps, int8_t ncomps) {

#ifdef DEBUG
    int8_t null_count = 0;
    for (int8_t r = 0; r < ncomps; r += 1) {
        if (comps[r] == NULL) {
            null_count += 1;
        }

        if (groups[r] == NULL) {
            ERROR("component group can't be NULL");
            return false;
        }
    }
    if (null_count != 0 && null_count != ncomps) {
        ERROR("comps must be all NULL or none NULL");
        return false;
    }
#endif /* DEBUG */

    /* Advance all pointers by 1. */
    for (int8_t r = 0; r < ncomps; r += 1) {
        if (comps[r] == NULL) {
            comps[r] = groups[r]->mem;
        } else {
            comps[r] += groups[r]->compsize;
        }
    }

    /* Advance pointer of lowest entity until all pointers match. */
    while (true) {
        if (comps[0] - groups[0]->mem >= groups[0]->compsize * groups[0]->alive) {
            /* A pointer reached the end of the component group. */
            return false;
        }
        
        bool matching = true;

        Entity lowest = ((AbstractComp*)comps[0])->entity;
        Entity next_low = lowest;
        int8_t lowest_index = 0;

        for (int8_t r = 1; r < ncomps; r += 1) {        
            if (comps[r] - groups[r]->mem >= groups[r]->compsize * groups[r]->alive) {
                /* A pointer reached the end of the component group. */
                return false;
            }
            
            Entity current = ((AbstractComp*)comps[r])->entity;
            if (current != lowest) {
                matching = false;

                if (current < lowest) {
                    next_low = lowest;
                    lowest = current;
                    lowest_index = r;
                } else if (current < next_low) {
                    next_low = current;
                }
            }
        }
        if (matching) {
            /* Yay. */
            return true;
        }

        comps[lowest_index] += groups[lowest_index]->compsize;
    }

    /* Generic data structures in C are difficult. */
}
