#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sqlite3.h>

typedef void (*sighandler_t)(int);

struct user_msg
{
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
} __attribute__((packed));

int sys_quit_c(int sfd);
int account_register_c(int sfd);
int sys_login_c(int sfd);
int modify_staff_info_c(int sfd);
int del_staff_c(int sfd);
int add_staff_c(int sfd);
int show_self_info_c(int sfd);
int iteam_modify_func();
int login_success_func(int sfd);


#define ERR_MSG(msg) do{\
        printf("%s :%d\n",__func__,__LINE__);\
        perror(msg);\
    }while(0)
#define PORT 6666
#define IP "127.0.0.1"
int ssfd = 0;


void handler(int sig)
{
	sys_quit_c(ssfd);
	close(ssfd);
	exit(0);
}
char iid[20]="";

int main(int argc, const char *argv[])
{

    char choose;
    int sfd =0;
    //1.创建字节流式套接字
    sfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sfd < 0){
        ERR_MSG("socket");
        return -1;
    }

    //允许端口快速重用
    int reuse = 1;

    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        ERR_MSG("socketopt");
        return -1;
    }

    //填充服务器的地址信息结构体
    struct sockaddr_in sin;
    sin.sin_family      = AF_INET;
    sin.sin_port        = htons(PORT);  //服务器的PORT
    sin.sin_addr.s_addr = inet_addr(IP);    //服务器的IP

    //连接服务器
    if(connect(sfd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        ERR_MSG("connect");
        return -1;
    }
    ssfd =sfd;
    sighandler_t s = signal(2, handler);
    if(s == SIG_ERR){
		ERR_MSG("signal");
		return -1;
	}

    printf("connect sucess sfd=%d\n", sfd);

    while(1){

        printf("请输入r:register **only for sys mgr.**\n");
        printf("请输入l:sys login \n");
        printf("请输入q:sys quit\n");
        choose = getchar();
        while(getchar() != 10);
        switch(choose)
        {
            case 'r':
                account_register_c(sfd);
                break;

            case 'l':
                if(sys_login_c(sfd)==0){
                    login_success_func(sfd);
                }
                break;

            case 'q':
                close(sfd);
                exit(0);
        }
    }

    return 0;
}

int login_success_func(int sfd)
{
    char choose ;
    while(1)
    {
        printf("请选择 a:add staff info **only for sys mgr.**\n");
        printf("请选择 d:delete staff info **only for sys mgr.**\n");
        printf("请选择 m:modify staff info **only for sys mgr.**\n");
        printf("请选择 s:show self info \n");
        printf("请选择 b:返回上级\n");
        printf("请选择 q:sys quit\n");
        choose = getchar();

        switch(choose)
        {
            case 'a':
                add_staff_c(sfd);
                continue;

            case 'd':
                printf("__%d__choose:%c\n",__LINE__,choose);
                del_staff_c(sfd);
                continue;

            case 's':
                show_self_info_c(sfd);
                continue;

            case 'm':
                modify_staff_info_c(sfd);
                continue;
            
            case 'b':
            return 0;

            case 'q':
            if(sys_quit_c(sfd)==0){
                exit(0);
            }
        }
    }
}


int show_self_info_c(int sfd)
{
    struct user_msg msg;
    memset(&msg, 0, sizeof(msg));
    int res = 0;
    msg.choose = 's' ;

    printf("请输入id:\n");
    scanf("%s", msg.id);
    while(getchar() != 10);
    if(send(sfd, &msg, sizeof(msg), 0) < 0){
        ERR_MSG("send");
        return -1;
    }

    printf("__%d__查看请求发送成功\n", __LINE__);
    //接受服务器端发送的提示信息
    bzero(&msg, sizeof(msg));
    res = recv(sfd, &msg, sizeof(msg), 0);
    if(res < 0){
        ERR_MSG("recv");
        return -1;
    }
    else if(0 == res){
        printf("服务器关闭\n");
		return -1;
    }
    printf("notice:%s\nname:%s\naddr:%s\nid:%s\nsalary:%s\nphone:%s\nage:%s\n",\
msg.notice, msg.name, msg.addr, msg.id, msg.salary,msg.phone, msg.age);
    return 0;
}

int sys_quit_c(int sfd)
{
    struct user_msg msg;
    bzero(&msg,sizeof(msg)); 
    int res = 0;
    msg.choose = 'q' ;
    strcpy(msg.id,iid);
    if(send(sfd, &msg, sizeof(msg), 0) < 0)
    {
        ERR_MSG("send");
        return -1;
    }

    //接受服务器端发送的提示信息
    bzero(&msg, sizeof(msg));
    res = recv(sfd, &msg, sizeof(msg), 0);

    if(res < 0)
    {
        ERR_MSG("send");
        return -1;
    }

    printf("%s\n", msg.notice);
    return 0;
}

int account_register_c(int sfd)
{
    int res = 0;
    struct user_msg msg;
    memset(&msg, 0, sizeof(struct user_msg));
    printf("请输入账号：\n");
    scanf("%s", msg.id);
    while(getchar() != 10);

    printf("请输入密码:\n");
    scanf("%s", msg.pwd);

    while(getchar() != 10);

    printf("请输入class:管理员1,other 0\n");
    msg.class = getchar();

    while(getchar() != 10);

    msg.choose = 'r' ;
    msg.stage = '1';
    printf("id即将发送id=%s choose=%c sfd=%d\n", msg.id, msg.choose, sfd);

    if(send(sfd, &msg, sizeof(msg), 0) < 0)
    {
        ERR_MSG("send");
        return -1;
    }

    printf("id发送成功id=%s sfd=%d\n", msg.id, sfd);
    //接受服务器端发送的提示信息
    bzero(&msg, sizeof(msg));
    res = recv(sfd, &msg, sizeof(msg), 0);

    if(res < 0)
    {
        ERR_MSG("send");
        return -1;
    }
    else
        if(0 == res)
        {
            printf("server is closed\n");
        }

    printf("notice=%s\n", msg.notice);
    return 0;
}

int sys_login_c(int sfd)
{
    struct user_msg msg;
    memset(&msg, 0, sizeof(struct user_msg));
    int res = 0;
    printf("请输入账号id：\n");
    scanf("%s", msg.id);
    while(getchar() != 10);
    strcpy(iid,msg.id);
    
    printf("请输入密码pwd:\n");
    scanf("%s", msg.pwd);
    while(getchar() != 10);

    msg.choose = 'l' ;
    printf("id即将发送登录信息id=%s choose=%c sfd=%d\n", msg.id, msg.choose, sfd);

    if(send(sfd, &msg, sizeof(struct user_msg), 0) < 0)
    {
        ERR_MSG("send");
        return -1;
    }

    printf("id发送成功，将会接受登录成功提示id=%s sfd=%d\n", msg.id, sfd);
    //接受服务器端发送的提示信息
    bzero(&msg, sizeof(msg));
    res = recv(sfd, &msg, sizeof(msg), 0);
    if(res < 0){
        ERR_MSG("send");
        return -1;
    }
    else if(0 == res){
        printf("server is closed\n");
    }
    //判断一下服务器发来的提示，如果是ID不存在，需要重新回到登录界面
    if(strncmp(msg.notice, "NOTEXIST", 8) == 0){
        printf("id接受成功：ID不存在，或者账号密码错误id=%s pwd=%s\n", msg.id, msg.pwd);
//        sys_login_c(sfd); 调用这句和return -1效果一样
        return -1;
    }

    printf("登录成功notice=%s\n", msg.notice);
    return 0;
}


int modify_staff_info_c(int sfd)
{
    int res =0;
    struct user_msg msg;
    bzero(&msg, sizeof(msg));
    printf("请输入需要修改人员的id\n");
    scanf("%s", msg.id);
    while(getchar() != 10);

    msg.choose = 'm';
    //先发送ID去验证有没有这个人
    if(send(sfd, &msg, sizeof(msg), 0) < 0)
    {
        ERR_MSG("send");
        return -1;
    }
    printf("__%d__ID已发送id:%s\n",__LINE__,msg.id);
    //接受服务器端发送的提示信息并判断
    bzero(&msg, sizeof(msg));
    res = recv(sfd, &msg, sizeof(msg), 0);
    if(res < 0)
    {
        ERR_MSG("send");
        return -1;
    }
    else if(0 == res)
    {
        printf("__%d__server is closed\n",__LINE__);
    }
    //如果存在这个ID，可以修改相关条款
    else if(strncmp(msg.notice, "1",1)==0){//id 存在，调用条款修改函数
        printf("__%d__notice:%s\n",__LINE__,msg.notice);
        iteam_modify_func(sfd,&msg);
        }
    return 0;
}

int del_staff_c(int sfd)
{

    int res = 0;
    struct user_msg msg;
    printf("清输入要删除人员的id\n");
    scanf("%s",msg.id);
    while(getchar() != 10);
    msg.choose = 'd' ;

    if(send(sfd, &msg, sizeof(struct user_msg), 0) < 0)
    {
        ERR_MSG("send");
        return -1;
    }
    printf("__%d__发送完毕\n",__LINE__);
    //接受服务器端发送的提示信息
    bzero(&msg, sizeof(msg));
    res = recv(sfd, &msg, sizeof(msg), 0);
    if(res < 0){
        ERR_MSG("send");
        return -1;
    }
    else if(0 == res){
        printf("__%d__server is closed\n",__LINE__);
   }
    printf("__%d__%s\n",__LINE__, msg.notice);
    return 0;
}

int add_staff_c(int sfd)
{
    struct user_msg msg;
    int res = 0;
    bzero(&msg, sizeof(msg));
    printf("输入id:\n");
    scanf("%s", msg.id);
    while(getchar() != 10);

    printf("输入name:\n");
    scanf("%s", msg.name);

    while(getchar() != 10);

    printf("输入phone:\n");
    scanf("%s", msg.phone);

    while(getchar() != 10);

    printf("输入addr\n");
    scanf("%s", msg.addr);

    while(getchar() != 10);

    printf("输入salary:\n");
    scanf("%s", (msg.salary));

    while(getchar() != 10);

    printf("输入age:\n");
    scanf("%s", (msg.age));

    while(getchar() != 10);

    msg.choose = 'a';
    printf("__%d__ sfd = %d res = %d\n", __LINE__, sfd, res);
    res = send(sfd, &msg, sizeof(struct user_msg), 0);
    if(res < 0){
        ERR_MSG("send");
        return -1;
    }

    printf("发送成功 : choose = %c id=%s\n", msg.choose, msg.id);
    //接受服务器端发送的提示信息
    printf("即将接受到你添加成功的信息choose=%c\n", msg.choose);
    bzero(&msg, sizeof(msg));
    res = recv(sfd, &msg, sizeof(msg), 0);
    if(res < 0){
        ERR_MSG("recv");
        return -1;
    }
    else if(0 == res){
        printf("server is closed\n");
    }
    printf("__%d__%s\n", __LINE__, msg.notice);
    return 0;
}


int iteam_modify_func(int sfd ,struct user_msg *msg)
{
    char iteam ;
    int res = 0;
    msg->choose = 'i';
    printf("__%d__,choose:%c\n",__LINE__,msg->choose);
    while(1){
        printf("请选择需要修改的条款\n");
        printf("请输入n:修改姓名\n");
        printf("请输入p:修改电话\n");
        printf("请输入s:修改薪资\n");
        printf("请输入a:修改年龄\n");
        printf("请输入d:修改地址\n");
        printf("请输入b:返回上级\n");
        scanf("%c", &iteam);
        while(getchar() != 10);

        switch(iteam)
        {
            case 'n':
            printf("请输入需要修改的name\n");
            scanf("%s", msg->name);
            while(getchar() != 10);            break;

            case 'p':
            printf("请输入需要修改的phone\n");
            scanf("%s", msg->phone);
            while(getchar() != 10);            break;
            
            case 'd':
            printf("请输入需要修改的addr\n");
            scanf("%s", msg->addr);
            while(getchar() != 10);            break;
            
            case 's':
            printf("请输入需要修改的的员salary\n");
            scanf("%s", (msg->salary));
            while(getchar() != 10);            break;
            
            case 'a':
            printf("请输入需要修改的age");
            scanf("%s", (msg->age));
            while(getchar() != 10);            break;

            case 'b':
            return 0;
        }

        if(send(sfd, msg, sizeof(*msg), 0) < 0){
            ERR_MSG("send");
            return -1;
        }

       //接受服务器端发送的提示信息
        bzero(msg, sizeof(*msg));
        res = recv(sfd, msg, sizeof(*msg), 0);
        if(res < 0){
            ERR_MSG("send");
            return -1;
        }
        else if(0 == res){
            printf("__%d__,server is closed\n",__LINE__);
        }
        printf("__%d__成功修改：%s\n",__LINE__, msg->notice);
        }
    return 0;
}





