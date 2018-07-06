#include "common.h"

int proc_remote::process()
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

	case 3:
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
	case 13:
	    //shell commond.
	    //1:check bwfs service. 
	    //2:stop bwfs service.
	    //3:block client ip network.
	    decodeShell(&g_shell_cmd);
	    g_shell_res = doShellCmd(g_shell_cmd);
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
int proc_remote::doShellCmd(int cmd){
    return 0;
}

