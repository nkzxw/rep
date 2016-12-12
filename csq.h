// Cancel SAFE API set start
//
// The following APIs are to help ease the pain of writing queue packages that
// handle the cancellation race well. The idea of this set of APIs is to not
// force a single queue data structure but allow the cancel logic to be hidden
// from the drivers. A driver implements a queue and as part of its header
// includes the IO_CSQ structure. In its initialization routine it calls
// IoInitializeCsq. Then in the dispatch routine when the driver wants to
// insert an IRP into the queue it calls IoCsqInsertIrp. When the driver wants
// to remove something from the queue it calls IoCsqRemoveIrp. Note that Insert
// can fail if the IRP was cancelled in the meantime. Remove can also fail if
// the IRP was already cancelled.
//
// There are typically two modes where drivers queue IRPs. These two modes are
// covered by the cancel safe queue API set.
//
// Mode 1:
// One is where the driver queues the IRP and at some later
// point in time dequeues an IRP and issues the IO request.
// For this mode the driver should use IoCsqInsertIrp and IoCsqRemoveNextIrp.
// The driver in this case is expected to pass NULL to the irp context
// parameter in IoInsertIrp.
//
// Mode 2:
// In this the driver queues the IRP, issues the IO request (like issuing a DMA
// request or writing to a register) and when the IO request completes (either
// using a DPC or timer) the driver dequeues the IRP and completes it. For this
// mode the driver should use IoCsqInsertIrp and IoCsqRemoveIrp. In this case
// the driver should allocate an IRP context and pass it in to IoCsqInsertIrp.
// The cancel API code creates an association between the IRP and the context
// and thus ensures that when the time comes to remove the IRP it can ascertain
// correctly.
//
// Note that the cancel API set assumes that the field DriverContext[3] is
// always available for use and that the driver does not use it.
//
#ifndef CSQ_H_
#define CSQ_H_
typedef
NTSTATUS
(*PCSQ_ROUTINE) (
				 IN PIRP StartContext
				 );

struct _MyCSQ{
	IO_CSQ CancelSafeQueue;// CSQ work as header for PendingIrpQueue
	LIST_ENTRY   PendingIrpQueue;// Irps waiting to be processed
	KSPIN_LOCK QueueLock;//  SpinLock to protect access to the queue
	KSEMAPHORE IrpQueueSemaphore;
	PETHREAD ThreadObject;
	BOOLEAN ThreadShouldStop;
	PCSQ_ROUTINE csq_func;
	//PVOID csq_ctx;
};
typedef struct _MyCSQ MyCSQ,*PMyCSQ;
int initcsq(PMyCSQ);
int uninitcsq(PMyCSQ);
void csqenqueue(PMyCSQ, PIRP);
PIRP csqdequeue(IN  PMyCSQ, IN PVOID);
#endif