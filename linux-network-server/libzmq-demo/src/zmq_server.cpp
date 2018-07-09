#include "common.h"

FILE *g_Handle=NULL;

void logInit(char *pOutFile)
{
    char LogFile[MAX_PATH]="\0";

    sprintf(LogFile, "%s.log", pOutFile);
    DebugLogSetLevel(APP_LOG_DEBUG);
    DebugLogSetLogType(DEBUGLOG_STDOUT_FILE);
    g_Handle = fopen(LogFile, "w+");
    set_fd(g_Handle);
    bw_log(APP_LOG_DEBUG, "server log start");
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

    //int pid=getpid();
    sprintf(logename,"/opt/server_%d", port);

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
    void *g_context = zmq_init(1);
    int conn_type = 0;
    class proc_remote pr;
    if(pr.listen(conn_type, port,g_context)) {
	printf("listen port %d fail\n",port);
	return 0;
    }
    printf("listen port %d success\n",port);
    while(1) {
	pr.recv(conn_type);
	pr.process();
	pr.send(conn_type);
	//fflush(g_Handle);
    }
    pr.zclose(conn_type);
    logClose();
    zmq_term(g_context);

    return 0;
}
