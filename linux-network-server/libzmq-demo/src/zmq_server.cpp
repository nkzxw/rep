#include "common.h"
//#include "debuglog.h"

FILE *g_Handle=NULL;

void logInit(char *pOutFile)
{
	char LogFile[MAX_PATH]="\0";
	
	sprintf(LogFile, "%s.log", pOutFile);
	DebugLogSetLevel(APP_LOG_DEBUG);
	DebugLogSetLogType(DEBUGLOG_STDOUT_FILE);
	g_Handle = fopen(LogFile, "w+");
	set_fd(g_Handle);
	Log1(APP_LOG_DEBUG, "server log start");
}

void logClose()
{
	if(g_Handle) {
		fflush(g_Handle);
		fclose(g_Handle);
	}
}

int main(int argc, char* argv[])
{
	int port=5555;
	int c;
	char logename[64]="\0";
	

	int pid=getpid();
	
	
	sprintf(logename,"/opt/server_%d", pid);

	while (1) {
		struct option long_options[] = {
			{"port",  1,  0, 'p'},
			{ NULL , NULL , NULL , NULL }
		};
		int option_index = 0;
		c = getopt_long(argc, argv, "p:", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
			case 'p':
				sscanf(optarg,"%d",&port);
				break;
			default:
				printf ("non-option argv: %c,%s",c,optarg);
				return 0;
		}
	}

	if (optind < argc) {
		printf ("non-option argv");
		return 0;
	}
	
	logInit(logename);
	//int enfsd = system("pgrep enfsd");	
	//printf("enfsd %d\n",enfsd);
	{
	    //char *cmd_get_bwfs_status = "ps aux|grep enfsd |wc -l> 1.txt";
	    //char *cmd_get_bwfs_status = "ps aux|grep enfsd > 1.txt";
	    char *cmd_get_bwfs_status = (char*)"pstree |grep enfsd|wc -l > 1.txt";
	    char enfsd_thread[8]={};
	    int err = 0;//system(cmd_get_bwfs_status);		
	    if(err < 0){
		err = -1;	
	    }
	    int fd_ps = open("1.txt",O_RDONLY);	
	    if(fd_ps < 0){
		err = -2;
	    }
	    int r_size = read(fd_ps,enfsd_thread,7);	
	    if(r_size < 0){
		err = -3;
	    }
	    int ps_cnt = atoi(enfsd_thread);
	    printf("enfsd count =%d\n",ps_cnt);
	}
	void *g_context = zmq_init(1);
	int conn_type = 0;
	class proc_remote pr;
	if(pr.listen(conn_type, port,g_context)) {
		cout<<"listen port "<<port<<" fail"<<endl;
		return 0;
	}
	printf("listen port %d success\n",port);
	while(1) {
		pr.recv(conn_type);
		pr.process();
		pr.send(conn_type);
		fflush(g_Handle);
	}
	pr.zclose(conn_type);
	logClose();
	zmq_term(g_context);
	
	return 0;
}
