/* Linked List implementation of Symbol Table*/
#include <assert.h>
#include <stdlib.h>
#include "symtable.h"
#include <string.h>

/* Structure of Binding*/
struct Binding
{
   /* Key */
   const char *pcKey;

   /* Value */
   void * pvValue;

   /* The address of the next Binding. */
   struct Binding *psNextBinding;
};

/* How Symbol Table is set up*/
struct SymTable
{
   /* The number of bindings */
   size_t length; 

   /* The address of the first Binding. */
   struct Binding *psFirstBinding;
};

SymTable_T SymTable_new(void)
{
   SymTable_T oSymTable;
   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (oSymTable == NULL)
      return NULL;
   
   oSymTable->psFirstBinding = NULL;
   oSymTable->length = 0;
   return oSymTable;
}

void SymTable_free(SymTable_T oSymTable)
{
   struct Binding *psCurrentBinding;
   struct Binding *psNextBinding;

   assert(oSymTable != NULL);

   /* Runs through the entire linked list 
   and frees all the dynamically allocated keys and 
   the binding object*/
   for (psCurrentBinding = oSymTable->psFirstBinding;
        psCurrentBinding != NULL;
        psCurrentBinding = psNextBinding)
   {
      psNextBinding = psCurrentBinding->psNextBinding;
      free((void *)(psCurrentBinding->pcKey));
      free(psCurrentBinding);
   }

   free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable)
{
    assert(oSymTable != NULL);
    return oSymTable->length;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
    struct Binding *psCurrentBinding;
    struct Binding *psNextBinding;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    /* Loops through the entire linked list and checks
    if any of the keys match the key passed in
    */
    for (psCurrentBinding = oSymTable->psFirstBinding;
            psCurrentBinding != NULL;
            psCurrentBinding = psNextBinding)
    {
        
        if (strcmp(psCurrentBinding->pcKey,pcKey)==0){
            return 1;
        }
       
        
        psNextBinding = psCurrentBinding->psNextBinding;


    }
    return 0; 
}

int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue){

        struct Binding *psNewBinding;

        assert(oSymTable != NULL);

        /* Makes sure not to add a binding with the same key*/
        if (SymTable_contains(oSymTable, pcKey)==1)
            return 0;
        
        psNewBinding = (struct Binding *) malloc(sizeof(struct Binding));

        if (psNewBinding == NULL)
            return 0;

        /* Creates a new binding and moves the new binding to the beginning
        of the linked list*/
        psNewBinding->pcKey=strcpy(malloc(strlen(pcKey)+1),pcKey);
        /* Makes sure that we have enough room to allocate a new key*/
        if (psNewBinding->pcKey==NULL){
            free(psNewBinding);
            return 0;
        }
        psNewBinding->pvValue= (void *) pvValue;
        psNewBinding->psNextBinding=oSymTable->psFirstBinding;
        oSymTable->psFirstBinding=psNewBinding;
        oSymTable->length=oSymTable->length+1;
        return 1; 
    }

void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue){
        struct Binding *psCurrentBinding;
        struct Binding *psNextBinding;

        assert(oSymTable != NULL);
        /* Loop through the linked list until we find the key 
        and replace its value with the new value and return the old value*/
        for (psCurrentBinding = oSymTable->psFirstBinding;
                psCurrentBinding != NULL;
                psCurrentBinding = psNextBinding)
        {
            if (strcmp(psCurrentBinding->pcKey,pcKey)==0){
                void * OldValue = psCurrentBinding->pvValue;
                psCurrentBinding->pvValue= (void *) pvValue;
                return OldValue; 
            }
            psNextBinding = psCurrentBinding->psNextBinding;
        }
        return NULL;
    }

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
    struct Binding *psCurrentBinding;
    struct Binding *psNextBinding;

    assert(oSymTable != NULL);
    /* Same gist as SymTable_replace but this time we do not
    replace anything*/
    for (psCurrentBinding = oSymTable->psFirstBinding;
            psCurrentBinding != NULL;
            psCurrentBinding = psNextBinding)
    {
        
        if (strcmp(psCurrentBinding->pcKey,pcKey)==0){
            return psCurrentBinding->pvValue;
        }
            psNextBinding = psCurrentBinding->psNextBinding;
    }
    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
    struct Binding *psCurrentBinding;
    struct Binding *psPreviousBinding;
    struct Binding *psNextBinding;
    
    assert(oSymTable != NULL);

    psCurrentBinding=oSymTable->psFirstBinding;
    if(psCurrentBinding==NULL)
        return NULL;
    /* Did it by casework since first case is linked
    to oSymTable whereas the others we can just call
    psNextBinding */
    /* Essentially if the first binding in the linked list
    has the same key as the key passed in we remove it 
    and make the second binding the first one 
    and free the corresponding key and binding*/
    if (strcmp(psCurrentBinding->pcKey,pcKey)==0){
        void * value = psCurrentBinding->pvValue;
        psNextBinding=psCurrentBinding->psNextBinding;
        free((void *)(psCurrentBinding->pcKey));
        free( psCurrentBinding);
        oSymTable->psFirstBinding = psNextBinding;
        oSymTable->length=oSymTable->length-1;
        return value;
    }
    /* Same as above just now looping through the entire linked
    list*/
    for (psPreviousBinding = oSymTable->psFirstBinding;
            psPreviousBinding != NULL;
            psPreviousBinding = psCurrentBinding)
    {
        psCurrentBinding = psCurrentBinding->psNextBinding;
        if (psCurrentBinding==NULL)
            return NULL;
        if (strcmp(psCurrentBinding->pcKey,pcKey)==0){
            void * value = psCurrentBinding->pvValue;
            psNextBinding=psCurrentBinding->psNextBinding;
            free((void *) (psCurrentBinding->pcKey));
            free(psCurrentBinding);
            psPreviousBinding->psNextBinding=psNextBinding;
            oSymTable->length=oSymTable->length-1;
            return value; 
        }
    }
    return NULL;
}

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra){
        struct Binding *psCurrentBinding;
        assert(oSymTable != NULL);
        assert(pfApply != NULL);
        /* Applues the function for every binding*/
            for (psCurrentBinding = oSymTable->psFirstBinding;
            psCurrentBinding != NULL;
            psCurrentBinding = psCurrentBinding->psNextBinding)
                (*pfApply)((void*)psCurrentBinding->pcKey,(void*) 
                psCurrentBinding->pvValue,(void*)pvExtra);
    }






















