#include <zmq.h>
//#include <zmq_utils.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <sys/ioctl.h>

#include "basetypes.h"
#include "debuglog.h"
using namespace std;

#define MAX_PATH	260

struct openargs{
	CHAR lpFileName[MAX_PATH];
	CHAR lpFileName2[MAX_PATH];
	DWORD dwDesiredAccess;
};

struct closeargs{
	int hFile;
	int hFile2;
};

//lock and unlock
struct lockargs{
	int hFile;
	int hFile2;
	DWORD dwType;//读写锁标记、解锁//type
	DWORD dwFileStart;//start
	DWORD dwFileLen;//len
};

struct openres{
	int hFile;
	int hFile2;
};

struct closeres{
	int  res;
	int  res2;
};
//lock and unlock
struct lockres{
	int  res;
	int  res2;
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
	unsigned int        seqid;
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
int g_hPipe = 0;
void* g_context;

extern int  gi1,gi2,gk1,gk2;
extern char * strDesired[7];
extern char*strShare[8];

struct proc_remote{
	char message[M_SIZE];
	char ip_port[M_SIZE];
	void* g_socket;
	int  op_type;
	struct openargs g_opena;
	struct openres g_openr;
	struct closeargs g_closea;
	struct closeres g_closer;
	struct lockargs g_locka;
	struct lockres g_lockr;
	struct lockargs g_unlocka;
	struct lockres g_unlockr;
	enfs_fcb_delegation g_DelegationInfor;
	
	int inline openfile(openargs *t)
	{
		encodeOpen(t);
		send();
		recv();
		process();
		return 0;
	}

	int inline openfilex(openargs *t,struct openres *buff){
		encodeOpen(t);
		//Log3(APP_LOG_ERROR,"%s,encodeOpen=%s",ip_port, message);
		send();
		recv();
		process();
		buff->hFile=g_openr.hFile;
		buff->hFile2=g_openr.hFile2;
		//Log3(APP_LOG_ERROR,"open handle=%p,%p",g_openr.hFile, g_openr.hFile2);
		return 0;
	}

	int inline closefilex(struct openres *buff)
	{
		printf("start test delegation closefilex \n");
		g_closea.hFile = buff->hFile;
		g_closea.hFile2 = buff->hFile2;
		encodeClose(&g_closea);
		//Log2(APP_LOG_ERROR,"encodeClose=%s",message);
		send();
		recv();
		process();
		//Log3(APP_LOG_ERROR,"close res=%d,%d",g_closer.res, g_closer.res2);
		return 0;
	}

	int inline checkopen2()
	{
		printf("start test delegation checkopen2 \n");
		if (g_openr.hFile < 0) 
			return 0;

		return 1;
	}

	int inline closefile()
	{
		g_closea.hFile = g_openr.hFile;
		g_closea.hFile2 = g_openr.hFile2;
		encodeClose(&g_closea);
		send();
		recv();
		process();
		return 0;
	}
	
	int inline lock(struct lockargs *t)
	{
		encodeLock(t);
		send();
		recv();
		process();
		return 0;
	}
	
	int inline unlock(struct lockargs *t)
	{
		encodeUnLock(t);
		send();
		recv();
		process();

		return 0;
	}

	int inline encodeOpen(struct openargs *t)
	{
		memset(message,0,M_SIZE);
		sprintf(message,"%u %s %s %lu", 1,
			t->lpFileName, 
			t->lpFileName2, 
			t->dwDesiredAccess);
			
		return 0;
	}

	int inline encodeOpenR(struct openres *t)
	{
		memset(message,0,M_SIZE);
		sprintf(message,"%u %d %d", 2, 
			t->hFile,
			t->hFile2);
		
		return 0;
	}

	int inline encodeLock(struct lockargs *t)
	{
		memset(message,0,M_SIZE);
		sprintf(message,"%u %d %d %lu %lu %lu", 3,
			t->hFile, 
			t->hFile2, 
			t->dwType, 
			t->dwFileStart,
			t->dwFileLen);
		
		return 0;
	}

	int inline encodeLockR(struct lockres *t)
	{
		memset(message,0,M_SIZE);
		sprintf(message,"%u %d %d", 4, 
			t->res,
			t->res2);
			
		return 0;
	}

	int inline encodeUnLock(struct lockargs *t)
	{
		memset(message,0,M_SIZE);
		sprintf(message,"%u %d %d %lu %lu %lu", 5,
			t->hFile, 
			t->hFile2, 
			t->dwType, 
			t->dwFileStart,
			t->dwFileLen);
			
		return 0;
	}

	int inline encodeUnLockR(struct lockres *t)
	{
		memset(message,0,M_SIZE);
		sprintf(message,"%u %d %d", 6, 
			t->res,
			t->res2);
			
		return 0;
	}

	int inline encodeClose(struct closeargs *t)
	{
		memset(message,0,M_SIZE);
		sprintf(message,"%u %d %d", 7, 
			t->hFile,
			t->hFile2);
			
		return 0;
	}

	int inline encodeCloseR(struct closeres *t)
	{
		memset(message,0,M_SIZE);
		sprintf(message,"%u %d %d", 8, 
			t->res,
			t->res2);
			
		return 0;
	}

	void decodeOpen(struct openargs *t)
	{
		sscanf(message, "%u%s%s%u", &op_type,
			&(t->lpFileName),
			&(t->lpFileName2),
			&( t->dwDesiredAccess));
		memset(message,0, M_SIZE);
	}

	void decodeOpenR(struct openres *t)
	{
		sscanf(message, "%u%d%d", &op_type,
			&(t->hFile),
			&(t->hFile2)
			);
		
		memset(message,0, M_SIZE);
	}

	void decodeLock(struct lockargs *t)
	{
		sscanf(message, "%u%d%d%lu%lu%lu", &op_type,
			&(t->hFile),
			&(t->hFile2),
			&(t->dwType),
			&(t->dwFileStart),	
			&(t->dwFileLen));
		memset(message,0, M_SIZE);
	}

	void decodeLockR(struct lockres *t)
	{
		sscanf(message, "%u%d%d", &op_type,
			&(t->res),
			&(t->res2));
		memset(message,0, M_SIZE);
	}
	void decodeUnLock(struct lockargs *t)
	{
		sscanf(message, "%u%d%d%lu%lu%lu", &op_type,
			&(t->hFile),
			&(t->hFile2),
			&(t->dwType),
			&(t->dwFileStart),	
			&(t->dwFileLen));
		memset(message,0, M_SIZE);
	}
	void decodeUnLockR(struct lockres *t)
	{
		sscanf(message, "%u%d%d", &op_type,
			&(t->res),
			&(t->res2));
		memset(message,0, M_SIZE);
	}

	void decodeClose(struct closeargs *t)
	{
		sscanf(message, "%u%d%d", &op_type,
			&(t->hFile),
			&(t->hFile2));
		memset(message,0, M_SIZE);
	}


	void decodeCloseR(struct closeres *t)
	{
		sscanf(message, "%u%d%d", &op_type,
			&(t->res),
			&(t->res2));
		memset(message,0, M_SIZE);
	}

	void decodeType()
	{
		sscanf(message, "%u", &op_type);
	}
	
	int inline enfs_filelock(int fd, struct lockargs liov)
	{
		struct flock lock;
		int ret = 0;

		lock.l_type = liov.dwType;
		lock.l_start = liov.dwFileStart;
		lock.l_whence = SEEK_SET;//SEEK_SET,SEEK_CUR,SEEK_END
		lock.l_len = liov.dwFileLen;
		lock.l_pid = getpid();

		ret = fcntl(fd, F_SETLK, &lock);
		if (ret < 0) {
			Log2(APP_LOG_DEBUG, "file set lock false,error=%s",strerror(errno));
		}

		return ret;
	}

	int inline enfs_fileunLock(int fd, struct lockargs liov)
	{
		struct flock lock;
		int ret = 0;
		
//		lock.l_type = liov.dwType;
		lock.l_type = F_UNLCK;
		lock.l_start = liov.dwFileStart;
		lock.l_whence = SEEK_SET;//SEEK_SET,SEEK_CUR,SEEK_END
		lock.l_len = liov.dwFileLen;
		lock.l_pid = getpid();

		ret = fcntl(fd, F_SETLKW, &lock);
		if (ret < 0) {
			Log2(APP_LOG_DEBUG, "file set unlock false,error=%s",strerror(errno));
		}

		return ret;
	}
#if 0
	void MyOutPutDebugStr(char* str,int par1=0,int par2=0,int par3=0,int par4=0)	
	{		
		char buff[512];
		memset(buff,0,512);
		sprintf(buff,"%s,%d,%d,%d,%d",str,par1,par2,par3,par4);
		Log2(APP_LOG_ERROR,"%s",buff);
	}
#endif
	int inline decodeDelegationRes(enfs_fcb_delegation *t)
	{
		memcpy(t,message+strlen(message)+1,sizeof(enfs_fcb_delegation));
		memset(message,0,M_SIZE);
		
		return 0;
	}
	
	int inline encodeDelegationRes(enfs_fcb_delegation *t)
	{
		memset(message,0,M_SIZE);
		sprintf(message,"%u ",12);
		memcpy(message+strlen(message)+1,t,sizeof(enfs_fcb_delegation));
		
		return 0;
	}

	int inline decodeDelegationCmd(int *t)
	{
		int hf = 0;
		sscanf(message, "%d %d", &op_type,&hf);
		memset(message,0, M_SIZE);
		*t=hf;
		
		return 0;
	}
	
	int inline encodDelegationCmd(GET_DELEGATION_CMD *t)
	{
		memset(message,0,M_SIZE);
		sprintf(message,"%d %d", 11, 
			t->hFile);
		
		return 0;
	}

	int inline sendDelegationCmd(GET_DELEGATION_CMD t)
	{
		encodDelegationCmd(&t);
		send();
		recv();
		process();

		return 0;
	}

	int inline ioctrl_get_deleg(int fd, enfs_fcb_delegation *deleg)
	{
		int ret = 0, ret2 = 0;
		int cdev_fd = 0;
		unsigned long long bytesret = 1;
		struct ioctl_struct bw_arg;
		
		memset(&bw_arg,0,sizeof(bw_arg));
		cdev_fd = open("/dev/vdmap_dev", O_RDONLY);
		if (fd <= 0) {
			ret = -ret;
			Log2(APP_LOG_ERROR, "Open cdev failed(%d).", cdev_fd);
		}

		memset(&bw_arg, 0, sizeof(struct ioctl_struct));
		bw_arg.inbuffer  = ((unsigned long long)(&fd));
		bw_arg.in_size   = sizeof(unsigned int);
		bw_arg.outbuffer = (unsigned long long)deleg;
		bw_arg.out_size  = sizeof(enfs_fcb_delegation);
		bw_arg.bytesreturned = (unsigned long long)(&bytesret);

		ret = ioctl(cdev_fd,IOCTL_CODE_GET_DELEGATION_INFO,&bw_arg);
		if (ret < 0) {
			ret = -ret;
			Log2(APP_LOG_ERROR, "Call ioctl failed(%d).", ret);
			return ret;
		}
#if 0
		printf("ioctrl_get_deleg test ioctl success,ret=%d, sizeof(enfs_fcb_delegation)=%lu, access=%u, status=%lu, seqid=%u, other[%llu-%llu-%llu]\n",
 				ret, sizeof(enfs_fcb_delegation), deleg->access, 
				deleg->state_id.status, deleg->state_id.seqid, 
				*(UINT64*)&deleg->state_id.other[0], 
				*(UINT64*)&deleg->state_id.other[8], 
				*(UINT64*)&deleg->state_id.other[16]);
#endif
		
		if (cdev_fd > 0) {
			ret2 = close(cdev_fd);
			if (ret2 < 0) {
				ret = -ret2;
				Log2(APP_LOG_ERROR, "Close cdev failed(%d).", errno);
			}
		}
		
		return ret;
	}

	int inline doDelegationCmd()
	{
		GET_DELEGATION_CMD t;
		decodeDelegationCmd(&t.hFile);
		int ret = 1;

		if(t.hFile < 0)
			return t.hFile;

		memset(&g_DelegationInfor,0,sizeof(g_DelegationInfor));
		ret = ioctrl_get_deleg(t.hFile,&g_DelegationInfor);
 		if (ret < 0) {
			Log3(APP_LOG_ERROR, "ioctrl_read_error=%d,hFile=%d\n", errno, t.hFile);
			return ret;
		}
  		else {
			printf("sizeof(enfs_fcb_delegation)=%lu, access=%u, status=%lu, seqid=%u, other[%llu-%llu-%llu]\n",
				sizeof(enfs_fcb_delegation), 
				g_DelegationInfor.access, 
				g_DelegationInfor.state_id.status, 
				g_DelegationInfor.state_id.seqid, 
				*(UINT64*)&g_DelegationInfor.state_id.other[0], 
				*(UINT64*)&g_DelegationInfor.state_id.other[8], 
				*(UINT64*)&g_DelegationInfor.state_id.other[16]);
		}
		
		encodeDelegationRes(&g_DelegationInfor);
		return 0;
	}

	int inline process()
	{
		decodeType();
		switch (op_type){
		case 1:
			decodeOpen(&g_opena);
			g_openr.hFile = open(g_opena.lpFileName, g_opena.dwDesiredAccess,0755);
			if (g_openr.hFile < 0) {
				Log4(APP_LOG_DEBUG, "open enfs2 file errorCode fileName==%s,%x,errno=%s",
					g_opena.lpFileName,g_opena.dwDesiredAccess,strerror(errno));
			}
			g_openr.hFile2 = open(g_opena.lpFileName2, g_opena.dwDesiredAccess,0755);
			if (g_openr.hFile2 < 0){
				Log4(APP_LOG_DEBUG, "open nfs file errorCode fileName2==%s,%x,errno=%s",
					g_opena.lpFileName2,g_opena.dwDesiredAccess,strerror(errno));
			}
			encodeOpenR(&g_openr);
			break;
				
		case 2:
			decodeOpenR(&g_openr);
			break;

		case 3://锁操作，加锁。
			decodeLock(&g_locka);
			g_lockr.res = enfs_filelock(g_locka.hFile, g_locka);
			if (g_lockr.res < 0){
				Log6(APP_LOG_DEBUG, "lockFile enfs2 errorCode==%d,fileHandle==%x,[%llu,%llu],errno=%s",
					g_lockr.res,g_locka.hFile,g_locka.dwFileStart,g_locka.dwFileStart+g_locka.dwFileLen,strerror(errno));
			}
			g_lockr.res2 = enfs_filelock(g_locka.hFile2, g_locka);
			if (g_lockr.res < 0){
				Log6(APP_LOG_DEBUG, "lockFile nfs errorCode==%d,fileHandle==%x,[%llu,%llu],errno=%s",
					g_lockr.res,g_locka.hFile2,g_locka.dwFileStart,g_locka.dwFileStart+g_locka.dwFileLen,strerror(errno));
			}
			encodeLockR(&g_lockr);
			break;
				
		case 4:
			decodeLockR(&g_lockr);
			break;
				
		case 5:
			decodeUnLock(&g_unlocka);
			g_unlockr.res = enfs_fileunLock(g_unlocka.hFile, g_unlocka);
			if (g_unlockr.res < 0){
				Log6(APP_LOG_DEBUG, "UnlockFile enfs2 errorCode==%d,fileHandle==%x,[%lld,%lld],errno=%s",
					g_unlockr.res,g_unlocka.hFile,g_unlocka.dwFileStart,g_unlocka.dwFileStart+g_unlocka.dwFileLen,strerror(errno));
			}
			g_unlockr.res2 = enfs_fileunLock(g_unlocka.hFile2, g_unlocka);
			if (g_unlockr.res2 < 0){
				Log6(APP_LOG_DEBUG, "UnlockFile nfs errorCode==%d,fileHandle==%x,[%lld,%lld],errno=%s",
					g_unlockr.res,g_unlocka.hFile2,g_unlocka.dwFileStart,g_unlocka.dwFileStart+g_unlocka.dwFileLen,strerror(errno));
			}
			encodeUnLockR(&g_unlockr);
			break;
				
		case 6:
			decodeUnLockR(&g_unlockr);
			break;
				
		case 7:
			decodeClose(&g_closea);
			g_closer.res = close(g_closea.hFile);
			g_closer.res2 = close(g_closea.hFile2);
			encodeCloseR(&g_closer);
			break;
				
		case 8:
			decodeCloseR(&g_closer);
			break;
			
		case 9://do file io test for  byte range
			break;
		case 10:
			break;
		case 11://server process delegaton cmd
			memset(&g_DelegationInfor,0,sizeof(g_DelegationInfor));
			doDelegationCmd();
			break;
				
		case 12://client recv delegation infor
			decodeDelegationRes(&g_DelegationInfor);
			break;
		default:
			break;
		}
		return 0;
	}

	int inline listen(int con_type=0, int port=0)
	{
		if (con_type==0){
			g_socket = zmq_socket(g_context,ZMQ_REP);//ZMQ_REQ ZMQ_REP  //ZMQ_PUB ZMQ_SUB
			sprintf(ip_port,"tcp://*:%d",port);// accept connections on a socket//"tcp://*:5555"
			int res = zmq_bind(g_socket, ip_port);
			return res;
		}

		return 0;
	}

	int inline connect(int  con_type=0, const char *addr=0)
	{
		printf("connect\n");
		if (con_type==0){
			printf("connect         0000\n");
			g_socket = zmq_socket(g_context,ZMQ_REQ);
			sprintf(ip_port,"tcp://%s",addr);
			zmq_connect(g_socket, ip_port);
		}

		return 0;
	}

	int inline recv(int con_type=0)
	{
		if (con_type==0){
			zmq_msg_t recv_msg;
			zmq_msg_init(&recv_msg);
			zmq_msg_recv(&recv_msg,g_socket,0); //阻塞
//			ZMQ_DONTWAIT;//ZMQ_DONTWAIT表示非阻塞
			memcpy(message,(char*)zmq_msg_data(&recv_msg), M_SIZE);
			zmq_msg_close(&recv_msg);
		}

		return 0;
	}

	int inline send(int con_type=0)
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

	int inline zclose(int con_type=0)
	{
		if (con_type==0){
			zmq_close(g_socket);
		}
		return 0;
	}
	
	int inline checkopen()
	{
		int ret = 0;
		
		if((g_openr.hFile >= 0 && g_openr.hFile2 >= 0) || (g_openr.hFile < 0 && g_openr.hFile2 < 0)) {
			Log5(APP_LOG_ERROR, "%s\t%-10d\t%-10d\n",g_opena.dwDesiredAccess,g_opena.dwDesiredAccess,1,0);
			ret = 0;
		}
		else {
			Log5(APP_LOG_ERROR, "%s\t%-20s\t%-10d\t%-10d\n",g_opena.dwDesiredAccess,g_opena.dwDesiredAccess,0,1);
				ret = 1;
		}
		
		if(ret) {
			Log3(APP_LOG_ERROR, "open not matchs! %p,%p",g_openr.hFile,g_openr.hFile2);
		}
		else {
			Log3(APP_LOG_ERROR, "open matchs, %p,%p",g_openr.hFile,g_openr.hFile2);
		}
		
		return ret;
	}

	int inline checklock()
	{
		int ret = 0;
		
		if(g_lockr.res >= 0 && g_lockr.res2 >= 0) {
			ret = 0;
		}
		else if(g_lockr.res < 0 && g_lockr.res2 < 0){
			ret = 1;
		}
		else{
			ret=2;
		}
			
		if (ret==0 || ret==1){
			Log3(APP_LOG_ERROR, "lock match = %d,%d",g_lockr.res,g_lockr.res2);
		}
		else{
			Log3(APP_LOG_ERROR, "lock not match = %d,%d",g_lockr.res,g_lockr.res2);
		}
		return ret;
	}

	int inline checkunlock()
	{
		int ret = 0;
		if(g_unlockr.res >= 0 && g_unlockr.res2 >= 0) {
			ret = 0;
		}
		else if(g_unlockr.res < 0 && g_unlockr.res2 < 0){
			ret = 1;
		}
		else{
			ret=2;
		}
			
		if (ret==0 || ret==1){
			Log3(APP_LOG_ERROR, "unlock match = %d,%d",g_unlockr.res,g_unlockr.res2);
		}
		else{
			Log3(APP_LOG_ERROR, "unlock not match = %d,%d",g_unlockr.res,g_unlockr.res2);
		}
		return ret;
	}
};

