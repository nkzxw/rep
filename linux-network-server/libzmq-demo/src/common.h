#ifndef COMMON_H_
#define COMMON_H_
#include <zmq.h>
//#include <zmq_utils.h>
//#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
//#include <time.h>

//#include <sys/time.h>
//#include <netinet/in.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <error.h>
#if !defined _WIN32
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#endif

#include "basetypes.h"
#include "debuglog.h"
#define MAX_PATH	260

struct openargs{
    CHAR lpFileName[MAX_PATH];
    DWORD dwDesiredAccess;
    DWORD dwShareMode;
    DWORD dwCreationDisposition;
};

struct closeargs{
    int hFile;
};

//lock and unlock
struct lockargs{
    int hFile;
    DWORD dwType;//
    DWORD dwFileStart;//start
    DWORD dwFileLen;//len
};

struct openres{
    int hFile;
};

struct closeres{
    int res;
};
//lock and unlock
struct lockres{
    int res;
};

struct IO_RESULT 
{
    WORD opSucc;
    WORD opSucc2;
    WORD iotype;//
};

typedef struct _GET_DELEGATION_ARGS{
    INT hFile;
}GET_DELEGATION_CMD;

typedef struct enfs_fcb_state_id {
    unsigned long   status;
    unsigned int       seqid;
    char            other[20];
}enfs_fcb_state_id;

struct hlist_node {	
    struct hlist_node *next, **pprev;
};

typedef struct enfs_delegation {
    unsigned int		access;
    struct enfs_fcb_state_id	state_id;
    struct hlist_node ed_node;
}enfs_fcb_delegation;

enum{
    IOCTL_CODE_GET_DELEGATION_INFO=0xc020f04a,
};

struct ioctl_struct
{
    unsigned long long inbuffer;
    unsigned long long outbuffer;
    unsigned int in_size;
    unsigned int out_size;
    unsigned  long long bytesreturned;
};

#define M_SIZE 512
#define MAX_SERVER 10
//shell commond.
//1:check bwfs service. 
//2:stop bwfs service.
//3:block client ip network.
//4:stop block client ip network.

enum bwfs_cmd{
    SERVICE_CHECK=1,
    STOP_SERVICE,
    START_SERVICE,
    BLOCK_CLIENT,
    STOP_BLOCK_CLIENT
};
extern int gi1,gi2,gk1,gk2;
extern char * strDesired[7];
extern char*strShare[8];

class proc_remote{
    char message[M_SIZE];
    char ip_port[M_SIZE];
    void* g_socket;
    int op_type;
    public:
    struct openargs g_opena;
    struct openres g_openr;
    struct closeargs g_closea;
    struct closeres g_closer;
    struct lockargs g_locka;
    struct lockres g_lockr;
    struct lockargs g_unlocka;
    struct lockres g_unlockr;
    enfs_fcb_delegation g_DelegationInfor;
    bwfs_cmd g_shell_cmd; 
    int g_shell_res;
    public:
    int process();
    int doDelegationCmd();
    int doOpen();
    int openfile(openargs *t)
    {
	encodeOpen(t);
	send();
	recv();
	process();
	return 0;
    }

    int openfilex(openargs *t,struct openres *buff){
	encodeOpen(t);
	//bw_log(APP_LOG_ERROR,"%s,encodeOpen=%s",ip_port, message);
	send();
	recv();
	process();
	buff->hFile=g_openr.hFile;
	//bw_log(APP_LOG_ERROR,"open handle=%p,%p",g_openr.hFile, g_openr.hFile2);
	return 0;
    }

    int closefilex(struct openres *buff)
    {
	printf("start test delegation closefilex \n");
	g_closea.hFile = buff->hFile;
	encodeClose(&g_closea);
	//bw_log(APP_LOG_ERROR,"encodeClose=%s",message);
	send();
	recv();
	process();
	//bw_log(APP_LOG_ERROR,"close res=%d,%d",g_closer.res, g_closer.res2);
	return 0;
    }

    
    int closefile()
    {
	g_closea.hFile = g_openr.hFile;
	encodeClose(&g_closea);
	send();
	recv();
	process();
	return 0;
    }

    int lock(struct lockargs *t)
    {
	encodeLock(t);
	send();
	recv();
	process();
	return 0;
    }

    int unlock(struct lockargs *t)
    {
	encodeUnLock(t);
	send();
	recv();
	process();

	return 0;
    }
    int inline encodeOpen(struct openargs *t){
	memset(message,0,M_SIZE);
	sprintf(message,"%u %s %u %u %u", 1, 
		t->lpFileName, 
		t->dwDesiredAccess,
		t->dwShareMode, 
		t->dwCreationDisposition);
	return 0;
    }

    int encodeOpenR(struct openres *t)
    {
	memset(message,0,M_SIZE);
	sprintf(message,"%u %d", 2, 
		t->hFile);

	return 0;
    }

    int encodeLock(struct lockargs *t)
    {
	memset(message,0,M_SIZE);
	sprintf(message,"%u %d %lu %lu %lu", 3,
		t->hFile, 
		t->dwType, 
		t->dwFileStart,
		t->dwFileLen);

	return 0;
    }

    int encodeLockR(struct lockres *t)
    {
	memset(message,0,M_SIZE);
	sprintf(message,"%u %d", 4, 
		t->res);

	return 0;
    }

    int encodeUnLock(struct lockargs *t)
    {
	memset(message,0,M_SIZE);
	sprintf(message,"%u %d %lu %lu %lu", 5,
		t->hFile, 
		t->dwType, 
		t->dwFileStart,
		t->dwFileLen);

	return 0;
    }

    int encodeUnLockR(struct lockres *t)
    {
	memset(message,0,M_SIZE);
	sprintf(message,"%u %d", 6, 
		t->res);

	return 0;
    }

    int encodeClose(struct closeargs *t)
    {
	memset(message,0,M_SIZE);
	sprintf(message,"%u %d", 7, 
		t->hFile);

	return 0;
    }

    int encodeCloseR(struct closeres *t)
    {
	memset(message,0,M_SIZE);
	sprintf(message,"%u %d", 8, 
		t->res);

	return 0;
    }

    void decodeOpen(struct openargs *t){
	sscanf(message, "%u%s%u%u%u", &op_type,
		&(t->lpFileName),
		&( t->dwDesiredAccess), 
		&(t->dwShareMode), 
		&(t->dwCreationDisposition));
	memset(message,0, M_SIZE);
    }
    void decodeOpenR(struct openres *t)
    {
	sscanf(message, "%u%d", &op_type,
		&(t->hFile)
	      );

	memset(message,0, M_SIZE);
    }

    void decodeLock(struct lockargs *t)
    {
	sscanf(message, "%u%d%lu%lu%lu", &op_type,
		&(t->hFile),
		&(t->dwType),
		&(t->dwFileStart),	
		&(t->dwFileLen));
	memset(message,0, M_SIZE);
    }

    void decodeLockR(struct lockres *t)
    {
	sscanf(message, "%u%d", &op_type,
		&(t->res));
	memset(message,0, M_SIZE);
    }
    void decodeUnLock(struct lockargs *t)
    {
	sscanf(message, "%u%d%lu%lu%lu", &op_type,
		&(t->hFile),
		&(t->dwType),
		&(t->dwFileStart),	
		&(t->dwFileLen));
	memset(message,0, M_SIZE);
    }
    void decodeUnLockR(struct lockres *t)
    {
	sscanf(message, "%u%d", &op_type,
		&(t->res));
	memset(message,0, M_SIZE);
    }

    void decodeClose(struct closeargs *t)
    {
	sscanf(message, "%u%d", &op_type,
		&(t->hFile));
	memset(message,0, M_SIZE);
    }


    void decodeCloseR(struct closeres *t)
    {
	sscanf(message, "%u%d", &op_type,
		&(t->res));
	memset(message,0, M_SIZE);
    }

    void decodeType()
    {
	sscanf(message, "%u", &op_type);
    }
#if 0
    void MyOutPutDebugStr(char* str,int par1=0,int par2=0,int par3=0,int par4=0)	
    {		
	char buff[512];
	memset(buff,0,512);
	sprintf(buff,"%s,%d,%d,%d,%d",str,par1,par2,par3,par4);
	bw_log(APP_LOG_ERROR,"%s",buff);
    }
#endif
    int decodeDelegationRes(enfs_fcb_delegation *t)
    {
	memcpy(t,message+strlen(message)+1,sizeof(enfs_fcb_delegation));
	memset(message,0,M_SIZE);

	return 0;
    }

    int encodeDelegationRes(enfs_fcb_delegation *t)
    {
	memset(message,0,M_SIZE);
	sprintf(message,"%u ",12);
	memcpy(message+strlen(message)+1,t,sizeof(enfs_fcb_delegation));

	return 0;
    }

    int decodeDelegationCmd(int *t)
    {
	int hf = 0;
	sscanf(message, "%d %d", &op_type,&hf);
	memset(message,0, M_SIZE);
	*t=hf;

	return 0;
    }

    int encodDelegationCmd(GET_DELEGATION_CMD *t)
    {
	memset(message,0,M_SIZE);
	sprintf(message,"%d %d", 11, 
		t->hFile);

	return 0;
    }

    int sendDelegationCmd(GET_DELEGATION_CMD t)
    {
	encodDelegationCmd(&t);
	send();
	recv();
	process();

	return 0;
    }

    int encodeShell(int g_shell_cmd)
    {
	memset(message,0,M_SIZE);
	sprintf(message,"%u %d",13, g_shell_cmd);
	return 0;	
    }
    int encodeShellRes(int g_shell_res)
    {
	memset(message,0,M_SIZE);
	sprintf(message,"%u %d",14, g_shell_res);
	return 0;	
    }
    int decodeShell(bwfs_cmd *t)
    {
	int hf = 0;
	sscanf(message, "%d %d", &op_type,&hf);
	memset(message,0, M_SIZE);
	*t = (bwfs_cmd)hf;
	return 0;
    }
    int decodeShellRes(int *res){
	int hf = 0;
	sscanf(message, "%d %d", &op_type,&hf);
	memset(message,0, M_SIZE);
	*res=hf;
	return 0;
    }
    int listen(int con_type, int port, void* context)
    {
	if (con_type==0){
	    g_socket = zmq_socket(context,ZMQ_REP);//ZMQ_REQ ZMQ_REP  //ZMQ_PUB ZMQ_SUB
	    sprintf(ip_port,"tcp://*:%d",port);// accept connections on a socket//"tcp://*:5555"
	    int res = zmq_bind(g_socket, ip_port);
	    return res;
	}

	return 0;
    }

    int connect(int con_type, const char *addr, void* context)
    {
	if (con_type==0){
	    g_socket = zmq_socket(context,ZMQ_REQ);
	    sprintf(ip_port,"tcp://%s",addr);
	    int ret = zmq_connect(g_socket, ip_port);
	    printf("connect: %s = %d\n",ip_port,ret);
	}

	return 0;
    }

    int recv(int con_type=0)
    {
	if (con_type==0){
	    zmq_msg_t recv_msg;
	    zmq_msg_init(&recv_msg);
	    zmq_msg_recv(&recv_msg,g_socket,0);
	    memcpy(message,(char*)zmq_msg_data(&recv_msg), M_SIZE);
	    zmq_msg_close(&recv_msg);
	}

	return 0;
    }

    int send(int con_type=0)
    {
	if (con_type==0){
	    zmq_msg_t send_msg;
	    zmq_msg_init_size(&send_msg,M_SIZE);
	    memcpy(zmq_msg_data(&send_msg), message, M_SIZE);
	    zmq_sendmsg(g_socket,&send_msg,0);
	    zmq_msg_close(&send_msg);
	}

	return 0;
    }

    int zclose(int con_type=0)
    {
	if (con_type==0){
	    zmq_close(g_socket);
	}
	return 0;
    }

    int checkopen(int hFile)
    {
	int ret = 0;

	if((g_openr.hFile >= 0 && hFile >= 0) || (g_openr.hFile < 0 && hFile < 0)) {
	    bw_log(APP_LOG_ERROR, "%s\t%-10d\t%-10d\n",g_opena.dwDesiredAccess,g_opena.dwDesiredAccess,1,0);
	    ret = 0;
	}
	else {
	    bw_log(APP_LOG_ERROR, "%s\t%-20s\t%-10d\t%-10d\n",g_opena.dwDesiredAccess,g_opena.dwDesiredAccess,0,1);
	    ret = 1;
	}

	if(ret) {
	    bw_log(APP_LOG_ERROR, "open not matchs! %p,%p",g_openr.hFile,hFile);
	}
	else {
	    bw_log(APP_LOG_ERROR, "open matchs, %p,%p",g_openr.hFile,hFile);
	}

	return ret;
    }

    int checkopen2()
    {
	if (g_openr.hFile < 0) {
	    bw_log(APP_LOG_ERROR,"open file error %d", g_openr.hFile );
	    return 0;
	}

	return 1;
    }

    int checklock(int res)
    {
	int ret = 0;

	if(g_lockr.res >= 0 && res >= 0) {
	    ret = 0;
	}
	else if(g_lockr.res < 0 && res < 0){
	    ret = 1;
	}
	else{
	    ret=2;
	}

	if (ret==0 || ret==1){
	    bw_log(APP_LOG_ERROR, "lock match = %d,%d",g_lockr.res,res);
	}
	else{
	    bw_log(APP_LOG_ERROR, "lock not match = %d,%d",g_lockr.res,res);
	}
	return ret;
    }

    int checkunlock(int res)
    {
	int ret = 0;
	if(g_unlockr.res >= 0 && res >= 0) {
	    ret = 0;
	}
	else if(g_unlockr.res < 0 && res < 0){
	    ret = 1;
	}
	else{
	    ret=2;
	}

	if (ret==0 || ret==1){
	    bw_log(APP_LOG_ERROR, "unlock match = %d,%d",g_unlockr.res,res);
	}
	else{
	    bw_log(APP_LOG_ERROR, "unlock not match = %d,%d",g_unlockr.res,res);
	}
	return ret;
    }

    int DoShell(bwfs_cmd cmd){

	encodeShell(cmd);
	send();
	recv();
	process();

	return g_shell_res;
    } 
};
#endif
