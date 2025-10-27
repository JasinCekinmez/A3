/* Symbol table hash table implementation*/
#include <assert.h>
#include <stdlib.h>
#include "symtable.h"
#include <string.h>

/* The possible bucket sizes*/
static const size_t BucketSize[]={509, 1021, 2039, 4093, 8191, 16381, 32749, 65521};

/* Same structure as Binding before*/
struct Binding
{
   /* Key*/
   const char *pcKey;

   /* Value*/
   void * pvValue;

   /* The address of the next Binding.*/
   struct Binding *psNextBinding;
};

/* SymTable now has more parameters to allow for 
having linked lists in different buckets*/
struct SymTable
{
   /* The number of bindings*/
   size_t length; 

   /* The number of buckets*/
   size_t BucketSize;

   /* The current position in the BucketSize array
   corresponding to the current BucketSize*/
   /* Introduced to allow for not just one symbol table
   but many */
   size_t BucketIndex;

   /* Pointer to an array of pointer to bindings corresponding
   with the size BucketSize*/
   struct Binding **buckets;
};

/* Hash Function used to get the corresponding bucket
takes in const char *pcKey and size_t uBucketCount 
and returns the corresponding  size_t hash*/
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

SymTable_T SymTable_new(void)
{
   /* Creates oSymTable and makes sure it is not pointing to NULL*/
   SymTable_T oSymTable;
   oSymTable = malloc(sizeof(struct SymTable));
   if (oSymTable == NULL){
    return NULL;
   }
   
   /* Defines the various members of the struct */
   oSymTable->length = 0;
   oSymTable->BucketIndex=0;
   oSymTable->BucketSize=BucketSize[oSymTable->BucketIndex];
   oSymTable->buckets = calloc(oSymTable->BucketSize,sizeof(struct Binding*));
   if (oSymTable->buckets==NULL){
    free(oSymTable);
    return NULL;
   }
   return oSymTable;
}

void SymTable_free(SymTable_T oSymTable)
{
   struct Binding *psCurrentBinding;
   struct Binding *psNextBinding;
   size_t i;
   assert(oSymTable != NULL);
   
   /* Loops through every bucket and every Binding
   in each bucket and frees the key and binding*/
   for (i=0; i<oSymTable->BucketSize; i++){
    for (psCurrentBinding = oSymTable->buckets[i];
            psCurrentBinding != NULL;
            psCurrentBinding = psNextBinding)
    {
        psNextBinding = psCurrentBinding->psNextBinding;
        free((void *)psCurrentBinding->pcKey);
        free(psCurrentBinding);
    }
}
    /* Frees the remaning part of oSymTable*/
    free(oSymTable->buckets);
    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable){
    assert(oSymTable!=NULL);
    return oSymTable->length;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
    struct Binding *psCurrentBinding;
    struct Binding *psNextBinding;
    size_t hash = SymTable_hash(pcKey, oSymTable->BucketSize);

    assert(oSymTable != NULL);
    /* Loops through the linked list of the corresponding bucket
    and stops if it finds the matching key*/
    for (psCurrentBinding = oSymTable->buckets[hash];
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

/* Resize is a helper function that takes in 
SymTable_T oSymTable and what it does it 
modifies the buckets part of SymTable
it expands the bucket size to the next value in
BucketSize and repositions the bindings accordingly in
this new buckets and returns nothing (void)
*/
static void SymTable_Resize(SymTable_T oSymTable){
    struct Binding *psCurrentBinding;
    struct Binding *psNextBinding;
    size_t i;
    struct Binding **bucketsNew;
    struct Binding **bucketsOld;
    size_t hash;
    /* Updates the symbol table with the new bucket size and index
    and callocs a new bucket*/
    
    oSymTable->BucketIndex=(oSymTable->BucketIndex)+1;
    oSymTable->BucketSize=BucketSize[oSymTable->BucketIndex];
    bucketsNew = calloc(oSymTable->BucketSize,sizeof(struct Binding*));
    if (bucketsNew == NULL)
        return;
    
    /* Loops through every bucket and each linkedlist in each bucket
    and repositions the bindings into the new buckets in place*/
    for (i=0; i<BucketSize[(oSymTable->BucketIndex)-1]; i++)
        for (psCurrentBinding = oSymTable->buckets[i];
                psCurrentBinding != NULL;
                psCurrentBinding = psNextBinding){
                    psNextBinding = psCurrentBinding->psNextBinding;
                    hash = SymTable_hash(psCurrentBinding->pcKey, 
                        oSymTable->BucketSize);
                    psCurrentBinding->psNextBinding = bucketsNew[hash];
                    bucketsNew[hash] = psCurrentBinding;
                }
    
    /* Frees the old buckets and has the oSymTable point to the New Buckets*/
    bucketsOld=oSymTable->buckets;
    free(bucketsOld);
    oSymTable->buckets=bucketsNew;
}

int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue){
        struct Binding *psNewBinding;
        size_t hash;
        size_t bucketMax; 
        assert(oSymTable != NULL);

        /* If the current number of bindings is greater than the current number
        of buckets and number of bindings is less than bucketMax we resize buckets */
        bucketMax=65521;
        if(oSymTable->BucketSize<oSymTable->length && oSymTable->length<bucketMax){
            SymTable_Resize(oSymTable);
        }
        
        if (SymTable_contains(oSymTable, pcKey)==1)
            return 0;
        
        psNewBinding = malloc(sizeof(struct Binding));
        if (psNewBinding == NULL){
            return 0;
        }
        
        /* Puts the new binding at the beginning of the corresponding
        bucket linked list */
        hash = SymTable_hash(pcKey, oSymTable->BucketSize);
        psNewBinding->pcKey=strcpy(malloc(strlen(pcKey)+1),pcKey);
        if (psNewBinding->pcKey==NULL){
            free(psNewBinding);
            return 0;
        }
        psNewBinding->pvValue= (void *) pvValue;
        psNewBinding->psNextBinding=oSymTable->buckets[hash];
        oSymTable->buckets[hash]=psNewBinding;
        oSymTable->length=oSymTable->length+1;
        return 1; 
    }

void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue){
        struct Binding *psCurrentBinding;
        struct Binding *psNextBinding;
        size_t hash;
        assert(oSymTable != NULL);
        hash = SymTable_hash(pcKey, oSymTable->BucketSize);
        
        /* Loop through the corresponding linked list until we find the key 
        and replace its value with the new value and return the old value*/
        for (psCurrentBinding = oSymTable->buckets[hash];
                psCurrentBinding != NULL;
                psCurrentBinding = psNextBinding)
        {
            if (strcmp(psCurrentBinding->pcKey,pcKey)==0){
               void * OldValue = psCurrentBinding->pvValue;
               psCurrentBinding->pvValue = (void *) pvValue;
               return OldValue; 
            }
            psNextBinding = psCurrentBinding->psNextBinding;
        }
        return NULL;
    }

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
    struct Binding *psCurrentBinding;
    struct Binding *psNextBinding;
    size_t hash;
    
    assert(oSymTable != NULL);
    hash= SymTable_hash(pcKey, oSymTable->BucketSize);
    
    /* Same gist as SymTable_replace but this time we do not
    replace anything*/
    for (psCurrentBinding = oSymTable->buckets[hash];
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
    size_t hash;
    
    assert(oSymTable != NULL);
    hash= SymTable_hash(pcKey, oSymTable->BucketSize);
    psCurrentBinding=oSymTable->buckets[hash];
    if(psCurrentBinding==NULL)
        return NULL;
    
    /* Did it by casework since first case is linked
    to buckets whereas the others we can just call
    psNextBinding */
    /* Essentially if the first binding in the linked list
    has the same key as the key passed in we remove it 
    and make the second binding the first one 
    and free the corresponding key and binding*/
    if (strcmp(psCurrentBinding->pcKey,pcKey)==0){
        void * value = psCurrentBinding->pvValue;
        psNextBinding=psCurrentBinding->psNextBinding;
        free((void *)(psCurrentBinding->pcKey));
        free(psCurrentBinding);
        oSymTable->buckets[hash] = psNextBinding;
        oSymTable->length=oSymTable->length-1;
        return value;
    }
    
    /* Same as above just now looping through the entire linked
    list*/
    for (psPreviousBinding = oSymTable->buckets[hash];
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
        size_t i;
        
        assert(oSymTable != NULL);
        assert(pfApply != NULL);
        
        /* Applies the function to each binding*/
        for (i=0; i<oSymTable->BucketSize; i++)
            for (psCurrentBinding = oSymTable->buckets[i];
                psCurrentBinding != NULL;
                psCurrentBinding = psCurrentBinding->psNextBinding)
            (*pfApply)((void*)psCurrentBinding->pcKey,(void*) 
            psCurrentBinding->pvValue,(void*)pvExtra);
    }


