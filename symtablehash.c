#include <assert.h>
#include <stdlib.h>
#include "symtable.h"
#include <string.h>

static const int BucketSize[]={509, 1021, 2039, 4093, 8191, 16381, 32749, 65521};
static int BucketIndex = 0;
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

   
   struct Binding *buckets[BucketSize[BucketIndex]];
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

   oSymTable->buckets = (struct Binding [] *) calloc(BucketSize[BucketIndex],sizeof(struct Binding));
   oSymTable->length = 0;
   oSymTable->BucketSize=BucketSize[BucketIndex];
   return oSymTable;
}

void SymTable_free(SymTable_T oSymTable)
{
   struct Binding *psCurrentBinding;
   struct Binding *psNextBinding;
   size_t i;
   assert(oSymTable != NULL);
   for (i=0; i<BucketSize[BucketIndex]; i++){
    for (psCurrentBinding = oSymTable->buckets[i];
            psCurrentBinding != NULL;
            psCurrentBinding = psNextBinding)
    {
        if (psCurrentBinding->pcKey==0){
            free(psCurrentBinding);
            break;
        }
        
        psNextBinding = psCurrentBinding->psNextBinding;
        free((void *)(psCurrentBinding->pcKey));
        free(psCurrentBinding);
    }
}
   free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable){
    return oSymTable->length;
}


int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
    struct Binding *psCurrentBinding;
    struct Binding *psNextBinding;
    
    size_t hash = SymTable_hash(pcKey, BucketSize[BucketIndex]);

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

static SymTable_T Resize(SymTable_T oSymTable){
    struct Binding *psCurrentBinding;
    struct Binding *psNextBinding;
    size_t i;
    BucketIndex=BucketIndex+1;

    SymTable_T oSymTable_New;

    oSymTable_New = (SymTable_T)malloc(sizeof(struct SymTable_T));
    if (oSymTable_New == NULL)
        return NULL;

    oSymTable_New->buckets = (struct Binding *) calloc(BucketSize[BucketIndex],sizeof(struct Binding));
    oSymTable_New->length = oSymTable->length;
    oSymTable_New->BucketSize=BucketSize[BucketIndex];
    
    
    for (i=0; i<BucketSize[BucketIndex-1]; i++)
        for (psCurrentBinding = oSymTable->buckets[i];
                psCurrentBinding != NULL;
                psCurrentBinding = psNextBinding){
                    const char * CurrentKey=psCurrentBinding->pcKey;
                    hash=SymTable_hash(CurrentKey, BucketSize[BucketIndex])
                    psNewBinding = (struct Binding *) malloc(sizeof(struct Binding));
                    psNewBinding->pcKey = strcpy(malloc(strlen(CurrentKey)+1),CurrentKey);
                    psNewBinding->pvValue = psCurrentBinding->pvValue;
                    psNewBinding->psNextBinding=oSymTable_New->buckets[hash];
                    oSymTable_New->buckets[hash]=psNewBinding;
                    psNextBinding = psCurrentBinding->psNextBinding;
                }
        
    
    
    free(oSymTable);
    oSymTable=oSymTable_New;
    return oSymTable_New;


}

int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue){

        struct Binding *psNewBinding;

        assert(oSymTable != NULL);

        if(BucketSize[BucketIndex]+1>BucketSize[BucketIndex]){
            oSymTable=Resize(oSymTable);
        }
        
        if (SymTable_contains(oSymTable, pcKey)==1)
            return 0;
        
        psNewBinding = (struct Binding *) malloc(sizeof(struct Binding));

        if (psNewBinding == NULL)
            return 0;

        psNewBinding->pcKey=strcpy(malloc(strlen(pcKey)+1),pcKey);
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
        hash= SymTable_hash(pcKey, BucketSize[BucketIndex]);
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

        assert(oSymTable != NULL);
        hash= SymTable_hash(pcKey, BucketSize[BucketIndex]);
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

        assert(oSymTable != NULL);
        hash= SymTable_hash(pcKey, BucketSize[BucketIndex]);
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
            assert(oSymTable != NULL);
            assert(pfApply != NULL);
            for (i=0; i<BucketSize[BucketIndex]; i++)
                for (psCurrentBinding = oSymTable->buckets[i];
                    psCurrentBinding != NULL;
                    psCurrentBinding = psCurrentBinding->psNextBinding)
                (*pfApply)((void*)psCurrentBinding->pcKey,(void*) 
                psCurrentBinding->pvValue,(void*)pvExtra);




        }


