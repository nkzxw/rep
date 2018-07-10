#include "common.h"

typedef struct _test_option{
    INT rw1;
    INT close;//0 no close,1 close
    INT delayTime;//0 no delay,x delay time
    INT rw2;
}TEST_OPTION,*PTEST_OPTION;

INT dwDesired[] = { 
    O_CREAT|O_RDONLY,
    O_CREAT|O_WRONLY,
    O_CREAT|O_RDWR
};

CHAR  dwDesireds[][17] = { 
    "O_CREAT|O_RDONLY",
    "O_CREAT|O_WRONLY",
    "O_CREAT|O_RDWR"
};

INT dwFlags[] = {
    F_RDLCK,F_WRLCK
};

#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define GENERIC_EXECUTE                  (0x20000000L)
#define GENERIC_ALL                      (0x10000000L)

#define FILE_SHARE_READ                 0x00000001  
#define FILE_SHARE_WRITE                0x00000002  
#define FILE_SHARE_DELETE               0x00000004  

#define OPEN_ALWAYS         4

struct lockiov{
    UINT64 dwFileOffset;
    UINT64 nNumberOfBytesToLock;
};

FILE *g_TableFileHandle = NULL;
CHAR target[MAX_PATH];
CHAR target2[MAX_PATH];

void logInit(char *pOutFile)
{
    char LogFile[MAX_PATH]="\0";
    sprintf(LogFile, "%s_tbl.log", pOutFile);

    DebugLogSetLogType(DEBUGLOG_STDOUT_FILE);
    g_TableFileHandle= fopen(LogFile, "w+");
    set_fd(g_TableFileHandle);
}

void logClose()
{
    if(g_TableFileHandle) {
	fflush(g_TableFileHandle);
	fclose(g_TableFileHandle);
    }
}

void ha_case1(class proc_remote *pr1){
    int	res;
    res = pr1->DoShell(SERVICE_CHECK);
    bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"service bwfs status = %s", (res==1)?"ok":"fail");
    res = pr1->DoShell(STOP_SERVICE);
    bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"stop bwfs service = %s", (res==1)?"ok":"fail");
    res = pr1->DoShell(START_SERVICE);
    bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"start bwfs service = %s", (res==1)?"ok":"fail");
    res = pr1->DoShell(BLOCK_CLIENT);
    bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"block client = %s", (res==1)?"ok":"fail");
    res = pr1->DoShell(STOP_BLOCK_CLIENT);
    bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"stop block client = %s", (res==1)?"ok":"fail");
}
//mds stop service, and start service
//client open file success
void ha_case2(class proc_remote *mds, class proc_remote *client)
{
    int res = -1;
    res = mds->DoShell(STOP_SERVICE);
    bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"stop bwfs service = %s", (res==1)?"ok":"fail");
    res = mds->DoShell(START_SERVICE);
    bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"start bwfs service = %s", (res==1)?"ok":"fail");
    struct openargs opena={
	"",
	GENERIC_READ,
	7,
	OPEN_ALWAYS
    };
    strcpy(opena.lpFileName,target);
    client->openfile(&opena);
    if (!client->checkopen2()){
	bw_log(APP_LOG_ERROR,"client open file error");
    }
}

//client1 lock file
//client2 lock file fail
//mds stop service, and start service
//client2 lock file fail
void ha_case3(class proc_remote *mds, class proc_remote *clnt1, class proc_remote *clnt2)
{

}
//client1 lock file
//client2 lock file fail
//mds block client and stop block
//client2 lock file success
void ha_case4(class proc_remote *mds, class proc_remote *clnt1, class proc_remote *clnt2)
{

}

static const char *progname = NULL;

void usage(void)
{
    fprintf(stderr, "usage: %s -d server -f file -c compare_file -o output -t case_type -l log_level\n"
	    "server: ip:port\n"
	    "file: test file path\n"
	    "compare_file: file path to compare\n"
	    "output: log file path\n"
	    "case_type: test case type\n", 
	    "log_level: set log level\n", 
	    progname);
    exit(1);
}
//#define ULONAG unsigned long
int main(int argc, char* argv[])
{
    progname = argv[0];
    int c;
    int server_index = 0;
    int case_type = 0;
    int log_level = -1;
    int conn_type = 0;
    char server[MAX_SERVER][M_SIZE]={"0"};
    char pOutFile[M_SIZE]="\0";
    sprintf(target,"%s","/opt/1.txt");
    sprintf(pOutFile,"%s","/opt/zmq");

    while (1) {
	struct option long_options[] = {
	    {"server",  1,  0, 'd'},
	    {"file",  1,  0, 'f'},
//	    {"compare",  1,  0, 'c'},
	    {"output",  1,  0, 'o'},
	    {"case_type",  1,  0, 't'},
	    {"log_level",  1,  0, 't'},
	    { NULL , NULL , NULL , NULL }
	};
	int option_index = 0;
	c = getopt_long(argc, argv, "d:f:o:t:l:", long_options, &option_index);
	if (c == -1)
	    break;
	switch (c) {
	    case 'd':
		sprintf(server[server_index],"%s",optarg);
		server_index++;
		break;
	    case 'f':
		sprintf(target,"%s",optarg);
		break;
	    case 'o':
		sprintf(pOutFile,"%s",optarg);
		break;
	    case 't':
		case_type = atoi(optarg);
		break;
	    case 'l':
		log_level = atoi(optarg);
		break;
	    default:
		printf ("non-option argv: %c,%s",c,optarg);
		usage();
	}
    }
    logInit(pOutFile);
    if(log_level >= 0)
	DebugLogSetLevel(log_level);
    bw_log(APP_LOG_DEBUG, "%-20s\t%-60s\t","process1","process2");
    bw_log(APP_LOG_DEBUG, "%s\t%-20s\t%-10s\t%-10s","AccessMode","AccessMode","Bwfs","nfs");

    void * g_context = zmq_init(1); 
    class proc_remote *pr1 = new proc_remote();
    class proc_remote *pr2 = new proc_remote();
    class proc_remote *pr3 = new proc_remote();
    //class proc_remote pr4={};

    if ((optind < argc) || (server_index < 1)) {
	printf ("not enough args\n");
	usage();
    }
    pr1->connect(conn_type, server[0], g_context);
    pr2->connect(conn_type,server[1], g_context);
    //pr3.connect(conn_type,server[2], g_context);
    //pr4.connect(conn_type,server[3]);
    ha_case1(pr1);
    ha_case2(pr1,pr2);
#if 0
    if (case_type & 1) {
	open_case1(pr1);
	open_case2(pr1, pr2);
	open_case2(pr1, pr3);
    }
    if (case_type&8){
	printf("start test lock\n");
	for(unsigned int kk=0; kk<sizeof(dwDesired)/sizeof(INT); kk++) {
	    struct openargs opena = {
		"",
		"",
		dwDesired[kk]
	    };
	    strcpy(opena.lpFileName,target);
	    strcpy(opena.lpFileName2,target2);

	    bw_log(APP_LOG_ERROR, "open file is  +++++++++++++++++ %s",dwDesireds[kk]);
	    //测试单进程一个范围加锁
	    lock_case1(pr1, opena, g_liov_1,sizeof(g_liov_1)/sizeof(lockiov));
	    bw_log(APP_LOG_ERROR, "lock_case1 end");
	    //测试单个进程，多个范围加锁
	    lock_case2(pr1, opena, g_liov_1,sizeof(g_liov_1)/sizeof(lockiov));
	    bw_log(APP_LOG_ERROR, "lock_case2 end");
	    lock_case3(pr1, pr2, opena, g_liov_1,sizeof(g_liov_1)/sizeof(lockiov));
	    bw_log(APP_LOG_ERROR, "lock_case3 end");
	    lock_case4(pr1, pr2, opena, g_liov_1,sizeof(g_liov_1)/sizeof(lockiov));
	    bw_log(APP_LOG_ERROR, "lock_case4 end");
	    //			lock_case5(pr1, pr2, pr3, pr4, opena, g_liov_1, sizeof(g_liov_1)/sizeof(lockiov));
	    //			bw_log(APP_LOG_ERROR, "lock_case5 end");
	    lock_case6(pr1, pr2, opena, g_liov_1,sizeof(g_liov_1)/sizeof(lockiov));
	    bw_log(APP_LOG_ERROR, "lock_case6_1 end");
	    //			lock_case6(pr1, pr3, opena, g_liov_1,sizeof(g_liov_1)/sizeof(lockiov));
	    bw_log(APP_LOG_ERROR, "lock_case6_2 end");
	    lock_case7(pr1, pr2, opena, g_liov_1,sizeof(g_liov_1)/sizeof(lockiov));
	    bw_log(APP_LOG_ERROR, "lock_case7_1 end");
	    //			lock_case7(pr1, pr3, opena, g_liov_1,sizeof(g_liov_1)/sizeof(lockiov));
	    bw_log(APP_LOG_ERROR, "lock_case7_2 end");
	    lock_case8(pr1, pr2, opena, g_liov_1,sizeof(g_liov_1)/sizeof(lockiov));
	    bw_log(APP_LOG_ERROR, "lock_case8 end");
	}
	printf("test lock end\r\n");
    }

    if (case_type&16) {
	printf("start test delegation start\n");
	delegationTestCase1(&pr1);
	printf("start test delegation case1 test end\n");
	delegationTestCase2_3(2,&pr1,&pr2);
	printf("start test delegation case2 test end\n");
	delegationTestCase2_3(3,&pr2,&pr3);
	printf("start test delegation end\n");
    }

#endif
    pr1->zclose(conn_type);
    pr2->zclose(conn_type);
    pr3->zclose(conn_type);
    //	pr4.zclose(conn_type);
    zmq_term(g_context);
    logClose();

    return 0;
}
BOOL File_Open_Close(struct openres *openfileR,class proc_remote *pr,BOOL BeOpen,int rw)
{
    BOOL ret=TRUE;
    struct openargs opena={
	"",
	rw
    };
    strcpy(opena.lpFileName,target);
    if (BeOpen){
	pr->openfilex(&opena,openfileR);
	if (!pr->checkopen2()){
	    if(pr->g_openr.hFile < 0){
		ret=FALSE;
		bw_log(APP_LOG_ERROR,"pr1 open file error");
	    }
	}
    } 
    else{
	pr->closefilex(openfileR);
    }

    return ret;
}

