#ifdef __cplusplus
extern "C" 
{
#endif
#include <ntddk.h>
#ifdef __cplusplus
}
#endif

#include "csq.h"
//
// CsampAcquireLock modifies the execution level of the current processor.
// 
// KeAcquireSpinLock raises the execution level to Dispatch Level and stores
// the current execution level in the Irql parameter to be restored at a later
// time.  KeAcqurieSpinLock also requires us to be running at no higher than
// Dispatch level when it is called.
//
// The annotations reflect these changes and requirments.
//
//
//_IRQL_raises_(DISPATCH_LEVEL)
//_IRQL_requires_max_(DISPATCH_LEVEL)
//_Acquires_lock_(CONTAINING_RECORD(Csq,DEVICE_EXTENSION, CancelSafeQueue)->QueueLock)
static VOID CsampAcquireLock(
							 PIO_CSQ Csq,//_Out_ _At_(*Irql, _Post_ _IRQL_saves_)
							 PKIRQL  Irql
							 )
{
	PMyCSQ pcsq;

	pcsq = CONTAINING_RECORD(Csq, MyCSQ, CancelSafeQueue);
	//
	// Suppressing because the address below pcsq is valid since it's
	// part of DEVICE_EXTENSION structure.
	//
	//#pragma prefast(suppress: __WARNING_BUFFER_UNDERFLOW, "Underflow using expression 'pcsq->QueueLock'")
	KeAcquireSpinLock(&pcsq->QueueLock, Irql);
}

//
// CsampReleaseLock modifies the execution level of the current processor.
// 
// KeReleaseSpinLock assumes we already hold the spin lock and are therefore
// running at Dispatch level.  It will use the Irql parameter saved in a
// previous call to KeAcquireSpinLock to return the thread back to it's original
// execution level.
//
// The annotations reflect these changes and requirments.
//

//_IRQL_requires_(DISPATCH_LEVEL)
//_Releases_lock_(CONTAINING_RECORD(Csq,DEVICE_EXTENSION, CancelSafeQueue)->QueueLock)
static VOID CsampReleaseLock(
							 PIO_CSQ Csq,
							 KIRQL   Irql
							 )
{
	PMyCSQ   pcsq;
	pcsq = CONTAINING_RECORD(Csq, MyCSQ, CancelSafeQueue);
	//	//
	//	// Suppressing because the address below pcsq is valid since it's
	//	// part of DEVICE_EXTENSION structure.
	//	//
	//#pragma prefast(suppress: __WARNING_BUFFER_UNDERFLOW, "Underflow using expression 'pcsq->QueueLock'")
	KeReleaseSpinLock(&pcsq->QueueLock, Irql);
}

static VOID CsampCompleteCanceledIrp(
									 PIO_CSQ             pCsq,
									 PIRP                Irp
									 )
{
	UNREFERENCED_PARAMETER(pCsq);
	Irp->IoStatus.Status = STATUS_CANCELLED;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
}
static VOID CsampInsertIrp (
							IN PIO_CSQ   Csq,
							IN PIRP      Irp
							)
{
	PMyCSQ   pcsq;
	pcsq = CONTAINING_RECORD(Csq, MyCSQ, CancelSafeQueue);
	InsertTailList(&pcsq->PendingIrpQueue, &Irp->Tail.Overlay.ListEntry);
}
static VOID CsampRemoveIrp(
						   PIO_CSQ Csq,
						   PIRP    Irp
						   )
{
	UNREFERENCED_PARAMETER(Csq);
	RemoveEntryList(&Irp->Tail.Overlay.ListEntry);
}


static PIRP CsampPeekNextIrp(
							 PIO_CSQ Csq,
							 PIRP    Irp,//start from index
							 PVOID   PeekContext//key
							 )
{
	PMyCSQ      pcsq;
	PIRP                    nextIrp = NULL;
	PLIST_ENTRY             nextEntry;
	PLIST_ENTRY             listHead;
	PIO_STACK_LOCATION     irpStack;

	pcsq = CONTAINING_RECORD(Csq,
		MyCSQ, CancelSafeQueue);
	listHead = &pcsq->PendingIrpQueue;
	//
	// If the IRP is NULL, we will start peeking from the listhead, else
	// we will start from that IRP onwards. This is done under the
	// assumption that new IRPs are always inserted at the tail.
	//
	if (Irp == NULL) {
		nextEntry = listHead->Flink;
	} else {
		nextEntry = Irp->Tail.Overlay.ListEntry.Flink;
	}
	while(nextEntry != listHead) {
		nextIrp = CONTAINING_RECORD(nextEntry, IRP, Tail.Overlay.ListEntry);
		irpStack = IoGetCurrentIrpStackLocation(nextIrp);
		//
		// If context is present, continue until you find a matching one.
		// Else you break out as you got next one.
		//
		if (PeekContext) {
			if (irpStack->FileObject == (PFILE_OBJECT) PeekContext) {
				break;
			}
		} else {
			break;
		}
		nextIrp = NULL;
		nextEntry = nextEntry->Flink;
	}
	////
	//// Check if this is from start packet.
	////
	//if (PeekContext == NULL) {
	//	pcsq->CurrentIrp = nextIrp;
	//}
	return nextIrp;
}

#define DELAY_ONE_MICROSECOND 	(-10)
#define DELAY_ONE_MILLISECOND	(DELAY_ONE_MICROSECOND*1000)

static VOID msleep(LONGLONG msec)
{
	LARGE_INTEGER my_interval;
	my_interval.QuadPart = DELAY_ONE_MILLISECOND;
	my_interval.QuadPart *= msec;
	DbgPrint("Mysleep start");
	KeDelayExecutionThread(KernelMode,0,&my_interval);
	DbgPrint("Mysleep end");
}
/*++
Routine Description:
This is the main thread that removes IRP from the queue
and peforms I/O on it. 
Arguments:
Context     -- pointer to the device object
--*/

static VOID CsampPollingThread(
							   IN PVOID Context
							   )
{
	//PDEVICE_OBJECT DeviceObject = Context;
	PMyCSQ pcsq =  Context;
	PIRP Irp;
	NTSTATUS    Status;
	KeSetPriorityThread(KeGetCurrentThread(), LOW_REALTIME_PRIORITY );
	//
	// Now enter the main IRP-processing loop
	//
	for(;;)
	{    
		// Wait indefinitely for an IRP to appear in the work queue or for
		// the Unload routine to stop the thread. Every successful return
		// from the wait decrements the semaphore count by 1.
		KeWaitForSingleObject(&pcsq->IrpQueueSemaphore,
			Executive,
			KernelMode,
			FALSE,
			NULL );
		DbgPrint("CsampPollingThread got IrpQueueSemaphore.\ncsq=%p,csq_func=%p",
			pcsq, pcsq->csq_func);
		//
		// See if thread was awakened because driver is unloading itself...
		//
		if (pcsq->ThreadShouldStop) {
			PsTerminateSystemThread( STATUS_SUCCESS );
		}
		//
		// Remove a pending IRP from the queue.
		//
		Irp = csqdequeue(pcsq, NULL); //IoCsqRemoveNextIrp(&pcsq->CancelSafeQueue, NULL);
		if (!Irp) {
			DbgPrint("Oops, a queued irp got cancelled");
			continue; // go back to waiting
		}
		Status = pcsq->csq_func(Irp);
		if(Status!=0){
			msleep(1000);
			DbgPrint("CsampPollingThread csqenqueue");
			csqenqueue(pcsq, Irp);
		}
	}
}
int initcsq(PMyCSQ pcsq){
	NTSTATUS status;
	HANDLE threadHandle;
	KeInitializeSpinLock(&pcsq->QueueLock);
	KeInitializeSemaphore(&pcsq->IrpQueueSemaphore, 0, MAXLONG );
	InitializeListHead( &pcsq->PendingIrpQueue );
	IoCsqInitialize( 
		&pcsq->CancelSafeQueue,
		CsampInsertIrp,
		CsampRemoveIrp,
		CsampPeekNextIrp,
		CsampAcquireLock,
		CsampReleaseLock,
		CsampCompleteCanceledIrp );
	pcsq->ThreadShouldStop = FALSE;
	DbgPrint("IoCreateDevice ThreadShouldStop=FALSE");
	status = PsCreateSystemThread(
		&threadHandle,
		(ACCESS_MASK)0,
		NULL,
		(HANDLE) 0,
		NULL,
		CsampPollingThread,
		pcsq);
	DbgPrint("initcsq thread=%p",threadHandle);
	ObReferenceObjectByHandle(threadHandle,
		THREAD_ALL_ACCESS,
		NULL,
		KernelMode,
		&pcsq->ThreadObject,
		NULL );
	return 0;
}
int uninitcsq(PMyCSQ pcsq)
{
	DbgPrint("uninitcsq ThreadShouldStop = TRUE");
	pcsq->ThreadShouldStop = TRUE;
	//
	// Make sure the thread wakes up
	//
	//#pragma prefast(suppress: __WARNING_ERROR, "Passing TRUE as last parameter of KeReleaseSemaphore is just a hint that a wait is next.")
	KeReleaseSemaphore(&pcsq->IrpQueueSemaphore,
		0,  // No priority boost
		1,  // Increment semaphore by 1
		TRUE );// WaitForXxx after this call
	//
	// Wait for the thread to terminate
	//
	KeWaitForSingleObject(pcsq->ThreadObject,
		Executive,
		KernelMode,
		FALSE,
		NULL);
	DbgPrint("uninitcsq Thread stopped");
	ObDereferenceObject(pcsq->ThreadObject);
	return 0;
}

//IoCsqInsertIrp uses the queue's dispatch routines to insert the IRP. The IoCsqInsertIrp routine:
//1.Calls the queue's CsqAcquireLock routine to lock the queue.
//2.Calls the queue's CsqInsertIrp routine to insert the IRP.
//3.Marks the IRP as pending.
//4.Calls the queue's CsqReleaseLock routine to unlock the queue.
void csqenqueue(PMyCSQ pcsq, 
				PIRP Irp)
{
	IoCsqInsertIrp(&pcsq->CancelSafeQueue, 
		Irp,
		NULL);//key for IoCsqRemoveIrp

	KeReleaseSemaphore(&pcsq->IrpQueueSemaphore,
		0,// No priority boost
		1,// Increment semaphore by 1
		FALSE );// No WaitForXxx after this call
}

//IoCsqRemoveNextIrp uses the queue's dispatch routines to remove the IRP. The IoCsqRemoveNextIrp routine:
//1.Calls the queue's CsqAcquireLock routine to lock the queue.
//2.Calls the queue's CsqPeekNextIrp routine to find the next matching IRP in the queue. 
//IoCsqRemoveNextIrp passes the value of the PeekContext parameter as the PeekContext parameter of CsqPeekNextIrp. 
//CsqPeekNextIrp returns a pointer to the next matching IRP, or NULL if there is no matching IRP.
//3.If the return value of CsqPeekNextIrp is non-NULL, IoCsqRemoveNextIrp calls the queue's CsqRemoveIrp routine to remove the IRP.
//4.Calls the queue's CsqReleaseLock routine to unlock the queue.
PIRP csqdequeue(IN  PMyCSQ pcsq,
				IN PVOID PeekContext)
{
	return IoCsqRemoveNextIrp(&pcsq->CancelSafeQueue,
		PeekContext);//key for CsampPeekNextIrp 
}

void csqcleanup(PMyCSQ pcsq, 
				PIRP Irp)//cleanup all pending(cancel) irp related to this Irp's fileobject  
{
	PIO_STACK_LOCATION  irpStack;
	PIRP                pendingIrp;
	irpStack = IoGetCurrentIrpStackLocation(Irp);
	pendingIrp = IoCsqRemoveNextIrp(&pcsq->CancelSafeQueue,
		irpStack->FileObject);//key for CsampPeekNextIrp CsqPeekNextIrp 
	while(pendingIrp) 
	{
		pendingIrp->IoStatus.Information = 0;
		pendingIrp->IoStatus.Status = STATUS_CANCELLED;
		DbgPrint("Cleanup cancelled irp\n");
		IoCompleteRequest(pendingIrp, IO_NO_INCREMENT);
		pendingIrp = IoCsqRemoveNextIrp(&pcsq->CancelSafeQueue, irpStack->FileObject);
	}
}