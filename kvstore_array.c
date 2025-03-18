#include "kvstore.h"

array_t Array;


int kvs_array_create(array_t *array){
    if(!array) return -1;
    array->array_table = kv_malloc(ARRAY_SIZE * sizeof(struct kvs_array_item));
    if(!array->array_table) return -1;
    memset(array->array_table , 0 , ARRAY_SIZE * sizeof(struct kvs_array_item));
    array->array_idx = 0;
    return 0;
}
void kvs_array_destory(array_t *array){
    if(!array) return;
    if(!array->array_table) return;
    int i = 0;
    for(i = 0;i < array->array_idx;++i){
        if(array->array_table[i].key != NULL){
            kv_free(array->array_table[i].key);
            array->array_table[i].key = NULL;
            kv_free(array->array_table[i].value);
            array->array_table[i].value = NULL;
        }
    }
    kv_free(array->array_table);
}

int kvs_array_set(array_t *array , char *key , char *value){
    if(!array || !key || !value) return -1;
    if(array->array_idx == ARRAY_SIZE) return -1;
    char *pkey = kv_malloc(strlen(key) + 1);
    if(!pkey) return -1;
    strncpy(pkey , key , strlen(key) + 1);
    char *pvalue = kv_malloc(strlen(value) + 1);
    if(!pvalue){
        kv_free(pkey);
        return -1;
    }
    strncpy(pvalue , value , strlen(value) + 1);
    int i = 0;
    for(i = 0;i < array->array_idx;++i){
        if(array->array_table[i].key == NULL){
            array->array_table[i].key = pkey;
            array->array_table[i].value = pvalue;
            return 0;
        }
    }

    if(i < ARRAY_SIZE && i == array->array_idx){
        array->array_table[i].key = pkey;
        array->array_table[i].value = pvalue;
        ++array->array_idx;
    }
    return 0;
}


char * kvs_array_get(array_t *array , char *key){
    if(!array || !key) return NULL;
    int i = 0;
    for(i = 0;i < array->array_idx;++i){
        if(array->array_table[i].key == NULL) continue;
        if(strcmp(array->array_table[i].key , key) == 0){
            return array->array_table[i].value;
        }
    }
    return NULL;
}


int kvs_array_mod(array_t *array , char *key , char *value){
    if(!array || !key || !value) return -1;
    int i = 0;
    for(i = 0;i < array->array_idx;++i){
        if(array->array_table[i].key == NULL) continue;
        if(strcmp(array->array_table[i].key , key) == 0){
            char *pvalue = kv_malloc(strlen(value) + 1);
            if(!pvalue){
                return -1;
            }
            strncpy(pvalue , value , strlen(value) + 1);
            kv_free(array->array_table[i].value);
            array->array_table[i].value = pvalue;
            return 0;
        }
    }
    return -1;
}


int kvs_array_del(array_t *array , char *key){
    if(!array || !key) return -1;
    int i = 0;
    for(i = 0;i < array->array_idx;++i){
        if(array->array_table[i].key == NULL) continue;
        if(strcmp(array->array_table[i].key , key) == 0){
            kv_free(array->array_table[i].key);
            array->array_table[i].key = NULL;
            kv_free(array->array_table[i].value);
            array->array_table[i].value = NULL;
            if(i == array->array_idx - 1) --array->array_idx;
            return 0;
        }
    }
    return -1;
}



int kvs_array_count(array_t *array){
    if(!array) return -1;
    int count = 0;
    for(int i = 0;i < array->array_idx;++i){
        if(array->array_table[i].key == NULL) continue;
        ++count;
    }
    return count;
}