#include "init.h"


int add_staff(sqlite3* pstaff_db,int newfd,struct user_msg *msg)
{
	char **presult;
	int row = 0;
	int column = 0;
	char sql[600]="";
    char *errmsg=NULL;
	//遍历查找是否存在，如果存在就无需添加
	sprintf(sql, "select * from info_list where id=\"%s\" ;", msg->id);
	if(sqlite3_get_table(pstaff_db,sql,&presult,&row,&column,&errmsg)!=0){
		//(!=0)就是调用失败，返回错误码
		printf("sqlite3_get_table: %s 	__%d__\n", errmsg, __LINE__);
		return -1;
	}
    printf("__%d__row:%d\n",__LINE__,row);
	if(1 == row){
		//id 存在
		bzero(msg->notice,sizeof(msg->notice));
		strcpy(msg->notice, "**ID info EXISTS,你可以修改**");
		if(send(newfd,msg,sizeof(*msg),0)<0){
			ERR_MSG("send");
			return -1;
		}
	}
	else {
		//判断是否为管理员
	//	printf("__%d__roe:%d presult[(row+1)*column-2]:%s\n",__LINE__,row,presult[(row+1)*column-2]);
	//	if(strcmp(presult[(row+1)*column-2],"1")==0){
			bzero(sql,sizeof(sql));
			sprintf(sql, "insert into info_list values(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\")",\
			msg->id,msg->name,msg->phone,msg->addr,msg->salary,msg->age);
			if((sqlite3_exec(pstaff_db,sql,NULL,NULL,&errmsg))!=0){
				printf("sqlite3_exec :%s __%d__",errmsg,__LINE__);
				return -1;
			}
			printf("add info success\n");
			//给客户端发送提示信息
			bzero(msg,sizeof(*msg));
			strcat(msg->notice,"*add info success*");
			if(send(newfd,msg,sizeof(*msg),0)<0){
				ERR_MSG("send");
				return -1;
			}
		//}
	}

	return 0;
}


int del_staff(sqlite3* pstaff_db,int newfd,struct user_msg *msg)
{
    char sql[128]="";
    char **presult;
	int row = 0;
	int column = 0;
    char *errmsg=NULL;
	bzero(sql,sizeof(sql));
	sprintf(sql, "select * from info_list where id=\"%s\" ;", msg->id);
	if(sqlite3_get_table(pstaff_db,sql,&presult,&row,&column,&errmsg)!=0){
		//(!=0)就是调用失败，返回错误码
		printf("sqlite3_get_table: %s 	__%d__\n", errmsg, __LINE__);
		return -1;
	}
	if(1 == row){
		//id 存在，可以删除
		sprintf(sql, "delete from info_list where id=\"%s\"",msg->id);
		if((sqlite3_exec(pstaff_db,sql,NULL,NULL,&errmsg))!=0){
			printf("sqlite3_exec :%s __%d__\n",errmsg,__LINE__);
			return -1;
		}
		printf("del info success\n");

		bzero(msg,sizeof(*msg));
		strcat(msg->notice,"*del info success*");
		if(send(newfd,msg,sizeof(*msg),0)<0){   	//给客户端发送提示信息
			ERR_MSG("send");
			return -1;
		}
	}
	return 0;
}


int modify_staff_info(sqlite3* pstaff_db,int newfd,struct user_msg *msg)
{
	char **presult;
	int row = 0;
	int column = 0;
    char sql[520]="";
    char *errmsg=NULL;
    printf("收到即要修改的ID:%s\n",msg->id);
	//遍历查找是否存在，如果存在则修改，不存在提示输入错误
	sprintf(sql, "select * from info_list where id=\"%s\" ;", msg->id);
	if(sqlite3_get_table(pstaff_db,sql,&presult,&row,&column,&errmsg)!=0){
		//!=0就是调用失败，返回错误码
		printf("sqlite3_get_table: %s 	__%d__\n", errmsg, __LINE__);
		return -1;
	}
	if(0 == row){
		//没有找到，给客户端发送提示信息，id 不存在
		bzero(msg->notice,sizeof(msg->notice));
		strcpy(msg->notice, "0 ID NOT EXISTS**");
	}
	else if(row == 1){
        bzero(msg->notice,sizeof(msg->notice));
		strcpy(msg->notice, "1 ID EXISTS**");
    }
    if(send(newfd,msg,sizeof(*msg),0)<0){
    	ERR_MSG("send");
    	return -1;
    }
    printf("_——%d__找到要修改的ID:%s\n",__LINE__,msg->id);
    return 0;
    }

int modify_staff_info_by_item(sqlite3* pstaff_db,int newfd,struct user_msg *msg)
{   
    char **presult;
	int row = 0;
	int column = 0;
    char sql[520]="";
    char *errmsg=NULL;
	//遍历查找id
	sprintf(sql, "select * from info_list where id=\"%s\" ;", msg->id);
	if(sqlite3_get_table(pstaff_db,sql,&presult,&row,&column,&errmsg)!=0){
		//!=0就是调用失败，返回错误码
		printf("sqlite3_get_table: %s 	__%d__\n", errmsg, __LINE__);
		return -1;
	}
	//判断是否为管理员需要在登录的时候判断比较方便,并且row==1
	if(1==row){
        if(strlen(msg->name)!=0){
        bzero(sql,sizeof(sql));
        sprintf(sql, "update info_list set name=\"%s\" where id =\"%s\"" ,msg->name,msg->id);
        }
        if(strlen(msg->phone)!=0){
        bzero(sql,sizeof(sql));
        sprintf(sql, "update info_list set phone=\"%s\" where id =\"%s\"", msg->phone,msg->id);
        }
        if(strlen(msg->salary)!=0){
        bzero(sql,sizeof(sql));
        sprintf(sql, "update info_list set salary=\"%s\" where id =\"%s\"",msg->salary,msg->id);
        }
        if(strlen(msg->age)!=0){
        bzero(sql,sizeof(sql));
        sprintf(sql, "update info_list set age=\"%s\" where id =\"%s\"",msg->age,msg->id);
        }
        if(strlen(msg->addr)!=0){
        bzero(sql,sizeof(sql));
        sprintf(sql, "update info_list set addr=\"%s\" where id =\"%s\" ",msg->addr,msg->id);
        }
        if((sqlite3_exec(pstaff_db,sql,NULL,NULL,&errmsg))!=0){
        	printf("sqlite3_exec :%s __%d__",errmsg,__LINE__);
        	return -1;
        }
		//给客户端发送提示信息
		bzero(msg->notice,sizeof(msg->notice));
		strcat(msg->notice,"*modify info success*");
		if(send(newfd,msg,sizeof(*msg),0)<0){
			ERR_MSG("send");
			return -1;
		}
        printf("__%d__modify info success notice:%s phone:%s\n",__LINE__,msg->notice,msg->phone);
	}	
	return 0;
}



//员工注册，把账号密码保存到数据库
int account_register(sqlite3* pstaff_db,int newfd,struct user_msg *msg)
{
    printf("__%d__**************msg->id=%s\n",__LINE__,msg->id);

	char **presult;
	int row = 0;
	int column =0;
    char sql[120]="";
    char *errmsg=NULL;
	//根据id遍历查找是否存在，如果不存在保存到account_list
	sprintf(sql, "select * from account_list where id=\"%s\" ;",msg->id);
	if(sqlite3_get_table(pstaff_db,sql,&presult,&row,&column,&errmsg)!=0){
		//就是调用失败，返回错误码(!=0)
		printf("sqlite3_get_table: %s 	__%d__\n", errmsg, __LINE__);
		return -1;
	}
    printf("__%d__**************\n",__LINE__);
    //printf("__%d__row=%d column=%d presult[1]=%s\n",__LINE__,row,column,presult[1]);
    if(row==1){
		//发送账号已经存在
		bzero(msg->notice,sizeof(msg->notice));
		strcpy(msg->notice, "**账号已经存在**");
		if(send(newfd,msg,sizeof(*msg),0)<0){
			ERR_MSG("send");
			return -1;
		}
        printf("__%d__noitce=%s\n",__LINE__,msg->notice);
	}
	else{
		//没有找到，可以注册，
		printf("__%d__没有找到，可以注册id=%s\n",__LINE__,msg->id);
		bzero(sql,sizeof(sql));
		sprintf(sql, "insert into account_list values(\"%s\",\"%s\",\"%c\",1);", msg->id,msg->pwd,msg->class);
		if(sqlite3_exec(pstaff_db, sql, NULL, NULL, &errmsg) != 0){
			printf("sqlite3_exec:%s __%d__\n", errmsg, __LINE__);
		}
		else{
			bzero(msg->notice,sizeof(msg->notice));
			strcpy(msg->notice, "**注册成功**");
			if(send(newfd,msg,sizeof(*msg),0)<0){
				ERR_MSG("send");
				return -1;
			}
            printf("__%d__id发送成功notice=%s\n",__LINE__,msg->notice);
		}
	}
	
	return 0;
}
//员工登录
int sys_login(sqlite3* pstaff_db,int newfd,struct user_msg *msg)
{
	char **presult;
	int row = 0;
	int column = 0;
    char sql[120]="";
    char *errmsg=NULL;
	//先查询数据库中是否存在这个账号
	sprintf(sql, "select * from account_list where id=\"%s\" and pwd = \"%s\" ;", msg->id,msg->pwd);
	if(sqlite3_get_table(pstaff_db,sql,&presult,&row,&column,&errmsg)!=0){
		//!=0就是函数调用失败，返回错误码
		printf("sqlite3_get_table: %s 	__%d__\n", errmsg, __LINE__);
		return -1;
	}
	if(1==row){
		//找到的情况，先判断是否已经登录,如果已经登录，提示不能重复登录
		if(strcmp(presult[(row+1)*column-1],"0")==0){
			bzero(msg->notice,sizeof(msg->notice));
			strcat(msg->notice,"*logined already,不能重复登录*");
			if(send(newfd,msg,sizeof(*msg),0)<0){
				ERR_MSG("send");
				return -1;
			}
            printf("*logined already,不能重复登录*\n");
         }
		else{
			//给客户端发送提示信息login success，并修改登录状态
			bzero(msg->notice,sizeof(msg->notice));
			strcat(msg->notice,"*login success*");
			if(send(newfd,msg,sizeof(*msg),0)<0){
				ERR_MSG("send");
				return -1;
			}
            bzero(sql,sizeof(sql));
			sprintf(sql, "update account_list set stage = '0' where id=\"%s\" ;", msg->id);
			
			if(sqlite3_exec(pstaff_db, sql, NULL, NULL, &errmsg) != 0){
				printf("sqlite3_exec:%s %d\n", errmsg, __LINE__);
				return -1;
			}
            printf("登录成功__%d__notice=%s\n",  __LINE__,msg->notice);
		}
	}
    else{
		//没有找到，给客户端发送提示信息，id 不存在
		bzero(msg->notice,sizeof(msg->notice));
		strcpy(msg->notice, "NOTEXIST 不存在，请重新输入**");
		if(send(newfd,msg,sizeof(*msg),0)<0){
			ERR_MSG("send");
			return -1;
		}
    }
	return 0;
}

//查询信息
int show_self_info(sqlite3* pstaff_db,int newfd,struct user_msg *msg) 
{
	char sql[128]="";
	char **presult;
	int row = 0;
	int column = 0;
    char *errmsg=NULL;
	bzero(sql,sizeof(sql));
	sprintf(sql, "select * from info_list where id=\"%s\" ;", msg->id);
	if(sqlite3_get_table(pstaff_db, sql, &presult, &row, &column,  &errmsg) != 0)
	{
		//查找失败
		printf("sqlite3_exec:%s %d\n", errmsg, __LINE__);
		return -1;
	}
	printf("id = %s __%d__row=%d\n",msg->id, __LINE__,row);
	//如果没有找到ID相应的信息,发送not FOUND
	//如果找到了发送单词相应的意思
	if(0 == row)
	{
		strcpy(msg->notice, "账号id不存在");
		if(send(newfd,msg->notice,sizeof(msg->notice),0)<0){
			ERR_MSG("send");
			return -1;
		}
//        printf("__%d__row:%d column:%d presult[column]:%s notice:%s\n",__LINE__,row,column,presult[column],msg->notice);
	}
	else if(1==row){
		//发送info_list中的一行id,name名字 phone手机号 地址addr 薪资salary 年龄age
		//info_list(id char primary key,name char,phone char,addr char,salary float,age int)
		
//        printf("__%d__row:%d column:%d presult[5]:%s presult[6]%s presult[7]%s presult[8]%s presult[9]%s\n",__LINE__,
//        row,column,presult[5],presult[6],presult[7],presult[8],presult[9]);
		bzero(msg,sizeof(*msg));
		strcpy(msg->notice, "信息如下：");
        strcpy(msg->id, presult[column]);
        strcpy(msg->name, presult[column+1]);
        strcpy(msg->phone, presult[column+2]);
        strcpy(msg->addr, presult[column+3]);
        memcpy(&(msg->salary), presult[column+4],sizeof(float));
        memcpy(&(msg->age), presult[column+5],sizeof(int));
	}	
    if(send(newfd,msg,sizeof(*msg),0)<0){
			ERR_MSG("send");
			return -1;
	}
    printf("__%d__row:%dcolumn:%d员工信息发送成功notice:%s\nid:%s name:%s",__LINE__,row,column,msg->notice,msg->id,msg->name);
	return 0;
}

//退出系统
int sys_quit(sqlite3* pstaff_db,int newfd,struct user_msg *msg)
{
	char sql[120]="";
    char *errmsg=NULL;
	//修改stage为1-未登录，给客户端发送提示信息
	bzero(sql,sizeof(sql));
	sprintf(sql, "update account_list set stage = 1 where id=\"%s\" ;", msg->id);
	if(sqlite3_exec(pstaff_db, sql, NULL, NULL, &errmsg) != 0){
		printf("sqlite3_exec:%s %d\n", errmsg, __LINE__);
		return -1;
	}
	bzero(msg->notice,sizeof(msg->notice));
	strcat(msg->notice,"*quit success*");
	if(send(newfd,msg,sizeof(*msg),0)<0){
		ERR_MSG("send");
		return -1;
	}
    printf("id:%s__%d__notice:%s\n",msg->id, __LINE__,msg->notice);
	return 0;
}









