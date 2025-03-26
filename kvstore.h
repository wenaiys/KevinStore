#ifndef __KVSTORE_H__
#define __KVSTORE_H__


#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <unistd.h>

#include <pthread.h>
#include <sys/poll.h>
#include <sys/epoll.h>
#include <sys/time.h>

#define BUFFER_LENGTH		512
typedef int (*RCALLBACK)(int fd);
struct conn_item {
	int fd;
	
	char rbuffer[BUFFER_LENGTH];
	int rlen;
	char wbuffer[BUFFER_LENGTH];
	int wlen;

	union {
		RCALLBACK accept_callback;
		RCALLBACK recv_callback;
	} recv_t;
	RCALLBACK send_callback;
};

#define NETWORK_EPOLL 0
#define NETWORK_NTYCO		1
#define NETWORK_IOURING		2

#define ENABLE_NETWORK_SELECT	NETWORK_NTYCO


#define ENABLE_ARRAY_ENGINE  1
#define ENABLE_RBTREE_ENGINE 1
int kvstore_request(struct conn_item* item);

int reactor_entry();
int ntyco_entry();
void *kv_malloc(size_t size);
void kv_free(void *ptr);

#if ENABLE_ARRAY_ENGINE

#define ARRAY_SIZE 1024

struct kvs_array_item{
	char * key;
	char * value;
};

typedef struct array_s {
	struct kvs_array_item *array_table;
	int array_idx;
} array_t;

extern array_t Array;


int kvs_array_create(array_t *array);
void kvs_array_destory(array_t *array);

int kvs_array_set(array_t *array , char *key , char *value);
char * kvs_array_get(array_t *array , char *key);
int kvs_array_mod(array_t *array , char *key , char *value);
int kvs_array_del(array_t *array , char *key);

int kvs_array_count(array_t *array);

#endif
#if ENABLE_RBTREE_ENGINE
typedef struct _rbtree rbtree_t;
extern rbtree_t Tree;
int kvs_rbtree_create(rbtree_t *tree);
void kvs_rbtree_destory(rbtree_t *tree);

int kvs_rbtree_set(rbtree_t *tree , char *key , char *value);
char * kvs_rbtree_get(rbtree_t *tree , char *key);
int kvs_rbtree_mod(rbtree_t *tree , char *key , char *value);
int kvs_rbtree_del(rbtree_t *tree , char *key);

int kvs_rbtree_count(rbtree_t *tree);


#endif


#endif