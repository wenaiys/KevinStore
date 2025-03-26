#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <getopt.h>

#include <sys/socket.h>
#include <sys/time.h>

#include <arpa/inet.h>
#define MAX_MSG_LENGTH 512
#define TIME_SUB_MS(tv1, tv2)  ((tv1.tv_sec - tv2.tv_sec) * 1000 + (tv1.tv_usec - tv2.tv_usec) / 1000)
int send_msg(int connfd , char *msg , int length){
    int ret = send(connfd , msg , length , 0);
    if(ret<0){
        perror("send");
        exit(1);
    }
    return ret;
}

int recv_msg(int connfd , char *msg , int length){
    int ret = recv(connfd , msg , length , 0);
    if(ret<0){
        perror("recv");
        exit(1);
    }
    return ret;
}
void equal(char *pattern , char *result , char *casename){
    if(strcmp(pattern , result) == 0){
        //printf("==> PASS --> %s\n", casename);
    }else{
        printf("failed -->%s , '%s' != '%s'\n" , casename , pattern , result);
    }
}

void test_case(int connfd , char *msg , char *pattern , char *casename){
    send_msg(connfd , msg , strlen(msg));
    char result[MAX_MSG_LENGTH] = {0};
    recv_msg(connfd , result , MAX_MSG_LENGTH);
    equal(pattern , result , casename);
}


void array_testcase(int connfd) {

	test_case(connfd, "SET Name King", "SUCCESS", "SETCase");
	test_case(connfd, "GET Name", "King", "GETCase");
	test_case(connfd, "MOD Name Darren", "SUCCESS", "MODCase");
	test_case(connfd, "GET Name", "Darren", "GETCase");
	test_case(connfd, "DEL Name", "SUCCESS", "DELCase");
	test_case(connfd, "GET Name", "NO EXIST", "GETCase");

}

void array_testcase_10w(int connfd) { // 10w

	int count = 100000;
	int i = 0;

	while (i ++ < count) {
		array_testcase(connfd);
	}

}

void rbtree_testcase(int connfd) {

	test_case(connfd, "RSET Name King", "SUCCESS", "SETCase");
	test_case(connfd, "RGET Name", "King", "GETCase");
	test_case(connfd, "RMOD Name Darren", "SUCCESS", "MODCase");
	test_case(connfd, "RGET Name", "Darren", "GETCase");
	test_case(connfd, "RDEL Name", "SUCCESS", "DELCase");
	test_case(connfd, "RGET Name", "NO EXIST", "GETCase");

}

void rbtree_testcase_10w(int connfd) { // 10w

	int count = 100000;
	int i = 0;

	while (i ++ < count) {
		array_testcase(connfd);
	}

}

int connect_tcp(char *ip , int post){
    int connfd = socket(AF_INET , SOCK_STREAM , 0);
    struct sockaddr_in tcpserver;
    tcpserver.sin_addr.s_addr = inet_addr(ip);
    tcpserver.sin_family = AF_INET;
    tcpserver.sin_port = htons(post);
    int ret = connect(connfd , (struct sockaddr*)&tcpserver , sizeof(struct sockaddr_in));
    if(ret){
        perror("conn");
        return -1;
    }
    return connfd;
}



// ./testcase -s 192.168.88.137 -p 2048 -m 1
int main(int argc , char *argv[]){
    char ip[16] = {0};
    int post = 0;
    int mode = 1;

    int opt;
    while((opt = getopt(argc , argv , "s:p:m:?"))!= -1){
        switch (opt){
        case 's':
            strcpy(ip , optarg);
            break;
        case 'p':
            post = atoi(optarg);
            break;
        case 'm':
            mode = atoi(optarg);
            break;
        
        default:
            return -1;
        }
    }
    int connfd = connect_tcp(ip , post);
    if (mode & 0x1) { // array
#if 1
		struct timeval tv_begin;
		gettimeofday(&tv_begin, NULL);
		
		array_testcase_10w(connfd);

		struct timeval tv_end;
		gettimeofday(&tv_end, NULL);

		int time_used = TIME_SUB_MS(tv_end, tv_begin);
		
		printf("array testcase--> time_used: %d, qps: %d\n", time_used, 600000 * 1000 / time_used);
#else
        array_testcase(connfd);
#endif
	}

	if (mode & 0x2) { // rbtree

		struct timeval tv_begin;
		gettimeofday(&tv_begin, NULL);
		
		//rbtree_testcase_10w(connfd);
		
		rbtree_testcase_10w(connfd);

		struct timeval tv_end;
		gettimeofday(&tv_end, NULL);

		int time_used = TIME_SUB_MS(tv_end, tv_begin);
		
		printf("rbtree testcase-->  time_used: %d, qps: %d\n", time_used, 600000 * 1000 / time_used);
	
	}
}