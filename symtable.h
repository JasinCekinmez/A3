#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED
#include <stddef.h>

/* For concision SymTable_T is defined to 
be a pointer to a struct SymTable*/
typedef struct SymTable *SymTable_T;

/* SymTable_new(void) takes in nothing (void) and returns 
a new symbol table SymTable_T object*/
SymTable_T SymTable_new(void);

/* SymTable_free takes in a SymTable_T, frees the dynamic memory
that the symbol table has and returns nothing (void) */
void SymTable_free(SymTable_T oSymTable);

/* SymTable_getLength takes in a SymTable_T and returns 
the size_t amount of bindings contained within oSymTable*/
size_t SymTable_getLength(SymTable_T oSymTable);

/*SymTable_put takes in a SymTable_T oSymTable, 
and the corresponding key, value
const char *pcKey, const void *pvValue
that you want to put in the symbol table
returns an int 1 if it was able to insert the pair into
the symbol table 0 otherwise */
int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue);

/* SymTable_replace takes in a ymTable_T oSymTable,
the corresponding key value pair 
const char *pcKey, const void *pvValue,
returns (void *) the previous value and replaces
that value with the new value pvValue else
nothing is changed and NULL is returned. 
*/
void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue);


/* SymTable_contains takes in a SymTable_T oSymTable
and a key const char *pcKey and returns an int 1 if the key 
is present in the symbol table else 0 otherwise 
*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* SymTable_get takes in a SymTable_T oSymTable
and a key const char *pcKey and returns a void * 
the corresponding value of pcKey if present in 
the symbol table else it returns NULL
*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* SymTable_remove takes in a SymTable_T oSymTable
and a key const char *pcKey and returns a void * 
the corresponding value of pcKey if present in the symbol table
and removes that binding from the symbol table else it returns NULL
*/
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);


/* SymTable_map takes in a SymTable_T oSymTable
function *pfApply(const char *pcKey, void *pvValue, void *pvExtra)
and an extra parameter const void *pvExtra that function is applied
to each binding and nothing (void) is returned 
*/
void SymTable_map(SymTable_T oSymTable,
void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra);

#endif