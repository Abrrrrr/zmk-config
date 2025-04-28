//git reset --hard HEAD^

// cooking.h
#ifndef COOKING_H
#define COOKING_H
#include <stdbool.h>

// Basic ingredient with nutrition info per gram
typedef struct ingredient {
    char *name;
    int caloriesPerGram;
} ingredient;

// Linked list of ingredients in a recipe
typedef struct ingredientList {
    struct ingredient *ingredient;
    int quantity;
    struct ingredientList *next;
} ingredientList;

// Recipe contains name, servings, list of ingredients, and link to next recipe
typedef struct recipie {
    char *name;
    int servings;
    ingredientList *ingredients;
    struct recipie *next;
} recipie;

// Book is a collection of recipes
typedef struct book {
    recipie *recipes;
} book;

// Pantry entry pairs an ingredient with its stored quantity
typedef struct pantryEntry {
    struct ingredient *ingredient;
    int quantity;
    struct pantryEntry *next;
} pantryEntry;

// Pantry holds linked list of pantry entries
typedef struct pantry {
    pantryEntry *items;
} pantry;

// Constructors
book* newBook(void);
pantry* newPantry(void);
recipie* newRecipie(const char *name, int servings);
ingredient* newIngredient(const char *name, int caloriesPerGram);

// Modifiers
void addRecipie(book *b, recipie *r);
void addIngredient(recipie *r, ingredient *ingr, int quantity);
void storeIngredient(pantry *p, ingredient *ingr, int quantity);

// Queries
book* canMakeAny(pantry *p, book *b);
book* canMakeAll(pantry *p, book *b);
book* withinCalorieLimit(pantry *p, book *b, int limit);

#endif // COOKING_H


// cooking.c
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "cooking.h"

// Create an empty book
book* newBook(void) {
    book *b = malloc(sizeof(book));
    if (!b) return NULL;
    b->recipes = NULL;
    return b;
}

// Create an empty pantry
pantry* newPantry(void) {
    pantry *p = malloc(sizeof(pantry));
    if (!p) return NULL;
    p->items = NULL;
    return p;
}

// Create a new recipe with given name and servings
recipie* newRecipie(const char *name, int servings) {
    recipie *r = malloc(sizeof(recipie));
    if (!r) return NULL;
    r->name = strdup(name);
    r->servings = servings;
    r->ingredients = NULL;
    r->next = NULL;
    return r;
}

// Create a new ingredient with name and calories per gram
ingredient* newIngredient(const char *name, int caloriesPerGram) {
    ingredient *i = malloc(sizeof(ingredient));
    if (!i) return NULL;
    i->name = strdup(name);
    i->caloriesPerGram = caloriesPerGram;
    return i;
}

// Add a recipe to a book
void addRecipie(book *b, recipie *r) {
    if (!b || !r) return;
    r->next = b->recipes;
    b->recipes = r;
}

// Add an ingredient to a recipe
void addIngredient(recipie *r, ingredient *ingr, int quantity) {
    if (!r || !ingr) return;
    ingredientList *node = malloc(sizeof(ingredientList));
    if (!node) return;
    node->ingredient = ingr;
    node->quantity = quantity;
    node->next = r->ingredients;
    r->ingredients = node;
}

// Store an ingredient and its quantity in the pantry
void storeIngredient(pantry *p, ingredient *ingr, int quantity) {
    if (!p || !ingr) return;
    pantryEntry *cur = p->items;
    while (cur) {
        if (cur->ingredient == ingr) {
            cur->quantity += quantity;
            return;
        }
        cur = cur->next;
    }
    pantryEntry *node = malloc(sizeof(pantryEntry));
    if (!node) return;
    node->ingredient = ingr;
    node->quantity = quantity;
    node->next = p->items;
    p->items = node;
}

// Helper to check if pantry has enough of an ingredient
static bool hasEnough(pantry *p, ingredient *ingr, int needed) {
    pantryEntry *cur = p->items;
    while (cur) {
        if (cur->ingredient == ingr) {
            return cur->quantity >= needed;
        }
        cur = cur->next;
    }
    return false;
}

// Return a new book of all recipes that can be made individually from the pantry
book* canMakeAny(pantry *p, book *b) {
    if (!p || !b) return NULL;
    book *res = newBook();
    for (recipie *cur = b->recipes; cur; cur = cur->next) {
        bool ok = true;
        for (ingredientList *ing = cur->ingredients; ing; ing = ing->next) {
            if (!hasEnough(p, ing->ingredient, ing->quantity)) {
                ok = false;
                break;
            }
        }
        if (ok) {
            recipie *copy = newRecipie(cur->name, cur->servings);
            for (ingredientList *ing = cur->ingredients; ing; ing = ing->next) {
                addIngredient(copy, ing->ingredient, ing->quantity);
            }
            addRecipie(res, copy);
        }
    }
    return res;
}

// Return a book of all recipes only if the pantry can make ALL of them
book* canMakeAll(pantry *p, book *b) {
    if (!p || !b) return NULL;
    for (recipie *cur = b->recipes; cur; cur = cur->next) {
        for (ingredientList *ing = cur->ingredients; ing; ing = ing->next) {
            if (!hasEnough(p, ing->ingredient, ing->quantity)) {
                return NULL;
            }
        }
    }
    book *res = newBook();
    for (recipie *cur = b->recipes; cur; cur = cur->next) {
        recipie *copy = newRecipie(cur->name, cur->servings);
        for (ingredientList *ing = cur->ingredients; ing; ing = ing->next) {
            addIngredient(copy, ing->ingredient, ing->quantity);
        }
        addRecipie(res, copy);
    }
    return res;
}

// Return a book of recipes whose per-serving calories are below the limit
book* withinCalorieLimit(pantry *p, book *b, int limit) {
    if (!b) return NULL;
    book *res = newBook();
    for (recipie *cur = b->recipes; cur; cur = cur->next) {
        int totalCalories = 0;
        for (ingredientList *ing = cur->ingredients; ing; ing = ing->next) {
            totalCalories += ing->quantity * ing->ingredient->caloriesPerGram;
        }
        if (cur->servings > 0 && totalCalories / cur->servings < limit) {
            recipie *copy = newRecipie(cur->name, cur->servings);
            for (ingredientList *ing = cur->ingredients; ing; ing = ing->next) {
                addIngredient(copy, ing->ingredient, ing->quantity);
            }
            addRecipie(res, copy);
        }
    }
    return res;
}
