#include "init.h"

//网络初始化
int init_socket(int sfd)
{
	//1.创建字节流式套接字
	sfd = socket(AF_INET,SOCK_STREAM,0);
	if(sfd<0){
		ERR_MSG("socket");
		return -1;
	}
	//允许端口快速重用
	int reuse = 1;
	if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0){
		ERR_MSG("socketopt");
		return -1;
	}
	//绑定IP和端口
	struct sockaddr_in sin;
	sin.sin_family      = AF_INET;
	sin.sin_port        = htons(PORT);
	sin.sin_addr.s_addr = inet_addr(IP);
	
	if(bind(sfd,(struct sockaddr*)&sin,sizeof(sin))<0){
		ERR_MSG("bind");
		return -1;
	}
	printf("bind success\n");
	
	//listen 设置被动监听状态
	if(listen(sfd,10)<0){
		ERR_MSG("listen");
		return -1;
	}
	printf("listen success\n");
	return sfd;
}

//数据库初始化
sqlite3* init_sqlite(sqlite3** pstaff_db)
{
	char sql[N]="";
	char *errmsg = NULL;
	//创建数据库
	if(sqlite3_open("./staff.db",pstaff_db)){
		printf("sqlite3_open:%s %d\n", sqlite3_errmsg(*pstaff_db), __LINE__);
		return NULL;
	}
	printf("staff.db was create\n");
	
	//创建员工资料信息表info_list->id，name名字 phone手机号 地址addr 薪资salary 年龄age
	sprintf(sql,"create table if not exists info_list(id char primary key,name char,phone char,addr char,salary char,age char);");
	if((sqlite3_exec(*pstaff_db,sql,NULL,NULL,&errmsg))!=0){
		printf("sqlite3_exec:%s %d\n", errmsg, __LINE__);
		return NULL;
	}
	//创建管理系统用户信号表account_list/普通用户，管理员 class(1) account pwd
	sprintf(sql,"create table if not exists account_list(id char primary key,pwd char,class char,stage char);");
	if((sqlite3_exec(*pstaff_db,sql,NULL,NULL,&errmsg))!=0){
		printf("sqlite3_exec:%s %d\n", errmsg, __LINE__);
		return NULL;
	}

    //需要插入一个管理员账号，方便后注册的时候判断
    /*bzero(sql,sizeof(sql));
    sprintf(sql, "insert into account_list values(111,111,1,1)");
    if((sqlite3_exec(*pstaff_db,sql,NULL,NULL,&errmsg))!=0){
		printf("sqlite3_exec:%s %d\n", errmsg, __LINE__);
		return NULL;
	}*/
	return *pstaff_db;
}







