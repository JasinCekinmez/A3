#include <assert.h>
#include <stdlib.h>
#include "symtable.h"
#include <string.h>
struct Binding
{
   /* Key */
   const char *pcKey;

   /* Value */
   void * pvValue;

   /* The address of the next Binding. */
   struct Binding *psNextBinding;
};

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
    return oSymTable->length;
}




int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
    struct Binding *psCurrentBinding;
    struct Binding *psNextBinding;

    assert(oSymTable != NULL);

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

        for (psCurrentBinding = oSymTable->psFirstBinding;
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
        
        if (strcmp(psCurrentBinding->pcKey,pcKey)==0){
            void * temp = psCurrentBinding->pvValue;
            psNextBinding=psCurrentBinding->psNextBinding;
            free((void *)(psCurrentBinding->pcKey));
            free( psCurrentBinding);
            oSymTable->psFirstBinding = psNextBinding;
            oSymTable->length=oSymTable->length-1;
            return temp;
        }
        
        for (psPreviousBinding = oSymTable->psFirstBinding;
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

         for (psCurrentBinding = oSymTable->psFirstBinding;
            psCurrentBinding != NULL;
            psCurrentBinding = psCurrentBinding->psNextBinding)
          (*pfApply)((void*)psCurrentBinding->pcKey,(void*) 
          psCurrentBinding->pvValue,(void*)pvExtra);




    }






















