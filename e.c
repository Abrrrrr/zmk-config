/* blocks.h */
#ifndef BLOCKS_H
#define BLOCKS_H

#include <stdbool.h>

typedef struct {
    int N;
    int *above; // above[m] = block directly on m, or 0 if none
    int *below; // below[m] = block directly below m, or 0 if on table
} BlocksWorld;

// Create and destroy
BlocksWorld *create(int N);
void destroy(BlocksWorld *world);

// Observers
bool isOnTable(BlocksWorld *world, int m);
bool isOpen(BlocksWorld *world, int m);
int isOn(BlocksWorld *world, int m);
bool isAbove(BlocksWorld *world, int m, int n);

// Manipulator
void move(BlocksWorld *world, int m, int n);

#endif // BLOCKS_H

/* blocks.c */
#include "blocks.h"
#include <stdlib.h>

BlocksWorld *create(int N) {
    BlocksWorld *world = malloc(sizeof(BlocksWorld));
    if (!world) return NULL;
    world->N = N;
    world->above = calloc(N+1, sizeof(int));
    world->below = calloc(N+1, sizeof(int));
    return world;
}

void destroy(BlocksWorld *world) {
    if (!world) return;
    free(world->above);
    free(world->below);
    free(world);
}

bool isOnTable(BlocksWorld *world, int m) {
    if (!world || m < 1 || m > world->N) return false;
    return world->below[m] == 0;
}

bool isOpen(BlocksWorld *world, int m) {
    if (!world) return false;
    if (m == 0) return true;
    if (m < 1 || m > world->N) return false;
    return world->above[m] == 0;
}

int isOn(BlocksWorld *world, int m) {
    if (!world || m < 1 || m > world->N) return m;
    return world->below[m];
}

bool isAbove(BlocksWorld *world, int m, int n) {
    if (!world || m < 1 || m > world->N || n < 0 || n > world->N) return false;
    int cur = world->below[m];
    while (cur != 0) {
        if (cur == n) return true;
        cur = world->below[cur];
    }
    return false;
}

void move(BlocksWorld *world, int m, int n) {
    if (!world || m < 1 || m > world->N || n < 0 || n > world->N) return;
    if (!isOpen(world, m) || !isOpen(world, n)) return;
    int b = world->below[m];
    if (b != 0) world->above[b] = 0;
    world->below[m] = n;
    if (n != 0) world->above[n] = m;
}

/* main.c */
#include "blocks.h"
#include <stdio.h>

int main(void) {
    BlocksWorld *world = create(12);
    // Build towers: 1-2-3
    move(world, 2, 1);
    move(world, 3, 2);
    // Build towers: 4-5-6-7
    move(world, 5, 4);
    move(world, 6, 5);
    move(world, 7, 6);
    // Build towers: 8-9-10-11-12
    move(world, 9, 8);
    move(world,10, 9);
    move(world,11,10);
    move(world,12,11);

    for (int i = 1; i <= 12; i++) {
        printf("Block %d: on %d, open=%s, onTable=%s\n",
               i,
               isOn(world, i),
               isOpen(world, i) ? "true" : "false",
               isOnTable(world, i) ? "true" : "false");
    }
    destroy(world);
    return 0;
}

/* test_blocks.c */
#include <criterion/criterion.h>
#include "blocks.h"

Test(Blocks, CreateAndDestroy) {
    BlocksWorld *w = create(5);
    cr_assert_not_null(w);
    destroy(w);
}

Test(Blocks, IsOnTableInitially) {
    BlocksWorld *w = create(3);
    for (int m = 1; m <= 3; m++) cr_assert(isOnTable(w, m));
    cr_assert_not(isOnTable(w, 0));
    cr_assert_not(isOnTable(w, 4));
    destroy(w);
}

Test(Blocks, IsOpenInitially) {
    BlocksWorld *w = create(2);
    for (int m = 1; m <= 2; m++) cr_assert(isOpen(w, m));
    cr_assert(isOpen(w, 0));
    cr_assert_not(isOpen(w, 3));
    destroy(w);
}

Test(Blocks, MoveAndRelations) {
    BlocksWorld *w = create(3);
    move(w, 2, 1);
    cr_assert_eq(isOn(w, 2), 1);
    cr_assert(isAbove(w, 2, 1));
    cr_assert_not(isAbove(w, 1, 2));
    // Illegal move: 1 has 2 on top, so move should have no effect
    move(w, 1, 2);
    cr_assert_eq(isOn(w, 1), 0);
    destroy(w);
}
