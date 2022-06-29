#ifdef TEST

#ifdef _WIN32a
#include "SDL.h"
#else
#include "SDL2/SDL.h"
#endif
#include "SDL_image.h"

#include "logging.h"
#include "constants.h"
#include "entity.h"
#include "state.h"
#include "icon.h"
#include "event.h"
#include "draw.h"
#include "terrain.h"

#include "minunit.h"

typedef struct {
    Entity entity;
    int val;
} CompInt;

typedef struct {
    Entity entity;
    double val;
} CompDouble;

int tests_run = 0;
int tests_failed = 0;

static char* test_new_component() {
    CompGroup groupint = compgroup_init(3, sizeof(CompInt));
    mu_assert(groupint.alive == 0, "");
    
    CompInt* comp_a = (CompInt*)component_init(&groupint, 1);
    mu_assert(comp_a != NULL, "");
    mu_assert(comp_a->entity == 1, "");
    mu_assert(groupint.alive == 1, "");
    comp_a->val = 5;
    
    CompInt* comp_b = (CompInt*)component_init(&groupint, 2);
    mu_assert(comp_b != NULL, "");
    mu_assert(comp_b->entity == 2, "");
    mu_assert(groupint.alive == 2, "");
    
    comp_b->val = 6;
    mu_assert(comp_a->val == 5, "");
    
    CompInt* comp_c = (CompInt*)component_init(&groupint, 3);
    mu_assert(comp_c != NULL, "");
    mu_assert(comp_c->entity == 3, "");
    mu_assert(groupint.alive == 3, "");

    comp_c->val = 7;
    mu_assert(comp_a->val == 5, "");

    CompInt* comp_d = (CompInt*)component_init(&groupint, 4);
    mu_assert(comp_d == NULL, "");
    mu_assert(groupint.alive == 3, "");
    mu_assert(comp_a->val == 5, "");
    mu_assert(comp_a->entity == 1, "");
}

static char* test_compbgone32() {
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

static char* test_compbgone64() {
    CompGroup groupdouble = compgroup_init(3, sizeof(CompDouble));
    groupdouble.alive = 3;
    CompDouble* comps = (CompDouble*)groupdouble.mem;
    
    comps[0].entity = 1;
    comps[0].val = 6.5;
    
    comps[1].entity = 2;
    comps[1].val = 5.5;
    
    comps[2].entity = 3;
    comps[2].val = 4.5;

    component_end(&groupdouble, 1);
    
    mu_assert(comps[0].entity == 2, "");
    mu_assert(comps[0].val == 5.5, "");
    mu_assert(comps[1].entity == 3, "");
    mu_assert(comps[1].val == 4.5, "");
    mu_assert(comps[2].entity == 0, "");
    mu_assert(groupdouble.alive == 2, "");
    return 0;
}

static char* test_entbgone1() {
    CompGroup groups[2] = {
        compgroup_init(3, sizeof(CompInt)),
        compgroup_init(3, sizeof(CompInt)),
    };
    CompGroup* groupa = &groups[0];
    CompGroup* groupb = &groups[1];
    
    CompInt* comp = (CompInt*)component_init(groupa, 1);
    mu_assert(comp != NULL, "");
    mu_assert(groupa->alive == 1, "");

    comp = (CompInt*)component_init(groupb, 1);
    mu_assert(comp != NULL, "");
    mu_assert(groupb->alive == 1, "");

    groups_entity_end((CompGroup*)groups, 2, 1);
    mu_assert(groupa->alive == 0, "");
    mu_assert(groupb->alive == 0, "");
    
    return 0;
}

static char* test_entbgone2() {
    CompGroup groups[2] = {
        compgroup_init(3, sizeof(CompInt)),
        compgroup_init(3, sizeof(CompInt)),
    };
    CompGroup* groupa = &groups[0];
    CompGroup* groupb = &groups[1];
    
    CompInt* comp = (CompInt*)component_init(groupa, 1);
    mu_assert(comp != NULL, "");
    mu_assert(groupa->alive == 1, "");
    comp->val = 7;
    
    comp = (CompInt*)component_init(groupa, 2);
    mu_assert(comp != NULL, "");
    mu_assert(groupa->alive == 2, "");
    comp->val = 8;

    comp = (CompInt*)component_init(groupb, 1);
    mu_assert(comp != NULL, "");
    mu_assert(groupb->alive == 1, "");
    comp->val = 9;
    
    comp = (CompInt*)component_init(groupb, 2);
    mu_assert(comp != NULL, "");
    mu_assert(groupb->alive == 2, "");
    comp->val = 10;

    groups_entity_end((CompGroup*)groups, 2, 1);
    mu_assert(groupa->alive == 1, "");
    mu_assert(groupb->alive == 1, "");
    
    comp = (CompInt*)groupa->mem;
    mu_assert(comp->entity == 2, "");
    mu_assert(comp->val == 8, "");
    
    comp = (CompInt*)groupb->mem;
    mu_assert(comp->entity == 2, "");
    mu_assert(comp->val == 10, "");
    
    return 0;
}

int main(int argc, char **argv) {
    mu_run_test(test_compbgone32);
    mu_run_test(test_compbgone64);
    mu_run_test(test_entbgone1);
    mu_run_test(test_entbgone2);

    printf("Passed: %d Failed: %d\n", tests_run - tests_failed, tests_failed);

    return tests_failed;
}

#endif /* TEST */
