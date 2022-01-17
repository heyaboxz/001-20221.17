#ifndef __INIT_H__
#define __INIT_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sqlite3.h>

#define N 128
#define PORT 6666
#define IP "127.0.0.1"


    struct user_msg {
        char choose;
        char name[20];
        char phone[20];
        char addr[128];
        char salary[20];
        char age[10];
        char id[20];
        char pwd[20];
        char class;
        char stage;
        char notice[128];
    }__attribute__((packed));

#define ERR_MSG(msg) do{\
	printf("func=%s line=%d\n",__func__,__LINE__);\
	perror(msg);\
}while(0)


int do_recv(int newfd,struct sockaddr_in cin,sqlite3* pstaff_db);
int init_socket(int sfd);
sqlite3* init_sqlite(sqlite3** pstaff_db);

int add_staff(sqlite3* pstaff_db,int newfd,struct user_msg *msg); //insert info in table
int del_staff(sqlite3* pstaff_db,int newfd,struct user_msg *msg);
int modify_staff_info(sqlite3* pstaff_db,int newfd,struct user_msg *msg);

int account_register(sqlite3* pstaff_db,int newfd,struct user_msg *msg);  //注册

int sys_login(sqlite3* pstaff_db,int newfd,struct user_msg *msg);
int show_self_info(sqlite3* pstaff_db,int newfd,struct user_msg *msg);

int sys_quit(sqlite3* pstaff_db,int newfd,struct user_msg *msg);

int modify_staff_info_by_item(sqlite3* pstaff_db,int newfd,struct user_msg *msg);

#endif
