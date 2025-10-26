#include <assert.h>
#include <stdlib.h>
#include "symtable.h"
#include <string.h>

static const size_t BucketSize[]={509, 1021, 2039, 4093, 8191, 16381, 32749, 65521};
/*static int BucketIndex = 0;*/
struct Binding
{
   /* Key */
   const char *pcKey;

   /* Value */
   void * pvValue;

   /* The address of the next Binding. */
   struct Binding *psNextBinding;
};


struct SymTable_T
{
   /* The number of bindings */
   size_t length; 

   size_t BucketSize;

   size_t BucketIndex;

   
   struct Binding **buckets;
};

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
   SymTable_T oSymTable;

   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable_T));
   if (oSymTable == NULL)
      return NULL;

   oSymTable->buckets = (struct Binding **) calloc(oSymTable->BucketSize,sizeof(struct Binding*));
   oSymTable->length = 0;
   oSymTable->BucketIndex=0;
   oSymTable->BucketSize=BucketSize[oSymTable->BucketIndex];
   return oSymTable;
}

void SymTable_free(SymTable_T oSymTable)
{
   struct Binding *psCurrentBinding;
   struct Binding *psNextBinding;
   size_t i;
   assert(oSymTable != NULL);
   for (i=0; i<oSymTable->BucketSize; i++){
    for (psCurrentBinding = oSymTable->buckets[i];
            psCurrentBinding != NULL;
            psCurrentBinding = psNextBinding)
    {
        if (psCurrentBinding->pcKey==0){
            break;
        }
        
        psNextBinding = psCurrentBinding->psNextBinding;
        free((void *)(psCurrentBinding->pcKey));
        free(psCurrentBinding);
    }
}
    free(oSymTable->buckets);
    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable){
    return oSymTable->length;
}


int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
    struct Binding *psCurrentBinding;
    struct Binding *psNextBinding;
    
    size_t hash = SymTable_hash(pcKey, oSymTable->BucketSize);

    assert(oSymTable != NULL);

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

static void Resize(SymTable_T oSymTable){
    struct Binding *psCurrentBinding;
    struct Binding *psNextBinding;
    size_t i;
    struct Binding **bucketsNew;
    struct Binding **bucketsOld;
    size_t hash;
    struct Binding * temp;
    oSymTable->BucketIndex=oSymTable->BucketIndex+1;
    oSymTable->BucketSize=BucketSize[oSymTable->BucketIndex];
    bucketsNew = (struct Binding **)calloc(oSymTable->BucketSize,sizeof(struct Binding*));
    if (bucketsNew == NULL)
        return;
    
    for (i=0; i<BucketSize[oSymTable->BucketIndex-1]; i++)
        for (psCurrentBinding = oSymTable->buckets[i];
                psCurrentBinding != NULL;
                psCurrentBinding = psNextBinding){
                    psNextBinding = psCurrentBinding->psNextBinding;
                    hash = SymTable_hash(psCurrentBinding->pcKey, oSymTable->BucketIndex);
                    psCurrentBinding->psNextBinding = bucketsNew[hash];
                    bucketsNew[hash] = psCurrentBinding;
                    /*temp = bucketsNew[hash]->psNextBinding;
                    bucketsNew[hash]=psCurrentBinding;
                    bucketsNew[hash]->psNextBinding=temp;
                    psNextBinding = psCurrentBinding->psNextBinding;*/
                }
    bucketsOld=oSymTable->buckets;
    free(bucketsOld);
    oSymTable->buckets=bucketsNew;
}

int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue){

        struct Binding *psNewBinding;
        size_t hash;

        assert(oSymTable != NULL);

        if(oSymTable->BucketSize<oSymTable->length && oSymTable->length<65521){
            Resize(oSymTable);
            printf("resized");
        }
        
        if (SymTable_contains(oSymTable, pcKey)==1)
            return 0;
        
        psNewBinding = (struct Binding *) malloc(sizeof(struct Binding));

        if (psNewBinding == NULL)
            return 0;
        hash = SymTable_hash(pcKey, oSymTable->BucketSize);

        psNewBinding->pcKey=strcpy(malloc(strlen(pcKey)+1),pcKey);
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
        hash= SymTable_hash(pcKey, oSymTable->length);
        for (psCurrentBinding = oSymTable->buckets[hash];
                psCurrentBinding != NULL;
                psCurrentBinding = psNextBinding)
        {
            if (strcmp(psCurrentBinding->pcKey,pcKey)==0){
               void * temp = psCurrentBinding->pvValue;
               psCurrentBinding->pvValue= (void *) pvValue;
               return temp; 
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
        hash= SymTable_hash(pcKey, oSymTable->length);
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
        hash= SymTable_hash(pcKey, oSymTable->length);
        psCurrentBinding=oSymTable->buckets[hash];
        if(psCurrentBinding==NULL)
            return NULL;
        
        
        if (strcmp(psCurrentBinding->pcKey,pcKey)==0){
            void * temp = psCurrentBinding->pvValue;
            psNextBinding=psCurrentBinding->psNextBinding;
            free((void *)(psCurrentBinding->pcKey));
            free( psCurrentBinding);
            oSymTable->buckets[hash] = psNextBinding;
            oSymTable->length=oSymTable->length-1;
            return temp;
        }
        
        for (psPreviousBinding = oSymTable->buckets[hash];
                psPreviousBinding != NULL;
                psPreviousBinding = psCurrentBinding)
        {
            
            psCurrentBinding = psCurrentBinding->psNextBinding;
            if (psCurrentBinding==NULL)
                return NULL;
            if (strcmp(psCurrentBinding->pcKey,pcKey)==0){
               void * temp = psCurrentBinding->pvValue;
               psNextBinding=psCurrentBinding->psNextBinding;
               free((void *) (psCurrentBinding->pcKey));
               free(psCurrentBinding);
               psPreviousBinding->psNextBinding=psNextBinding;
               oSymTable->length=oSymTable->length-1;
               return temp; 
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
            for (i=0; i<oSymTable->BucketSize; i++)
                for (psCurrentBinding = oSymTable->buckets[i];
                    psCurrentBinding != NULL;
                    psCurrentBinding = psCurrentBinding->psNextBinding)
                (*pfApply)((void*)psCurrentBinding->pcKey,(void*) 
                psCurrentBinding->pvValue,(void*)pvExtra);




        }


