#include "common.h"

typedef struct _test_option{
	INT rw1;
	INT close;//0 no close,1 close
	INT delayTime;//0 no delay,x delay time
	INT rw2;
}TEST_OPTION,*PTEST_OPTION;

TEST_OPTION g_delegation_testoption[]={
	{O_CREAT|O_RDONLY,0,0,O_CREAT|O_RDONLY},
	{O_CREAT|O_RDONLY,0,0,O_CREAT|O_WRONLY},
	{O_CREAT|O_WRONLY,0,0,O_CREAT|O_RDONLY},
	{O_CREAT|O_WRONLY,0,0,O_CREAT|O_WRONLY},
	{O_CREAT|O_RDONLY,1,0,O_CREAT|O_RDONLY},
	{O_CREAT|O_RDONLY,1,0,O_CREAT|O_WRONLY},
	{O_CREAT|O_WRONLY,1,0,O_CREAT|O_RDONLY},
	{O_CREAT|O_WRONLY,1,0,O_CREAT|O_WRONLY},
	{O_CREAT|O_RDONLY,1,40,O_CREAT|O_RDONLY},
	{O_CREAT|O_RDONLY,1,40,O_CREAT|O_WRONLY},
	{O_CREAT|O_WRONLY,1,40,O_CREAT|O_RDONLY},
	{O_CREAT|O_WRONLY,1,40,O_CREAT|O_WRONLY}
};

enum TestCase_DelegationTestResultStatus{
	NO_DELEGATION=0,
	GET_DELEGATION,
	DELEGATION_CHANGED,
	DELEGATION_UNCHANGED
};

CHAR gDelegationTestCase1_res[]={
DELEGATION_UNCHANGED,DELEGATION_UNCHANGED,DELEGATION_UNCHANGED,DELEGATION_UNCHANGED,
DELEGATION_UNCHANGED,DELEGATION_UNCHANGED,DELEGATION_UNCHANGED,DELEGATION_UNCHANGED,
DELEGATION_CHANGED,DELEGATION_CHANGED,DELEGATION_CHANGED,DELEGATION_CHANGED
};

CHAR gDelegationTestCase2_res[]={
	DELEGATION_UNCHANGED,DELEGATION_UNCHANGED,DELEGATION_UNCHANGED,DELEGATION_UNCHANGED,
	DELEGATION_UNCHANGED,DELEGATION_UNCHANGED,DELEGATION_UNCHANGED,DELEGATION_UNCHANGED,
	DELEGATION_CHANGED,DELEGATION_CHANGED,DELEGATION_CHANGED,DELEGATION_CHANGED
};

CHAR gDelegationTestCase3_res[]={
NO_DELEGATION,NO_DELEGATION,NO_DELEGATION,NO_DELEGATION,
GET_DELEGATION,GET_DELEGATION,GET_DELEGATION,GET_DELEGATION,
GET_DELEGATION,GET_DELEGATION,GET_DELEGATION,GET_DELEGATION
};

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

struct lockiov g_liov_1[]={
	{0,0},//0
	{0,10},//1//
	{1,7},//2//
	{6,5},//3//
	{6,6},//4//
	{8,3},//5
	{9,20},//6
	{10,20},//7
	{11,20},//8
	{0,20}//9
};

FILE *g_TableFileHandle = NULL;
CHAR target[MAX_PATH];
CHAR target2[MAX_PATH];

#define open_construct(opena,i)\
struct openargs opena={\
	"",\
	"",\
	dwDesired[i],\
};\
	strcpy(opena.lpFileName,target);\
	strcpy(opena.lpFileName2,target2);

#define lock_construct(p,k,v,i) \
{\
	p.g_openr.hFile,\
	p.g_openr.hFile2,\
	dwFlags[k],\
	(DWORD)(v[i].dwFileOffset ),\
	(DWORD)(v[i].nNumberOfBytesToLock),\
}

#define log_open_2(i1,i2,k1,k2)\
	do{\
	printf("open indexes = %d,%d,%d,%d\n",i1,k1,i2,k2);\
	Log5(APP_LOG_DEBUG, "open Desired-Share: [0x%x, 0x%x]->[0x%x, 0x%x]",\
	dwDesired[i1],\
	dwShare[k1],\
	dwDesired[i2],\
	dwShare[k2]);\
	}while (0);
	
#define log_lock_1(v,i1,k1) \
	do{ printf("log_lock_1 range indexes = %d\n",i1);\
	log_msg(APP_LOG_DEBUG, "lock: offset-len-IsExc:[0x%llu,0x%llu,0x%x]",\
	v[i1].dwFileOffset,\
	v[i1].nNumberOfBytesToLock,\
	dwFlags[k1]%2);\
	} while (0);

#define log_lock_2(v,k1,k2,i1,i2) \
	do{ printf("log_lock_2 range indexes = %d,%d\n",i1,i2);\
	Log7(APP_LOG_DEBUG, "lock: offset-length-IsExc:[0x%llu,0x%llu,0x%x]->[0x%llu,0x%llu,0x%x]",\
	v[i1].dwFileOffset,\
	v[i1].nNumberOfBytesToLock,\
	dwFlags[k1]%2,\
	v[i2].dwFileOffset,\
	v[i2].nNumberOfBytesToLock,\
	dwFlags[k2]%2);\
	} while (0);

#define log_lock_3(v,k1,k2,k3,i1,i2,i3) \
	do{ printf("log_lock_3 range indexes = %d,%d,%d\n",i1,i2,i3);\
	log_msg(APP_LOG_DEBUG, "lock: offset-len-IsExc:[0x%llu,0x%llu,0x%x]->[0x%llu,0x%llu,0x%x]->[0x%llu,0x%llu,0x%x]",\
	v[i1].dwFileOffset,\
	v[i1].nNumberOfBytesToLock,\
	dwFlags[k1]%2,\
	v[i2].dwFileOffset,\
	v[i2].nNumberOfBytesToLock,\
	dwFlags[k2]%2,\
	v[i3].dwFileOffset,\
	v[i3].nNumberOfBytesToLock,\
	dwFlags[k3]%2);\
	} while (0);

#define log_lock_4(v,k1,k2,k3,k4,i1,i2,i3,i4) \
	do{	printf("log_lock_4 range indexes = %d,%d,%d,%d\n",i1,i2,i3,i4);\
	log_msg(APP_LOG_DEBUG, "lock: offset-len-isExcl:[0x%llu,0x%llu,0x%x],[0x%llu,0x%llu,0x%x],[0x%llu,0x%llu,0x%x],[0x%llu,0x%llu,0x%x]",\
	v[i1].dwFileOffset,\
	v[i1].nNumberOfBytesToLock,\
	dwFlags[k1]%2,\
	v[i2].dwFileOffset,\
	v[i2].nNumberOfBytesToLock,\
	dwFlags[k2]%2,\
	v[i3].dwFileOffset,\
	v[i3].nNumberOfBytesToLock,\
	dwFlags[k3]%2,\
	v[i4].dwFileOffset,\
	v[i4].nNumberOfBytesToLock,\
	dwFlags[k4]%2);\
	} while (0);

#define for_open_1(i1) \
	for(i1=0;i1<sizeof(dwDesired)/sizeof(INT);i1++)

#define for_open_2(i1,i2) \
	for_open_1(i1)\
	for(i2=0;i2<sizeof(dwDesired)/sizeof(INT);i2++)

	
#define for_lock_1(i1,k1,len) \
	printf("for_lock_1  \n");\
	for(i1=0; i1<len; i1++) \
	for(k1=0;k1<sizeof(dwFlags)/sizeof(INT);k1++)
	
#define for_lock_2(k1,k2,i1,i2,len) \
	printf("for_lock_2  \n");\
	for_lock_1(i1,k1,len) \
	for(i2=i1;i2<len;i2++)\
	for(k2=0;k2<sizeof(dwFlags)/sizeof(INT);k2++)

#define for_lock_3(k1,k2,k3,i1,i2,i3,len) \
	printf("for_lock_3  \n");\
	for_lock_2(k1,k2,i1,i2,len) \
	for(i3=i2;i3<len;i3++)\
	for(k3=0;k3<sizeof(dwFlags)/sizeof(INT);k3++)

#define for_lock_4(k1,k2,k3,k4,i1,i2,i3,i4,len) \
	printf("for_lock_4  \n");\
	for_lock_3(k1,k2,k3,i1,i2,i3,len) \
	for(i4=i3;i4<len;i4++)\
	for(k4=0;k4<sizeof(dwFlags)/sizeof(INT);k4++)

void StrTime(char *pTime)
{
	time_t t = time(0);  
	strftime(pTime, 32, "%Y/%m/%d %X",localtime(&t) ); 
}

int open_case1(class proc_remote pr1)
{
	unsigned int i1=0;
	struct openargs opena;
	
	for_open_1(i1) {
		printf("open_case1 i1=%ud\n",i1);
		open_construct(opena,i1);
		pr1.openfile(&opena);
		if ((pr1.g_openr.hFile < 0 || pr1.g_openr.hFile2 < 0) || 
			(pr1.g_openr.hFile < 0 || pr1.g_openr.hFile2 > 0) ||
			(pr1.g_openr.hFile > 0 || pr1.g_openr.hFile2 < 0)) {
			Log2(APP_LOG_DEBUG, "open once on fail,Desired-Share:[%d]", dwDesired[i1]);
			continue;
		}
		else {
			Log2(APP_LOG_DEBUG, "open once success, Desired-Share:[%d]", dwDesired[i1]);
		}
		pr1.closefile();
	}
	return 0;
}

int open_case2(class proc_remote pr1,class proc_remote pr2)
{
	unsigned int i1=0,i2=0;
	struct openargs opena;
	struct openargs openb;
	
	for_open_2(i1,i2) 
	{
		open_construct(opena,i1);
		open_construct(openb,i2);
		pr1.openfile(&opena);
		pr2.openfile(&openb);
//		log_open_2(i1,i2,k1,k2);
		pr2.checkopen();
		pr1.closefile();
		pr2.closefile();
	}
	return 0;
}


// process1:	locka unlocka
int lock_case1(class proc_remote pr1, struct openargs opena, struct lockiov* mliov, unsigned int len)
{
	int ret=0;
	unsigned int i1,k1;
	
	for_lock_1(i1,k1,len) {
		Log3(APP_LOG_ERROR, "lock_case1 pr1 opena[%ud],%ud",i1,k1);
		pr1.openfile(&opena);
//		log_lock_1(mliov,i1,k1);
		if (pr1.g_openr.hFile < 0) {
			Log2(APP_LOG_DEBUG, "open once failed,%x", opena.dwDesiredAccess);
		}
		struct lockargs locka = lock_construct(pr1, k1, mliov, i1);
		pr1.lock(&locka);
		ret = pr1.checklock();
		if(ret == 1 || ret == 2) {
			Log6(APP_LOG_DEBUG, "lock,offset-len-locktype:[0x%x,0x%x,0x%x]->[%d,%d]",
				mliov[i1].dwFileOffset,
				mliov[i1].nNumberOfBytesToLock,
				dwFlags[k1],
				pr1.g_lockr.res,
				pr1.g_lockr.res2);
//			pr1.closefile();
//			continue;
		}
		if(!ret){
			pr1.unlock(&locka);
			ret = pr1.checkunlock();
			if(ret == 1 || ret == 2) {
				Log6(APP_LOG_DEBUG, "unlock,offset-len-locktype:[0x%x,0x%x,0x%x]->[%d,%d]",
					mliov[i1].dwFileOffset,
					mliov[i1].nNumberOfBytesToLock,
					dwFlags[k1],
					pr1.g_lockr.res,
					pr1.g_lockr.res2);
//				pr1.closefile();
//				continue;
			}
		}
		pr1.closefile();
	}
	return 0;
}

// process1: locka lockb
int lock_case2(class proc_remote pr1, struct openargs opena, struct lockiov* mliov, unsigned int len)
{
	static int count = 0;
	unsigned int k1,k2,i1,i2;
	int pr1ret=0,pr2ret=0;
	
	for_lock_2(k1,k2,i1,i2,len) {
		count++;
		Log3(APP_LOG_ERROR, "lock_case2 pr1 opena[%ud],%ud",i1,k1);
		Log3(APP_LOG_ERROR, "lock_case2 pr1 openb[%ud],%ud",i2,k2);
		pr1.openfile(&opena);
		struct lockargs locka = lock_construct(pr1, k1, mliov, i1);
		struct lockargs lockb = lock_construct(pr1, k2, mliov, i2);
//		log_lock_2(mliov,k1,k2,i1,i2);
		pr1.lock(&locka);
		pr1ret = pr1.checklock();
		if(pr1ret == 1 || pr1ret == 2) {
			Log6(APP_LOG_DEBUG, "lock false,offset-len-locktype:[0x%x,0x%x]->[%d,%d]",
				mliov[i1].dwFileOffset,
				mliov[i1].nNumberOfBytesToLock,
				dwFlags[k1],
				pr1.g_lockr.res,
				pr1.g_lockr.res2);
//			pr1.closefile();
//			continue;
		}
		
		pr1.lock(&lockb);	
		pr2ret = pr1.checklock();
		if(pr2ret == 1 || pr2ret == 2) {
			Log9(APP_LOG_DEBUG, "second lock false, offset-len-locktype: [0x%x,0x%x,0x%x],[0x%x,0x%x,0x%x]->[%d,%d]",
				mliov[i1].dwFileOffset,
				mliov[i1].nNumberOfBytesToLock,
				dwFlags[k1],
				mliov[i2].dwFileOffset,
				mliov[i2].nNumberOfBytesToLock,
				dwFlags[k2],
				pr1.g_lockr.res,
				pr1.g_lockr.res2);
//			pr1.closefile();
//			continue;
		}
		if(!pr1ret){
			pr1.unlock(&locka);
			if(pr1.checkunlock()){
				Log6(APP_LOG_DEBUG, "unlock false,offset-len-locktype:[0x%x,0x%x,0x%x]->[%d,%d]",
					mliov[i1].dwFileOffset,
					mliov[i1].nNumberOfBytesToLock,
					dwFlags[k1],
					pr1.g_lockr.res,
					pr1.g_lockr.res2);
//				pr1.closefile();
//				continue;
			}
		}
		if(!pr2ret){
			pr1.unlock(&lockb);
			if(pr1.checkunlock()){
				Log9(APP_LOG_ERROR, "unlock false, offset-len-locktype: [0x%x,0x%x,0x%x],[0x%x,0x%x,0x%x]->[%d,%d]",
					mliov[i1].dwFileOffset,
					mliov[i1].nNumberOfBytesToLock,
					dwFlags[k1],
					mliov[i2].dwFileOffset,
					mliov[i2].nNumberOfBytesToLock,
					dwFlags[k2],
					pr1.g_lockr.res,
					pr1.g_lockr.res2);
//				pr1.closefile();
//				continue;
			}
		}
		pr1.closefile();
	}
	
	return 0;
}

//2 process
//process1:	locka
//process2:       lockb     

int lock_case3(class proc_remote pr1, class proc_remote pr2, struct openargs opena, struct lockiov* mliov, unsigned int len)
{ 
	int pr1ret=0,pr2ret=0;
	unsigned int k1,k2,i1,i2;
	
	for_lock_2(k1,k2,i1,i2,len) {
		Log3(APP_LOG_ERROR, "lock_case3 pr1 opena[%ud],%ud",i1,k1);
		Log3(APP_LOG_ERROR, "lock_case3 pr2 openb[%ud],%ud",i2,k2);
		pr1.openfile(&opena);
		pr2.openfile(&opena);
		struct lockargs locka = lock_construct(pr1, k1, mliov, i1);
		struct lockargs lockb = lock_construct(pr2, k2, mliov, i2);
		pr1.lock(&locka);
		pr1ret = pr1.checklock();
		if(pr1ret == 1 || pr1ret == 2) {
			Log6(APP_LOG_DEBUG, "lock false,offset-len-locktype:[0x%x,0x%x,0x%x]->[%d,%d]",
				mliov[i1].dwFileOffset,
				mliov[i1].nNumberOfBytesToLock,
				dwFlags[k1],
				pr1.g_lockr.res,pr1.g_lockr.res2);
//			pr1.closefile();
//			pr2.closefile();
//			continue;
		}
		pr2.lock(&lockb);
		pr2ret = pr2.checklock();
		if(pr2ret == 1 || pr2ret == 2) {
			log_msg(APP_LOG_DEBUG, "second lock index = %d,%d false, offset-len-locktype: [0x%x,0x%x,0x%x],[0x%x,0x%x,0x%x]->[%d,%d]",
				i1,i2,
				mliov[i1].dwFileOffset,
				mliov[i1].nNumberOfBytesToLock,
				dwFlags[k1],
				mliov[i2].dwFileOffset,
				mliov[i2].nNumberOfBytesToLock,
				dwFlags[k2],
				pr2.g_lockr.res,
				pr2.g_lockr.res2);
//			pr1.closefile();
//			pr2.closefile();
//			continue;
		}
		if(!pr1ret){
			pr1.unlock(&locka);
			if(pr1.checkunlock()){
				Log6(APP_LOG_DEBUG, "unlock false,offset-len-locktype:[0x%x,0x%x,0x%x]->[%d,%d]",
					mliov[i1].dwFileOffset,
					mliov[i1].nNumberOfBytesToLock,
					dwFlags[k1],
					pr1.g_lockr.res,pr1.g_lockr.res2);
//				pr1.closefile();
//				pr2.closefile();
//				continue;
			}
		}
		if(!pr2ret){
			pr2.unlock(&lockb);
			if(pr2.checkunlock()){
				log_msg(APP_LOG_DEBUG, "unlock index = %d,%d false, offset-len-locktype: [0x%x,0x%x,0x%x],[0x%x,0x%x,0x%x]->[%d,%d]",
					i1,i2,
					mliov[i1].dwFileOffset,
					mliov[i1].nNumberOfBytesToLock,
					dwFlags[k1],
					mliov[i2].dwFileOffset,
					mliov[i2].nNumberOfBytesToLock,
					dwFlags[k2],
					pr2.g_lockr.res,
					pr2.g_lockr.res2);
//				pr1.closefile();
//				pr2.closefile();
//				continue;
			}
		}
		pr1.closefile();
		pr2.closefile();
	}
	
	return 0;
}

//2 process
//process1:	locka lockb
//process2:				lockc      
int lock_case4(class proc_remote pr1, class proc_remote pr2, struct openargs opena, struct lockiov* mliov, unsigned int len)
{
	unsigned int k1,k2,k3,i1,i2,i3;
	
	for_lock_3(k1,k2,k3,i1,i2,i3,len) {
		Log3(APP_LOG_ERROR, "lock_case4 pr1 opena[%ud],%ud",i1,k1);
		Log3(APP_LOG_ERROR, "lock_case4 pr1 openb[%ud],%ud",i2,k2);
		Log3(APP_LOG_ERROR, "lock_case4 pr2 openc[%ud],%ud",i3,k3);
		pr1.openfile(&opena);
		pr2.openfile(&opena);
		struct lockargs locka = lock_construct(pr1,k1,mliov,i1);
		struct lockargs lockb = lock_construct(pr1,k2,mliov,i2);
		struct lockargs lockc = lock_construct(pr2,k3,mliov,i3);
//		log_lock_3(mliov,k1,k2,k3,i1,i2,i3);
		pr1.lock(&locka);
		pr1.checklock();
		pr1.lock(&lockb);
		pr1.checklock();
		pr2.lock(&lockc);
		pr2.checklock();
		pr1.closefile();
		pr2.closefile();
	}
	
	return 0;
}

//4 process, 1 range
int lock_case5(class proc_remote pr1,class proc_remote pr2,
class proc_remote pr3,class proc_remote pr4,
struct openargs opena,struct lockiov* mliov, unsigned int len)
{
	unsigned int k1,k2,k3,k4,i1,i2,i3,i4;
	
	for_lock_4(k1,k2,k3,k4,i1,i2,i3,i4,len) {
		Log3(APP_LOG_ERROR, "lock_case5 pr1 opena[%ud],%ud",i1,k1);
		Log3(APP_LOG_ERROR, "lock_case5 pr2 openb[%ud],%ud",i2,k2);
		Log3(APP_LOG_ERROR, "lock_case5 pr3 openc[%ud],%ud",i3,k3);
		Log3(APP_LOG_ERROR, "lock_case5 pr4 opena[%ud],%ud",i4,k4);
		pr1.openfile(&opena);
		pr2.openfile(&opena);
		pr3.openfile(&opena);
		pr4.openfile(&opena);
		struct lockargs locka = lock_construct(pr1,k1,mliov,i1);
		struct lockargs lockb = lock_construct(pr2,k2,mliov,i2);
		struct lockargs lockc = lock_construct(pr3,k3,mliov,i3);
		struct lockargs lockd = lock_construct(pr4,k4,mliov,i4);
//		log_lock_4(mliov,k1,k2,k3,k4,i1,i2,i3,i4)
		pr1.lock(&locka);
		pr1.checklock();
		pr2.lock(&lockb);
		pr2.checklock();
		pr3.lock(&lockc);
		pr3.checklock();
		pr4.lock(&lockd);
		pr4.checklock();
		pr1.closefile();
		pr2.closefile();
		pr3.closefile();
		pr4.closefile();
	}
	return 0;
}

//2 process
//process1:	locka lockb unlocka
//process2:						lockc      
int lock_case6(class proc_remote pr1, class proc_remote pr2, struct openargs opena, struct lockiov* mliov, unsigned int len)
{
	unsigned int k1,k2,k3,i1,i2,i3;
	
	for_lock_3(k1,k2,k3,i1,i2,i3,len) {
		Log3(APP_LOG_ERROR, "lock_case6 pr1 opena[%ud],%ud",i1,k1);
		Log3(APP_LOG_ERROR, "lock_case6 pr1 openb[%ud],%ud",i2,k2);
		Log3(APP_LOG_ERROR, "lock_case6 pr2 openc[%ud],%ud",i3,k3);
		pr1.openfile(&opena);
		pr2.openfile(&opena);
		struct lockargs locka = lock_construct(pr1,k1,mliov,i1);
		struct lockargs lockb = lock_construct(pr1,k2,mliov,i2);
		struct lockargs lockc = lock_construct(pr2,k3,mliov,i3);
//		log_lock_3(mliov,k1,k2,k3,i1,i2,i3);
		pr1.lock(&locka);
		pr1.checklock();
		pr1.lock(&lockb);
		pr1.checklock();
		pr1.unlock(&locka);
		pr2.lock(&lockc);
		pr2.checklock();
		pr1.closefile();
		pr2.closefile();
	}
	return 0;
}

//2 process
//process1:	lock a       unlock a   lock c
//process2:        lockb
int lock_case7(class proc_remote pr1, class proc_remote pr2, struct openargs opena, struct lockiov* mliov, unsigned int len)
{
	unsigned int k1,k2,k3,i1,i2,i3;
	
	for_lock_3(k1,k2,k3,i1,i2,i3,len) {
		Log3(APP_LOG_ERROR, "lock_case7 pr1 opena[%ud],%ud",i1,k1);
		Log3(APP_LOG_ERROR, "lock_case7 pr1 openb[%ud],%ud",i2,k2);
		Log3(APP_LOG_ERROR, "lock_case7 pr2 openc[%ud],%ud",i3,k3);
		pr1.openfile(&opena);
		pr2.openfile(&opena);
		struct lockargs locka = lock_construct(pr1,k1,mliov,i1);
		struct lockargs lockb = lock_construct(pr1,k2,mliov,i2);
		struct lockargs lockc = lock_construct(pr2,k3,mliov,i3);
//		log_lock_3(mliov,k1,k2,k3,i1,i2,i3);
		pr1.lock(&locka);
		pr1.checklock();
		pr2.lock(&lockb);
		pr2.checklock();
		pr1.unlock(&locka);
		pr1.lock(&lockc);
		pr1.checklock();
		pr1.closefile();
		pr2.closefile();
	}
	return 0;
}

//2 process
//process1:	locka lockb unlockb
//process2:						lockc      
int lock_case8(class proc_remote pr1, class proc_remote pr2, struct openargs opena, struct lockiov* mliov, unsigned int len)
{
	unsigned int k1,k2,k3,i1,i2,i3;
	
	for_lock_3(k1,k2,k3,i1,i2,i3,len) {
		Log3(APP_LOG_ERROR, "lock_case8 pr1 opena[%ud],%ud",i1,k1);
		Log3(APP_LOG_ERROR, "lock_case8 pr1 openb[%ud],%ud",i2,k2);
		Log3(APP_LOG_ERROR, "lock_case8 pr3 openc[%ud],%ud",i3,k3);
		pr1.openfile(&opena);
		pr2.openfile(&opena);
		struct lockargs locka = lock_construct(pr1,k1,mliov,i1);
		struct lockargs lockb = lock_construct(pr1,k2,mliov,i2);
		struct lockargs lockc = lock_construct(pr2,k3,mliov,i3);
//		log_lock_3(mliov,k1,k2,k3,i1,i2,i3);
		pr1.lock(&locka);
		pr1.checklock();
		pr1.lock(&lockb);
		pr1.checklock();
		pr1.unlock(&lockb);
		pr2.lock(&lockc);
		pr2.checklock();
		pr1.closefile();
		pr2.closefile();
	}
	return 0;
}

//2 process
//process1:	lock a       
//process2:        lockb lock c
int lock_case_1_2(class proc_remote pr1, class proc_remote pr2, struct openargs opena, struct lockiov* mliov, unsigned int len)
{
	unsigned int k1,k2,k3,i1,i2,i3;
	
	for_lock_3(k1,k2,k3,i1,i2,i3,len) {
		pr1.openfile(&opena);
		pr2.openfile(&opena);
		struct lockargs locka = lock_construct(pr1,k1,mliov,i1);
		struct lockargs lockb = lock_construct(pr1,k2,mliov,i2);
		struct lockargs lockc = lock_construct(pr2,k3,mliov,i3);
//		log_lock_3(mliov,k1,k2,k3,i1,i2,i3);
		pr1.lock(&locka);
		pr1.checklock();
		pr2.lock(&lockb);
		pr2.checklock();
		pr2.lock(&lockc);
		pr2.checklock();
		pr1.closefile();
		pr2.closefile();
	}
	return 0;
}

void logInit(char *pOutFile)
{
	char LogFile[MAX_PATH]="\0";
	sprintf(LogFile, "%s_tbl.log", pOutFile);
	
	DebugLogSetLevel(APP_LOG_DEBUG);
	DebugLogSetLogType(DEBUGLOG_STDOUT_FILE);
	g_TableFileHandle= fopen(LogFile, "w+");
	set_fd(g_TableFileHandle);
	Log3(APP_LOG_DEBUG, "%-20s\t%-60s\t","process1","process2");
	Log5(APP_LOG_DEBUG, "%s\t%-20s\t%-10s\t%-10s","AccessMode","AccessMode","Bwfs","nfs");
}

void logClose()
{
	if(g_TableFileHandle) {
		fflush(g_TableFileHandle);
		fclose(g_TableFileHandle);
	}
}

void delegationTestCase1(class proc_remote *pr1);
//multiple procs or multiple clients
void delegationTestCase2_3(int testCase,class proc_remote *pr1,class proc_remote *pr2);

void ha_case1(class proc_remote *pr1){
    int	res = pr1->DoShell(SERVICE_CHECK);
    printf("service bwfs status = %s\n", (pr1->g_shell_res==1)?"ok":"fail");
    res = pr1->DoShell(STOP_SERVICE);
    printf("service bwfs stop = %s\n", (pr1->g_shell_res==1)?"ok":"fail");
    res = pr1->DoShell(START_SERVICE);
    printf("service bwfs start = %s\n", (pr1->g_shell_res==1)?"ok":"fail");
    res = pr1->DoShell(BLOCK_CLIENT);
    printf("block client = %s\n", (pr1->g_shell_res==1)?"ok":"fail");
    res = pr1->DoShell(STOP_BLOCK_CLIENT);
    printf("not block client = %s\n", (pr1->g_shell_res==1)?"ok":"fail");
}

static const char *progname = NULL;

void usage(void)
{
    fprintf(stderr, "usage: %s -d server -f file -c compare_file -o output -t case_type\n"
	    "server: ip:port\n"
	    "file: test file path\n"
	    "compare_file: file path to compare\n"
	    "output: log file path\n"
	    "case_type: test case type\n", 
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
	int conn_type = 0;
	char server[MAX_SERVER][M_SIZE]={"0"};
	char pOutFile[M_SIZE]="\0";
	sprintf(target,"%s","/opt/1.txt");
	sprintf(target2,"%s","/opt/2.txt");
	sprintf(pOutFile,"%s","/opt/zmq");

	while (1) {
		struct option long_options[] = {
			{"server",  1,  0, 'd'},
			{"file",  1,  0, 'f'},
			{"compare",  1,  0, 'c'},
			{"output",  1,  0, 'o'},
			{"case_type",  1,  0, 't'},
			{ NULL , NULL , NULL , NULL }
		};
		int option_index = 0;
		c = getopt_long(argc, argv, "d:f:c:o:t:", long_options, &option_index);
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
		case 'c':
			sprintf(target2,"%s",optarg);
			break;
		case 'o':
			sprintf(pOutFile,"%s",optarg);
			break;
		case 't':
			case_type = atoi(optarg);
			break;
		default:
			printf ("non-option argv: %c,%s",c,optarg);
			usage();
		}
	}
	logInit(pOutFile);

	class proc_remote pr1;
	class proc_remote pr2;
	class proc_remote pr3;
	//class proc_remote pr4={};
	
	void * g_context = zmq_init(1); 
	int res = pr1.connect(conn_type, server[0], g_context);

	ha_case1(&pr1);

	if ((optind < argc) || (server_index < 2)) {
		printf ("not enough args\n");
		usage();
	}
	pr1.connect(conn_type,server[0], g_context);
	pr2.connect(conn_type,server[1], g_context);
	pr3.connect(conn_type,server[2], g_context);

//	pr4.connect(conn_type,server[3]);
#if 0
	if (case_type & 1) {
		open_case1(pr1);
		open_case2(pr1, pr2);
		open_case2(pr1, pr3);
	}
#endif
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

			Log2(APP_LOG_ERROR, "open file is  +++++++++++++++++ %s",dwDesireds[kk]);
			//测试单进程一个范围加锁
			lock_case1(pr1, opena, g_liov_1,sizeof(g_liov_1)/sizeof(lockiov));
			Log1(APP_LOG_ERROR, "lock_case1 end");
			//测试单个进程，多个范围加锁
			lock_case2(pr1, opena, g_liov_1,sizeof(g_liov_1)/sizeof(lockiov));
			Log1(APP_LOG_ERROR, "lock_case2 end");
			lock_case3(pr1, pr2, opena, g_liov_1,sizeof(g_liov_1)/sizeof(lockiov));
			Log1(APP_LOG_ERROR, "lock_case3 end");
			lock_case4(pr1, pr2, opena, g_liov_1,sizeof(g_liov_1)/sizeof(lockiov));
			Log1(APP_LOG_ERROR, "lock_case4 end");
//			lock_case5(pr1, pr2, pr3, pr4, opena, g_liov_1, sizeof(g_liov_1)/sizeof(lockiov));
//			Log1(APP_LOG_ERROR, "lock_case5 end");
			lock_case6(pr1, pr2, opena, g_liov_1,sizeof(g_liov_1)/sizeof(lockiov));
			Log1(APP_LOG_ERROR, "lock_case6_1 end");
//			lock_case6(pr1, pr3, opena, g_liov_1,sizeof(g_liov_1)/sizeof(lockiov));
			Log1(APP_LOG_ERROR, "lock_case6_2 end");
			lock_case7(pr1, pr2, opena, g_liov_1,sizeof(g_liov_1)/sizeof(lockiov));
			Log1(APP_LOG_ERROR, "lock_case7_1 end");
//			lock_case7(pr1, pr3, opena, g_liov_1,sizeof(g_liov_1)/sizeof(lockiov));
			Log1(APP_LOG_ERROR, "lock_case7_2 end");
			lock_case8(pr1, pr2, opena, g_liov_1,sizeof(g_liov_1)/sizeof(lockiov));
			Log1(APP_LOG_ERROR, "lock_case8 end");
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

	pr1.zclose(conn_type);
	pr2.zclose(conn_type);
	pr3.zclose(conn_type);
//	pr4.zclose(conn_type);
	zmq_term(g_context);
	logClose();
	
	return 0;
}

////////delegation test
bool TestDelegationRes_forGetDelgation_succ(enfs_fcb_delegation *buff)
{
	int i = 0;

	printf("start test delegation TestDelegationRes_forGetDelgation_succ \n");

	for (i=0;i<20;i++) {
		if (0!=buff->state_id.other[i]) {
			return TRUE;
		}
	}
	
	return FALSE;
}

bool TestDelegationRes_forGetDelgation_fail(enfs_fcb_delegation *buff)
{
	return (TestDelegationRes_forGetDelgation_succ(buff)==FALSE);
}

bool TestDelegationRes_forDelgationUnChanged(enfs_fcb_delegation *buff1,enfs_fcb_delegation *buff2)
{
	int i = 0;

	printf("start test delegation processOnceDelegationGetRes \n");

	if (buff1->access!=buff2->access) {
		return FALSE;
	}

	if (buff1->state_id.seqid!=buff2->state_id.seqid) {
		return FALSE;
	}

	for (i=0;i<20;i++) {
		if (buff1->state_id.other[i]!=buff2->state_id.other[i]) {
			return FALSE;
		}
	}

	return TRUE;
}
bool TestDelegationRes_forDelgationChanged(enfs_fcb_delegation *buff1,enfs_fcb_delegation *buff2)
{
	return (TestDelegationRes_forDelgationUnChanged(buff1,buff2)==FALSE);
}

char *getDelegationTestResArray(char testCase)
{
	char *pResArray=NULL;

	if (1==testCase) {
		pResArray=gDelegationTestCase1_res;
	}

	if (2==testCase) {
		pResArray=gDelegationTestCase2_res;
	}

	if (3==testCase) {
		pResArray=gDelegationTestCase3_res;
	}

	return pResArray;
}
void MyOutPutDebugStr(char*str,int par1=0,int par2=0,int par3=0,int par4=0)
{
	char buff[512] = "\0";

	memset(buff,0,512);
	sprintf(buff,"%s,%d,%d,%d,%d",str,par1,par2,par3,par4);
	Log2(APP_LOG_ERROR,"%s",buff);
}
void outPutLog(bool testPass,int testCase,int optionIndex,enfs_fcb_delegation *buff1,enfs_fcb_delegation *buff2,char failOpen=0)
{
	char *pResArray=getDelegationTestResArray(testCase);
	char TCase[3];
	char pass[3];
	char rw1[20];
	char rw2[20];
	char close[10];
	char delay[10];
	char yuqi[128];
	char more_infor[512];
	char more_infor1[256];
	char more_infor2[256];
	memset(TCase,0,sizeof(TCase));
	memset(pass,0,sizeof(pass));
	memset(rw1,0,sizeof(rw1));
	memset(rw2,0,sizeof(rw2));
	memset(close,0,sizeof(close));
	memset(delay,0,sizeof(delay));
	memset(yuqi,0,sizeof(yuqi));
	memset(more_infor,0,sizeof(more_infor));
	memset(more_infor1,0,sizeof(more_infor1));
	memset(more_infor2,0,sizeof(more_infor2));
	sprintf(pass,"%d",testPass);
	sprintf(TCase,"%d",testCase);

	if (g_delegation_testoption[optionIndex].rw1 == (O_CREAT|O_RDONLY)) {
		sprintf(rw1,"%s","O_RDONLY");
	}
	else {
		sprintf(rw1,"%s","O_WRONLY");
	}

	if (g_delegation_testoption[optionIndex].rw2 == (O_CREAT|O_RDONLY)) {
		sprintf(rw2,"%s","O_RDONLY");
	}
	else {
		sprintf(rw2,"%s","O_WRONLY");
	}

	sprintf(close,"%d",g_delegation_testoption[optionIndex].close);
	sprintf(delay,"%d",g_delegation_testoption[optionIndex].delayTime);
	switch (pResArray[optionIndex]) {
	case NO_DELEGATION:
		sprintf(yuqi,"%s","NO_DELEGATION");
		break;
	case GET_DELEGATION:
		sprintf(yuqi,"%s","GET_DELEGATION");
		break;
	case DELEGATION_CHANGED:
		sprintf(yuqi,"%s","DELEGATION_CHANGED");
		break;
	case DELEGATION_UNCHANGED:
		sprintf(yuqi,"%s","DELEGATION_UNCHANGED");
		break;
	}

	if (0==failOpen) {
		sprintf(more_infor1,"pr1:(ed_access:%u,seqid:%u,other:[%llu-%llu-%llu])",
			buff1->access,buff1->state_id.seqid,
			*(UINT64*)&buff1->state_id.other[0],
			*(UINT64*)&buff1->state_id.other[8],
			*(UINT64*)&buff1->state_id.other[16]);
		if (buff2) {
			if (TestDelegationRes_forGetDelgation_succ(buff2)) {
				sprintf(more_infor2,"pr2:(ed_access:%u,seqid:%u,other:[%llu-%llu-%llu])",
					buff2->access,buff2->state_id.seqid,
					*(UINT64*)&buff2->state_id.other[0],
					*(UINT64*)&buff2->state_id.other[8],
					*(UINT64*)&buff2->state_id.other[16]);
			}
			else {
				sprintf(more_infor2,"SecondGet fail,pr1:(ed_access:%u,seqid:%u,other:[%llu-%llu-%llu])",
					buff2->access,buff2->state_id.seqid,
					*(UINT64*)&buff2->state_id.other[0],
					*(UINT64*)&buff2->state_id.other[8],
					*(UINT64*)&buff2->state_id.other[16]);
			}
			sprintf(more_infor,"%s;%s",more_infor1,more_infor2);
		}
		else {
			if (testPass) {
				return;
			}
			sprintf(more_infor,"firstGet fail:%s",more_infor1);
		}
	}
	else {
		if (1==failOpen) {
			sprintf(more_infor,"%s","FIRST OPEN FAIL");
		}
		else {
			sprintf(more_infor,"%s","SECOND OPEN FAIL");
		}
	}

	Log9(APP_LOG_ERROR, "%s %s %s %s %s %s %s %s",TCase,pass,rw1,close,delay,rw2,yuqi,more_infor);
//	Log5(APP_LOG_ERROR, "testCase:optionIndex:close:delay",testCase,optionIndex,g_delegation_testoption[optionIndex].close,g_delegation_testoption[optionIndex].delayTime);
}

bool processOnceDelegationGetRes(enfs_fcb_delegation *buff1,enfs_fcb_delegation *buff2,int testCase,int optionIndex)
{
	bool isTestOK = 0;

	if (NULL==buff2) {
		isTestOK=TestDelegationRes_forGetDelgation_succ(buff1);
	}
	else {
		char *pResArray=getDelegationTestResArray(testCase);
		switch (pResArray[optionIndex]) {
		case NO_DELEGATION:
			isTestOK=TestDelegationRes_forGetDelgation_fail(buff2);
			break;
		case GET_DELEGATION:
			isTestOK=TestDelegationRes_forGetDelgation_succ(buff2);
			break;
		case DELEGATION_CHANGED:
			if (TestDelegationRes_forGetDelgation_succ(buff2)) {
				isTestOK=TestDelegationRes_forDelgationChanged(buff1,buff2);
			}
			else {
				isTestOK=FALSE;
			}

			break;
		case DELEGATION_UNCHANGED:
			isTestOK=TestDelegationRes_forDelgationUnChanged(buff1,buff2);
			break;
		}
	}

	outPutLog(isTestOK,testCase,optionIndex,buff1,buff2);

	return isTestOK;
}

BOOL File_Open_Close(struct openres *openfileR,class proc_remote *pr,BOOL BeOpen,int rw)
{
	BOOL ret=TRUE;
	struct openargs opena={
		"",
		"",
		rw
	};
	strcpy(opena.lpFileName,target);
	strcpy(opena.lpFileName2,target2);
	if (BeOpen){
		pr->openfilex(&opena,openfileR);
		if (!pr->checkopen2()){
			if(pr->g_openr.hFile < 0){
				ret=FALSE;
				Log1(APP_LOG_ERROR,"pr1 open file error");
			}
		}
	} 
	else{
		pr->closefilex(openfileR);
	}
	
	return ret;
}

BOOL openFileStep(BOOL firstOpen,int optionIndex,class proc_remote *pr,struct openres *openfileR)
{
	printf("start test delegation  openFileStep\n");
	if (firstOpen)
		return File_Open_Close(openfileR,pr,TRUE,g_delegation_testoption[optionIndex].rw1);
	else
		return File_Open_Close(openfileR,pr,TRUE,g_delegation_testoption[optionIndex].rw2);
}

void closeFile(class proc_remote *pr,struct openres *openfileR)
{
	File_Open_Close(openfileR,pr,FALSE,0);
}

void closeFileStep(int optionIndex,class proc_remote *pr,struct openres *openfileR)
{
	printf("start test delegation closeFileStep \n");
	if (g_delegation_testoption[optionIndex].close) {
		closeFile(pr,openfileR);
	}
}

void delaytimeStep(int optionIndex)
{
	if (g_delegation_testoption[optionIndex].delayTime) {
		Log2(APP_LOG_ERROR,"wait second...%d",g_delegation_testoption[optionIndex].delayTime);
		sleep(g_delegation_testoption[optionIndex].delayTime);
	}
}

bool  getDelegationStep(class proc_remote *pr,GET_DELEGATION_CMD cmd,enfs_fcb_delegation *buffer)
{
	pr->sendDelegationCmd(cmd);
//	sleep(2);
	printf("start test delegation getDelegationStep  memcpy() delegation\n");
	memcpy(buffer,&pr->g_DelegationInfor,sizeof(enfs_fcb_delegation));

	return TRUE;
}

void testDeleagtionForOptions(int testCase,class proc_remote *pr1,class proc_remote *pr2)
{
	struct openres openfiler1,openfiler2;
	enfs_fcb_delegation deleinfor1,deleinfor2;
	GET_DELEGATION_CMD cmd1,cmd2;
	int i=0,count=sizeof(g_delegation_testoption)/sizeof(TEST_OPTION);

	for (i=0;i<count;i++) {
		memset(&openfiler1,0,sizeof(openfiler1));
		if (!openFileStep(TRUE,i,pr1,&openfiler1)) {
			//process error
			outPutLog(FALSE,testCase,i,NULL,NULL,1);
			continue;
		}

		cmd1.hFile=openfiler1.hFile;
		printf("start test delegation  testDeleagtionForOptions cmd=%d\n",cmd1.hFile);
		getDelegationStep(pr1,cmd1,&deleinfor1);

		if (processOnceDelegationGetRes(&deleinfor1,NULL,testCase,i)) {
			while (1) {
				closeFileStep(i,pr1,&openfiler1);
				delaytimeStep(i);

				if (1==testCase) {
					memset(&openfiler2,0,sizeof(openfiler2));
					if (openFileStep(FALSE,i,pr1,&openfiler2)) {
						cmd2.hFile=openfiler2.hFile;
						getDelegationStep(pr1,cmd2,&deleinfor2);
					}
					else {
						//process error
						outPutLog(FALSE,testCase,i,NULL,NULL,2);
						break;
					}
				}
				else {
					printf("pr2 open file start\n");
					memset(&openfiler2,0,sizeof(openfiler2));
					if (openFileStep(FALSE,i,pr2,&openfiler2)) {
						cmd2.hFile=openfiler2.hFile;
						getDelegationStep(pr2,cmd2,&deleinfor2);
					}
					else {
						//process error
						printf("pr2 open file false\n");
						outPutLog(FALSE,testCase,i,NULL,NULL,2);
						break;
					}
				}

				processOnceDelegationGetRes(&deleinfor1,&deleinfor2,testCase,i);

				//close file for second open
				if(1==testCase) {
					printf("1==testCase close file\n");
					closeFile(pr1,&openfiler2);
				}
				else {
					closeFile(pr2,&openfiler2);
				}
				break;
			}

			//cose file for first open if need
			if(0==g_delegation_testoption[i].close) {
				closeFile(pr1,&openfiler1);
			}
		}
		else {
			closeFile(pr1,&openfiler1);//close file for first open
		}
	}
}

//one proc
void delegationTestCase1(class proc_remote *pr1)
{
	testDeleagtionForOptions(1,pr1,NULL);
}
//multiple procs or multiple clients
void delegationTestCase2_3(int testCase,class proc_remote *pr1,class proc_remote *pr2)
{
	testDeleagtionForOptions(testCase,pr1,pr2);
}
