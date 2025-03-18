#include "kvstore.h"
#define KVSTORE_MAX_TOKENS 128
const char *commands[] = {
	"SET", "GET", "DEL", "MOD", "COUNT",
	"RSET", "RGET", "RDEL", "RMOD", "RCOUNT",
	"HSET", "HGET", "HDEL", "HMOD", "HCOUNT",
};

enum {
	KVS_CMD_START = 0,
	KVS_CMD_SET = KVS_CMD_START,
	KVS_CMD_GET,
	KVS_CMD_DEL,
	KVS_CMD_MOD,
	KVS_CMD_COUNT,
	
	KVS_CMD_RSET,
	KVS_CMD_RGET,
	KVS_CMD_RDEL,
	KVS_CMD_RMOD,
	KVS_CMD_RCOUNT,

	KVS_CMD_HSET,
	KVS_CMD_HGET,
	KVS_CMD_HDEL,
	KVS_CMD_HMOD,
	KVS_CMD_HCOUNT,
	
	KVS_CMD_SIZE,
};
void *kv_malloc(size_t size){
    return malloc(size);
}
void kv_free(void *ptr){
    free(ptr);
}
#if ENABLE_ARRAY_ENGINE

int kvstore_array_set(char *key, char *value) {
	return kvs_array_set(&Array, key, value);

}
char *kvstore_array_get(char *key) {

	return kvs_array_get(&Array, key);

}
int kvstore_array_delete(char *key) {
	return kvs_array_del(&Array, key);
}
int kvstore_array_modify(char *key, char *value) {
	return kvs_array_mod(&Array, key, value);
}
int kvstore_array_count(void) {
	return kvs_array_count(&Array);
}


#endif

int kvstore_parser_protocol(struct conn_item* item , char **tokens , int count){
    if(!item || !tokens[0] || count == 0) return -1;
    int cmd = KVS_CMD_START;
    for(cmd = KVS_CMD_START;cmd < KVS_CMD_SIZE;++cmd){
        if(strcmp(tokens[0] ,commands[cmd]) == 0){
            break;
        }
    }
    char *msg = item->wbuffer;
    char *key = tokens[1];
    char *value = tokens[2];
    memset(msg , 0 , BUFFER_LENGTH);
    switch (cmd){
    case KVS_CMD_SET:{
        int res = kvstore_array_set(key, value);
		if (!res) {
			snprintf(msg, BUFFER_LENGTH, "SUCCESS");
		} else {
			snprintf(msg, BUFFER_LENGTH, "FAILED");
        }
    }
        break;
    case KVS_CMD_GET:{
        char *val = kvstore_array_get(key);
        if(val){
            snprintf(msg, BUFFER_LENGTH, "%s",val);
        }else{
            snprintf(msg, BUFFER_LENGTH, "NO EXIST");
        }
    }
        break;
    case KVS_CMD_DEL:{
        int res = kvstore_array_delete(key);
        if (res < 0) {  // server
            snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
        } else if (res == 0) {
            snprintf(msg, BUFFER_LENGTH, "%s", "SUCCESS");
        } else {
            snprintf(msg, BUFFER_LENGTH, "NO EXIST");
        }
    }
        break;
    case KVS_CMD_MOD:{
        int res = kvstore_array_modify(key, value);
        if (res < 0) {  // server
            snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
        } else if (res == 0) {
            snprintf(msg, BUFFER_LENGTH, "%s", "SUCCESS");
        } else {
            snprintf(msg, BUFFER_LENGTH, "NO EXIST");
        }
    }
        break;
    case KVS_CMD_COUNT:{
        int count = kvstore_array_count();
        if (count < 0) {  // server
            snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
        } else {
            snprintf(msg, BUFFER_LENGTH, "%d", count);
        }
    }
        break;
    default:{
        printf("cmd: %s\n", commands[cmd]);
		assert(0);
        break;
    }
    }
}
int kvstore_split_token(char *msg , char **tokens){
    if(!msg || !tokens) return -1;
    int count = 0;
    char *token = strtok(msg , " ");
    while(token != NULL){
        tokens[count++] = token;
        token = strtok(NULL , " ");
    }
    return count;
}
int kvstore_request(struct conn_item* item){
    char *msg = item->rbuffer;
    char *tokens[KVSTORE_MAX_TOKENS];
    int count = kvstore_split_token(msg , tokens);
    kvstore_parser_protocol(item, tokens, count);
    return 0;
}

int init_engine(void){
#if ENABLE_ARRAY_ENGINE
    kvs_array_create(&Array);
#endif
}
int exit_engine(void){
#if ENABLE_ARRAY_ENGINE
    kvs_array_destory(&Array);
#endif
}


int main(){
    init_engine();
    
#if (ENABLE_NETWORK_SELECT == NETWORK_EPOLL)
    
    reactor_entry();
#elif (ENABLE_NETWORK_SELECT == NETWORK_NTYCO)

#elif (ENABLE_NETWORK_SELECT == NETWORK_IOURING)

#endif
    exit_engine();

}