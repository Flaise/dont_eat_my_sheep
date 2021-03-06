#ifdef TEST

#include "SDL.h"
#include "SDL_image.h"
#include <stdbool.h>
#include "logging.h"
#include "entity.h"
#include "constants.h"
#include "component.h"
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

CompInt* comp_int_init(CompGroup* group, Entity entity, int val) {
    CompInt* result = (CompInt*)component_init(group, entity);
    if (result != NULL) {
        result->val = val;
    }
    return result;
}

typedef struct {
    Entity entity;
    double val;
} CompDouble;

CompDouble* comp_double_init(CompGroup* group, Entity entity, double val) {
    CompDouble* result = (CompDouble*)component_init(group, entity);
    if (result != NULL) {
        result->val = val;
    }
    return result;
}

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

    return 0;
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
    
    CompInt* comp = comp_int_init(groupa, 1, 4);
    mu_assert(comp != NULL, "");
    mu_assert(groupa->alive == 1, "");

    comp = comp_int_init(groupb, 1, 5);
    mu_assert(comp != NULL, "");
    mu_assert(groupb->alive == 1, "");

    compgroups_entity_end((CompGroup*)groups, 2, 1);
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

    compgroups_entity_end((CompGroup*)groups, 2, 1);
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

static char* test_no_duplicate_component() {
    CompGroup groupint = compgroup_init(3, sizeof(CompInt));
    CompInt* comps = (CompInt*)groupint.mem;

    CompInt* comp = comp_int_init(&groupint, 1, 4);
    mu_assert(comp != NULL, "");
    mu_assert(groupint.alive == 1, "");
    
    comp = comp_int_init(&groupint, 1, 8);
    mu_assert(comp == NULL, "");
    mu_assert(comps[0].entity == 1, "");
    mu_assert(comps[0].val == 4, "");
    mu_assert(groupint.alive == 1, "");
    
    return 0;
}

static char* test_no_duplicate_early_component() {
    CompGroup groupint = compgroup_init(4, sizeof(CompInt));
    CompInt* comps = (CompInt*)groupint.mem;

    comp_int_init(&groupint, 1, 4);
    comp_int_init(&groupint, 3, 6);
    comp_int_init(&groupint, 2, 5);

    CompInt* comp = comp_int_init(&groupint, 1, 8);
    mu_assert(comp == NULL, "");
    
    mu_assert(groupint.alive == 3, "");
    mu_assert(comps[0].entity == 1, "");
    mu_assert(comps[0].val == 4, "");
    mu_assert(comps[1].entity == 2, "");
    mu_assert(comps[1].val == 5, "");
    mu_assert(comps[2].entity == 3, "");
    mu_assert(comps[2].val == 6, "");
    
    return 0;
}

static char* test_insertion_sort() {
    CompGroup groupint = compgroup_init(3, sizeof(CompInt));
    CompInt* comps = (CompInt*)groupint.mem;

    CompInt* comp = comp_int_init(&groupint, 2, 4);
    mu_assert(comp != NULL, "");
    comp = comp_int_init(&groupint, 1, 8);
    mu_assert(comp != NULL, "");
    mu_assert(groupint.alive == 2, "");
    
    mu_assert(comps[0].entity == 1, "");
    mu_assert(comps[0].val == 8, "");
    mu_assert(comps[1].entity == 2, "");
    mu_assert(comps[1].val == 4, "");

    return 0;
}

static char* test_insertion_sort_filled() {
    CompGroup groupint = compgroup_init(3, sizeof(CompInt));
    CompInt* comps = (CompInt*)groupint.mem;

    CompInt* comp = comp_int_init(&groupint, 3, 4);
    mu_assert(comp != NULL, "");
    comp = comp_int_init(&groupint, 2, 8);
    mu_assert(comp != NULL, "");
    comp = comp_int_init(&groupint, 1, 16);
    mu_assert(comp != NULL, "");
    mu_assert(groupint.alive == 3, "");
    
    mu_assert(comps[0].entity == 1, "");
    mu_assert(comps[0].val == 16, "");
    mu_assert(comps[1].entity == 2, "");
    mu_assert(comps[1].val == 8, "");
    mu_assert(comps[2].entity == 3, "");
    mu_assert(comps[2].val == 4, "");

    return 0;
}

static char* test_too_many_components() {
    CompGroup groupint = compgroup_init(1, sizeof(CompInt));
    CompInt* comps = (CompInt*)groupint.mem;

    CompInt* comp = comp_int_init(&groupint, 1, 4);
    mu_assert(comp != NULL, "");
    comp = comp_int_init(&groupint, 2, 8);
    mu_assert(comp == NULL, "");
    mu_assert(groupint.alive == 1, "");
    
    mu_assert(comps[0].entity == 1, "");
    mu_assert(comps[0].val == 4, "");

    return 0;
}

static char* test_iterate_empty() {
    CompGroup groupa = compgroup_init(2, sizeof(CompInt));
    
    CompGroup* groups[] = {&groupa};
    void* comps[] = {NULL};
    mu_assert(component_iterate((CompGroup**)&groups, (void**)&comps, 1) == false, "");

    return 0;
}

static char* test_iterate_all() {
    CompGroup groupa = compgroup_init(2, sizeof(CompInt));
    CompGroup groupb = compgroup_init(2, sizeof(CompInt));

    comp_int_init(&groupa, 1, 4);
    comp_int_init(&groupa, 2, 8);
    
    comp_int_init(&groupb, 1, 5);
    comp_int_init(&groupb, 2, 9);

    int result[5] = {0, 0, 0, 0, 0};
    int index = 0;

    CompGroup* groups[] = {&groupa, &groupb};
    void* comps[] = {NULL, NULL};
    while (component_iterate((CompGroup**)&groups, (void**)&comps, 2)) {
        CompInt* a = comps[0];
        CompInt* b = comps[1];

        result[index] = a->val;
        result[index + 1] = b->val;
        index += 2;
    }

    mu_assert(result[0] == 4, "");
    mu_assert(result[1] == 5, "");
    mu_assert(result[2] == 8, "");
    mu_assert(result[3] == 9, "");
    mu_assert(result[4] == 0, "");
    
    return 0;
}

static char* test_iterate_partial() {
    CompGroup groupa = compgroup_init(5, sizeof(CompInt));
    CompGroup groupb = compgroup_init(5, sizeof(CompInt));

    comp_int_init(&groupa, 1, 4);
    comp_int_init(&groupb, 1, 5);
    
    comp_int_init(&groupa, 2, 6);
    
    comp_int_init(&groupa, 3, 8);
    comp_int_init(&groupb, 3, 9);
    
    comp_int_init(&groupb, 4, 11);
    
    comp_int_init(&groupa, 5, 12);
    comp_int_init(&groupb, 5, 13);

    int result[7] = {0, 0, 0, 0, 0, 0, 0};
    int index = 0;

    CompGroup* groups[] = {&groupa, &groupb};
    void* comps[] = {NULL, NULL};
    while (component_iterate((CompGroup**)&groups, (void**)&comps, 2)) {
        CompInt* a = comps[0];
        CompInt* b = comps[1];

        result[index] = a->val;
        result[index + 1] = b->val;
        index += 2;
    }

    mu_assert(result[0] == 4, "");
    mu_assert(result[1] == 5, "");
    mu_assert(result[2] == 8, "");
    mu_assert(result[3] == 9, "");
    mu_assert(result[4] == 12, "");
    mu_assert(result[5] == 13, "");
    mu_assert(result[6] == 0, "");
    
    return 0;
}

static char* test_iterate_partial_skip() {
    CompGroup groupa = compgroup_init(5, sizeof(CompInt));
    CompGroup groupb = compgroup_init(5, sizeof(CompInt));

    comp_int_init(&groupa, 1, 4);
    comp_int_init(&groupb, 1, 5);
    
    comp_int_init(&groupa, 3, 8);
    comp_int_init(&groupb, 3, 9);
    
    comp_int_init(&groupb, 4, 11);
    
    comp_int_init(&groupa, 5, 12);
    comp_int_init(&groupb, 5, 13);

    int result[7] = {0, 0, 0, 0, 0, 0, 0};
    int index = 0;

    CompGroup* groups[] = {&groupa, &groupb};
    void* comps[] = {NULL, NULL};
    while (component_iterate((CompGroup**)&groups, (void**)&comps, 2)) {
        CompInt* a = comps[0];
        CompInt* b = comps[1];

        result[index] = a->val;
        result[index + 1] = b->val;
        index += 2;
    }

    mu_assert(result[0] == 4, "");
    mu_assert(result[1] == 5, "");
    mu_assert(result[2] == 8, "");
    mu_assert(result[3] == 9, "");
    mu_assert(result[4] == 12, "");
    mu_assert(result[5] == 13, "");
    mu_assert(result[6] == 0, "");
    
    return 0;
}

static char* test_component_for_entity() {
    CompGroup groupa = compgroup_init(5, sizeof(CompInt));

    mu_assert(component_of(&groupa, 1) == NULL, "");
    
    comp_int_init(&groupa, 1, 4);
    
    mu_assert(component_of(&groupa, 1) == groupa.mem, "");
    mu_assert(component_of(&groupa, 0) == NULL, "");
    
    comp_int_init(&groupa, 2, 9);
    mu_assert(component_of(&groupa, 1) == groupa.mem, "");
    mu_assert(component_of(&groupa, 2) == groupa.mem + sizeof(CompInt), "");
    
    return 0;
}

int main(int argc, char **argv) {
    mu_run_test(test_new_component);
    mu_run_test(test_compbgone32);
    mu_run_test(test_compbgone64);
    mu_run_test(test_entbgone1);
    mu_run_test(test_entbgone2);
    mu_run_test(test_no_duplicate_component);
    mu_run_test(test_no_duplicate_early_component);
    mu_run_test(test_insertion_sort);
    mu_run_test(test_insertion_sort_filled);
    mu_run_test(test_too_many_components);
    mu_run_test(test_iterate_empty);
    mu_run_test(test_iterate_all);
    mu_run_test(test_iterate_partial);
    mu_run_test(test_iterate_partial_skip);
    mu_run_test(test_component_for_entity);

    if (tests_failed > 0) {
        printf("Passed: %d Failed: %d\n", tests_run - tests_failed, tests_failed);
    } else {
        printf("All tests passed.\n");
    }

    return tests_failed;
}

#endif /* TEST */
