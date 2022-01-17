#include "init.h"

typedef void(*sighandler_t)(int);

void handler(int sig)
{
	while(waitpid(-1,NULL,WNOHANG)>0);
}

int main(int argc, const char *argv[])
{
	sighandler_t s = signal(SIGCHLD,handler);
	if(SIG_ERR == s){
	ERR_MSG("signal");
	return -1;
	}
	//数据库初始化
	sqlite3 *pstaff_db = NULL;
	init_sqlite(&pstaff_db);
	
	//网络初始化
	int sfd =0;
	
	sfd = init_socket(sfd);
	
	struct sockaddr_in cin;
	socklen_t addrlen = sizeof(cin);
	int newfd =0;
	//循环等待客户端连接
	
	
	while(1){
		//父进程专门用于连接
		newfd = accept(sfd,(struct sockaddr*)&cin,&addrlen);
		if(newfd<0){
			ERR_MSG("accept");
			return -1;
		}
	
		printf("[%s:%d]newfd=%d连接成功\n",inet_ntoa(cin.sin_addr),ntohs(cin.sin_port),newfd);

		//创建一个子进程，专门与客户端交互
		if(fork()==0){
			close(sfd);//子进程只做交互，所以sfd没有用处
			do_recv(newfd,cin,pstaff_db); //
			exit(0);
		}
		close(newfd);//由于父进程只需要连接，所以newfd没有用处
	}
	close(sfd);
	return 0;
}


int do_recv(int newfd,struct sockaddr_in cin,sqlite3* pstaff_db)
{
	ssize_t res = 0;
    struct user_msg msg;
    printf("newfd=%d\n",newfd);
	while(1){
        printf("循环等待操作请求__line=%d__ newfd = %d\n",__LINE__,newfd);
        memset(&msg,0,sizeof(msg));
		res = recv(newfd,&msg,sizeof(msg),0);
		if(res<0){
			ERR_MSG("recv");
			break;
		}
		else if(0 == res){
			printf("[%s:%d]断开连接\n",(char*)inet_ntoa(cin.sin_addr), ntohs(cin.sin_port));
			break;
		}
        printf("__line:%d__接受完成并解析msg:id=%s\n pwd=%s choose=%c\n",__LINE__,msg.id,msg.pwd,msg.choose);
		switch(msg.choose){
			case 'a':
		    add_staff(pstaff_db,newfd,&msg);break;
			
			case 'd':
			del_staff(pstaff_db,newfd,&msg);break;
			
			case 'm':
			modify_staff_info(pstaff_db,newfd,&msg);break;

            case 'i':
			modify_staff_info_by_item(pstaff_db,newfd,&msg);break;
			
			case 'l':
			sys_login(pstaff_db,newfd,&msg); break;
			
			case 'r':
			account_register(pstaff_db,newfd,&msg);break;

			case 'q':
			sys_quit(pstaff_db,newfd,&msg); break;
           
            case 's':
            show_self_info(pstaff_db,newfd,&msg); break;
			
		}	
	}
    return 0;
}










