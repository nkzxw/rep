#include "common.h"
int doShellCmd(bwfs_cmd cmd);


int ioctrl_get_deleg(int fd, enfs_fcb_delegation *deleg);
int enfs_filelock(int fd, struct lockargs liov);
int enfs_fileunLock(int fd, struct lockargs liov);
int StopBlockClient();
int StartBlockClient();
int GetEnfsdStatus();
int StopBwfsService();
int StartBwfsService();
int proc_remote::process()
{
    decodeType();
    switch (op_type){
	case 1:
	    decodeOpen(&g_opena);
	    doOpen();
	    encodeOpenR(&g_openr);
	    break;

	case 2:
	    decodeOpenR(&g_openr);
	    break;

	case 3:
	    decodeLock(&g_locka);
	    g_lockr.res = enfs_filelock(g_locka.hFile, g_locka);
	    if (g_lockr.res < 0){
		bw_log(APP_LOG_DEBUG, "lockFile enfs2 errorCode==%d,fileHandle==%x,[%llu,%llu],errno=%s",
			g_lockr.res,g_locka.hFile,g_locka.dwFileStart,g_locka.dwFileStart+g_locka.dwFileLen,strerror(errno));
	    }
	   /* g_lockr.res2 = enfs_filelock(g_locka.hFile2, g_locka);
	    if (g_lockr.res < 0){
		bw_log(APP_LOG_DEBUG, "lockFile nfs errorCode==%d,fileHandle==%x,[%llu,%llu],errno=%s",
			g_lockr.res,g_locka.hFile2,g_locka.dwFileStart,g_locka.dwFileStart+g_locka.dwFileLen,strerror(errno));
	    }
	    */
	    encodeLockR(&g_lockr);
	    break;

	case 4:
	    decodeLockR(&g_lockr);
	    break;

	case 5:
	    decodeUnLock(&g_unlocka);
	    g_unlockr.res = enfs_fileunLock(g_unlocka.hFile, g_unlocka);
	    if (g_unlockr.res < 0){
		bw_log(APP_LOG_DEBUG, "UnlockFile enfs2 errorCode==%d,fileHandle==%x,[%lld,%lld],errno=%s",
			g_unlockr.res,g_unlocka.hFile,g_unlocka.dwFileStart,g_unlocka.dwFileStart+g_unlocka.dwFileLen,strerror(errno));
	    }
	   /* g_unlockr.res2 = enfs_fileunLock(g_unlocka.hFile2, g_unlocka);
	    if (g_unlockr.res2 < 0){
		bw_log(APP_LOG_DEBUG, "UnlockFile nfs errorCode==%d,fileHandle==%x,[%lld,%lld],errno=%s",
			g_unlockr.res,g_unlocka.hFile2,g_unlocka.dwFileStart,g_unlocka.dwFileStart+g_unlocka.dwFileLen,strerror(errno));
	    }
	    */
	    encodeUnLockR(&g_unlockr);
	    break;

	case 6:
	    decodeUnLockR(&g_unlockr);
	    break;

	case 7:
	    decodeClose(&g_closea);
	    g_closer.res = close(g_closea.hFile);
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
	case 13:
	    decodeShell(&g_shell_cmd);
	    g_shell_res = ::doShellCmd(g_shell_cmd);
	    encodeShellRes(g_shell_res);
	    break;

	case 14://client recv delegation infor
	    decodeShellRes(&g_shell_res);
	    break;

	default:
	    break;
    }
    return 0;
}


int proc_remote::doDelegationCmd()
{
    GET_DELEGATION_CMD t;
    decodeDelegationCmd(&t.hFile);
    int ret = 1;

    if(t.hFile < 0)
	return t.hFile;

    memset(&g_DelegationInfor,0,sizeof(g_DelegationInfor));
    ret = ioctrl_get_deleg(t.hFile,&g_DelegationInfor);
    if (ret < 0) {
	bw_log(APP_LOG_ERROR, "ioctrl_read_error=%d,hFile=%d\n", errno, t.hFile);
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

#if !defined _WIN32
int ioctrl_get_deleg(int fd, enfs_fcb_delegation *deleg)
{
    int ret = 0, ret2 = 0;
    int cdev_fd = 0;
    unsigned long long bytesret = 1;
    struct ioctl_struct bw_arg;

    memset(&bw_arg,0,sizeof(bw_arg));
    cdev_fd = open("/dev/vdmap_dev", O_RDONLY);
    if (fd <= 0) {
	ret = -ret;
	bw_log(APP_LOG_ERROR, "Open cdev failed(%d).", cdev_fd);
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
	bw_log(APP_LOG_ERROR, "Call ioctl failed(%d).", ret);
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
	    bw_log(APP_LOG_ERROR, "Close cdev failed(%d).", errno);
	}
    }

    return ret;
}
int proc_remote::doOpen(){
    g_openr.hFile = open(g_opena.lpFileName, g_opena.dwDesiredAccess,0755);
    if (g_openr.hFile < 0) {
	bw_log(APP_LOG_DEBUG, "open enfs2 file errorCode fileName==%s,%x,errno=%s",
		g_opena.lpFileName,g_opena.dwDesiredAccess,strerror(errno));
    }
    /*g_openr.hFile2 = open(g_opena.lpFileName2, g_opena.dwDesiredAccess,0755);
    if (g_openr.hFile2 < 0){
	bw_log(APP_LOG_DEBUG, "open nfs file errorCode fileName2==%s,%x,errno=%s",
		g_opena.lpFileName2,g_opena.dwDesiredAccess,strerror(errno));
    }*/
    return 0;

}
int enfs_filelock(int fd, struct lockargs liov)
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
	bw_log(APP_LOG_DEBUG, "file set lock false,error=%s", strerror(errno));
    }

    return ret;
}

int enfs_fileunLock(int fd, struct lockargs liov)
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
	bw_log(APP_LOG_DEBUG, "file set unlock false,error=%s",strerror(errno));
    }

    return ret;
}
#else
int proc_remote::doOpen(){
    return 0;
}
int enfs_filelock(int fd, struct lockargs liov){
    return 0;
}
int enfs_fileunLock(int fd, struct lockargs liov){
    return 0;
}
#endif


//shell commond.
//1:check bwfs service. 
//2:stop bwfs service.
//3:block client ip network.
//4:stop block client ip network.
int doShellCmd(bwfs_cmd cmd){
    if(cmd == SERVICE_CHECK){
	return GetEnfsdStatus();
    }
    if(cmd == START_SERVICE){
	return StartBwfsService();
    }
    if(cmd == STOP_SERVICE){
	return StopBwfsService();
    }
    if(cmd == BLOCK_CLIENT){
	return StartBlockClient();
    }
    if(cmd == STOP_BLOCK_CLIENT){
	return StopBlockClient();
    }
    return -1;
}
//return 1:not blocked
int StopBlockClient(){
    char *cmd = (char*)"service iptables stop > /dev/null ";
    int ret = system(cmd);		
    bw_log(APP_LOG_DEBUG, "Stop block ret = %d",ret);
    return (ret==0)?1:-1;
}
//return 1:blocked
int StartBlockClient(){
    char *cmd = (char*)"service iptables start > /dev/null ";
    int ret = system(cmd);		
    bw_log(APP_LOG_DEBUG, "Start block ret =%d",ret);
    return (ret==0)?1:-1;
}
//reurn 1:started
int StartBwfsService(){
    char *cmd = (char*)"service bwfs start > /dev/null ";
    system(cmd);		
    int ret = GetEnfsdStatus();		
    bw_log(APP_LOG_DEBUG, "get enfs status=%d",ret);
    return (ret==1);
}
//reurn 1:stopped
int StopBwfsService(){
    char *cmd = (char*)"service bwfs stop > /dev/null ";
    system(cmd);		
    int ret = GetEnfsdStatus();		
    bw_log(APP_LOG_DEBUG, "get enfs status=%d",ret);
    return (ret==0);
}
//return 1:enfsd running
int GetEnfsdStatus(){
    //char *cmd = "ps aux|grep enfsd |wc -l> 1.txt";
    //char *cmd = "ps aux|grep enfsd > 1.txt";
    char *cmd = (char*)"pstree |grep enfsd|wc -l > 1.txt";
    int err = system(cmd);		
    if(err < 0){
	err = -1;	
    }
    int fd_ps = open("1.txt",O_RDONLY);	
    if(fd_ps < 0){
	err = -2;
    }
    char enfsd_thread[8]={};
    int r_size = read(fd_ps,enfsd_thread,7);	
    if(r_size < 0){
	err = -3;
    }
    int ps_cnt = atoi(enfsd_thread);
    //printf("enfsd count =%d",ps_cnt);
    return (ps_cnt==1);
}
