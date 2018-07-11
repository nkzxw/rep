#include "common.h"
/*
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

struct lockiov{
    UINT64 dwFileOffset;
    UINT64 nNumberOfBytesToLock;
};
*/
#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define GENERIC_EXECUTE                  (0x20000000L)
#define GENERIC_ALL                      (0x10000000L)

#define FILE_SHARE_READ                 0x00000001  
#define FILE_SHARE_WRITE                0x00000002  
#define FILE_SHARE_DELETE               0x00000004  

#define OPEN_ALWAYS         4

#define LOCKFILE_FAIL_IMMEDIATELY   0x00000001
#define LOCKFILE_EXCLUSIVE_LOCK     0x00000002

FILE *g_TableFileHandle = NULL;
CHAR target[MAX_PATH];

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
//test shell cmd useful
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
//stop bwfs service, and start service
//client open file success
void ha_case2(class proc_remote *mds, class proc_remote *client)
{
    int res = -1;
    //res = mds->DoShell(STOP_SERVICE);
    //bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"stop bwfs service = %s", (res==1)?"ok":"fail");
    //res = mds->DoShell(START_SERVICE);
    //bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"start bwfs service = %s", (res==1)?"ok":"fail");
    struct openargs opena={
	"",
	GENERIC_WRITE,
	7,
	OPEN_ALWAYS
    };
    strcpy(opena.lpFileName,target);
    client->openfile(&opena);
    client->checkopen2();
    client->closefile();
}

//client1 lock file
//client2 lock file fail
//mds stop service, and start service
//client2 lock file fail
void ha_case3(class proc_remote *mds, class proc_remote *clnt1, class proc_remote *clnt2)
{
    struct openargs opena={
	"",
	GENERIC_WRITE,
	7,
	OPEN_ALWAYS
    };
    strcpy(opena.lpFileName,target);

    clnt1->openfile(&opena);
    clnt1->checkopen2();

    clnt2->openfile(&opena);
    clnt2->checkopen2();

    struct lockargs locka ={clnt1->g_openr.hFile,1,1,1};
    clnt1->lock(&locka);
    clnt1->checklock(1);
    struct lockargs lockb ={clnt2->g_openr.hFile,1,1,1};
    clnt2->lock(&lockb);
    clnt2->checklock(0);

    int res = -1;
    res = mds->DoShell(STOP_SERVICE);
    bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"stop bwfs service = %s", (res==1)?"ok":"fail");
    res = mds->DoShell(START_SERVICE);
    bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"start bwfs service = %s", (res==1)?"ok":"fail");

    clnt2->lock(&lockb);
    clnt2->checklock(0);

    clnt1->closefile();
    clnt2->closefile();
}

//client1 open lock file
//mds block client and stop block
//client1 lock file fail
void ha_case4(class proc_remote *mds_pasive, class proc_remote *mds_active, class proc_remote *clnt1)
{
    struct openargs opena={
	"",
	GENERIC_WRITE,
	7,
	OPEN_ALWAYS
    };
    strcpy(opena.lpFileName,target);
    clnt1->openfile(&opena);
    clnt1->checkopen2();
    int res = -1;
    res = mds_pasive->DoShell(BLOCK_CLIENT);
    bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"block client = %s", (res==1)?"ok":"fail");
    res = mds_pasive->DoShell(BLOCK_CLIENT);
    bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"block client = %s", (res==1)?"ok":"fail");
    res = mds_active->DoShell(STOP_BLOCK_CLIENT);
    bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"stop block client = %s", (res==1)?"ok":"fail");
    res = mds_active->DoShell(STOP_BLOCK_CLIENT);
    bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"stop block client = %s", (res==1)?"ok":"fail");

    struct lockargs locka ={clnt1->g_openr.hFile,1,1,1};
    clnt1->lock(&locka);
    clnt1->checklock(0);
    clnt1->closefile();
}
//client1 open lock file success
//mds block client and stop block
//client2 open and lock file success
void ha_case5(class proc_remote *mds_pasive, class proc_remote *mds_active, class proc_remote *clnt1, class proc_remote *clnt2)
{
    struct openargs opena={
	"",
	GENERIC_WRITE,
	7,
	OPEN_ALWAYS
    };
    strcpy(opena.lpFileName,target);

    clnt1->openfile(&opena);
    clnt1->checkopen2();
    struct lockargs locka ={clnt1->g_openr.hFile,1,1,1};
    clnt1->lock(&locka);
    clnt1->checklock(1);

    int res = -1;
    res = mds_pasive->DoShell(BLOCK_CLIENT);
    bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"block client = %s", (res==1)?"ok":"fail");
    res = mds_pasive->DoShell(STOP_BLOCK_CLIENT);
    bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"stop block client = %s", (res==1)?"ok":"fail");
    res = mds_active->DoShell(BLOCK_CLIENT);
    bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"block client = %s", (res==1)?"ok":"fail");
    res = mds_active->DoShell(STOP_BLOCK_CLIENT);
    bw_log((res==1)?APP_LOG_DEBUG:APP_LOG_ERROR,"stop block client = %s", (res==1)?"ok":"fail");

    clnt2->openfile(&opena);
    clnt2->checkopen2();
    struct lockargs lockb ={clnt2->g_openr.hFile,1,1,1};
    clnt2->lock(&lockb);
    clnt2->checklock(1);

    clnt1->closefile();
    clnt2->closefile();
}


static const char *progname = NULL;

void usage(void)
{
    fprintf(stderr, "usage: %s -m mds -d server -f file -c compare_file -o output -t case_type -l log_level\n"
	    "server: ip:port\n"
	    "mds server: ip:port\n"
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
    int case_type = 0;
    int log_level = -1;
    int conn_type = 0;
    int server_index = 0;
    char server[MAX_SERVER][M_SIZE]={"0"};
    int mds_index = 0;
    char mds[MAX_SERVER][M_SIZE]={"0"};
    char pOutFile[M_SIZE]="\0";
    sprintf(target,"%s","/opt/1.txt");
    sprintf(pOutFile,"%s","/opt/zmq");

    while (1) {
	struct option long_options[] = {
	    {"server",  1,  0, 'd'},
	    {"mds",  1,  0, 'm'},
	    {"file",  1,  0, 'f'},
	    //	    {"compare",  1,  0, 'c'},
	    {"output",  1,  0, 'o'},
	    {"case_type",  1,  0, 't'},
	    {"log_level",  1,  0, 't'},
	    { NULL , NULL , NULL , NULL }
	};
	int option_index = 0;
	c = getopt_long(argc, argv, "m:d:f:o:t:l:", long_options, &option_index);
	if (c == -1)
	    break;
	switch (c) {
	    case 'm':
		sprintf(mds[mds_index],"%s",optarg);
		mds_index++;
		break;
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
    //class proc_remote pr4={};
    if ((optind < argc) || (server_index < 1)) {
	printf ("not enough args\n");
	usage();
    }
    class proc_remote *mds_pr[10];
    class proc_remote *pr[10];
    for(int i=0;i < mds_index;i++){
	mds_pr[i]= new proc_remote();
	mds_pr[i]->connect(conn_type,mds[i], g_context);
    }
    for(int i=0;i < server_index;i++){
	pr[i]= new proc_remote();
	pr[i]->connect(conn_type,server[i], g_context);
    }
    //pr3.connect(conn_type,server[2], g_context);
    //pr4.connect(conn_type,server[3]);
    //ha_case1(pr1);
    ha_case2(mds_pr[0],pr[0]);
#if 0
    if (case_type & 1) {
	open_case1(pr1);
	open_case2(pr1, pr2);
	open_case2(pr1, pr3);
    }
#endif
    for(int i=0;i < mds_index;i++){
	mds_pr[i]->zclose(conn_type);
    }
    for(int i=0;i < mds_index;i++){
	pr[i]->zclose(conn_type);
    }
    zmq_term(g_context);
    logClose();

    return 0;
}

