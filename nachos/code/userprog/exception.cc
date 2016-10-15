// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "console.h"
#include "synch.h"
#include "machine.h"


//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------
static Semaphore *readAvail;
static Semaphore *writeDone;
static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }



void GlobalThreadRun( int temp ) {

  // if (threadToBeDestroyed != NULL) {
  //   delete threadToBeDestroyed;
  //   threadToBeDestroyed = NULL;
  // }

#ifdef USER_PROGRAM
        if (currentThread->space != NULL) {     // if there is an address space
          currentThread->space->RestoreStateOnSwitch();
        }
        currentThread->RestoreUserState();

    #endif
        machine->Run();
      }

      static void ConvertIntToHex (unsigned v, Console *console)
      {
       unsigned x;
       if (v == 0) return;
       ConvertIntToHex (v/16, console);
       x = v % 16;
       if (x < 10) {
        writeDone->P() ;
        console->PutChar('0'+x);
      }
      else {
        writeDone->P() ;
        console->PutChar('a'+x-10);
      }
    }

    void
    ExceptionHandler(ExceptionType which)
    {
      int type = machine->ReadRegister(2);
      int memval, vaddr, printval, tempval, exp;
    unsigned printvalus;        // Used for printing in hex
    if (!initializedConsoleSemaphores) {
     readAvail = new Semaphore("read avail", 0);
     writeDone = new Semaphore("write done", 1);
     initializedConsoleSemaphores = true;
   }
   Console *console = new Console(NULL, NULL, ReadAvail, WriteDone, 0);;

   if ((which == SyscallException) && (type == SYScall_Halt)) {
     DEBUG('a', "Shutdown, initiated by user program.\n");
     interrupt->Halt();
   }
   else if ((which == SyscallException) && (type == SYScall_PrintInt)) {
     printval = machine->ReadRegister(4);
     if (printval == 0) {
       writeDone->P() ;
       console->PutChar('0');
     }
     else {
      if (printval < 0) {
        writeDone->P() ;
        console->PutChar('-');
        printval = -printval;
      }
      tempval = printval;
      exp=1;
      while (tempval != 0) {
       tempval = tempval/10;
       exp = exp*10;
     }
     exp = exp/10;
     while (exp > 0) {
      writeDone->P() ;
      console->PutChar('0'+(printval/exp));
      printval = printval % exp;
      exp = exp/10;
    }
  }
       // Advance program counters.
  machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
  machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
  machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
}
else if ((which == SyscallException) && (type == SYScall_PrintChar)) {
	writeDone->P() ;
        console->PutChar(machine->ReadRegister(4));   // echo it!
       // Advance program counters.
        machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
        machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
        machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
      }
      else if ((which == SyscallException) && (type == SYScall_PrintString)) {
       vaddr = machine->ReadRegister(4);
       machine->ReadMem(vaddr, 1, &memval);
       while ((*(char*)&memval) != '\0') {
         writeDone->P() ;
         console->PutChar(*(char*)&memval);
         vaddr++;
         machine->ReadMem(vaddr, 1, &memval);
       }
       // Advance program counters.
       machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
       machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
       machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
     }
     else if ((which == SyscallException) && (type == SYScall_PrintIntHex)) {
       printvalus = (unsigned)machine->ReadRegister(4);
       writeDone->P() ;
       console->PutChar('0');
       writeDone->P() ;
       console->PutChar('x');
       if (printvalus == 0) {
        writeDone->P() ;
        console->PutChar('0');
      }
      else {
        ConvertIntToHex (printvalus, console);
      }


       // Advance program counters.
      machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
      machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
      machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
    }
    else if((which == SyscallException) && (type == SYScall_GetReg)){
     //Returns the contents of the passed register
     machine->WriteRegister(2,machine->ReadRegister(machine->ReadRegister(4)));
     //Advande program counter
     machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
     machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
     machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
   }
   else if((which == SyscallException) && (type == SYScall_GetPA)){
    //Returns the physical address of the passed virtual address if it is valid
    unsigned virtAddr = machine->ReadRegister(4);
    unsigned vpn = (unsigned) virtAddr / PageSize;
    unsigned offset = (unsigned) virtAddr % PageSize;

    /*Validation Checks:
    1) virtual page number must not exceed the number of page table entries 
    2) Page table entry should hold a valid physical page number
    3) Obtained physical page number should not exceed the total no of physical pages
    These limits are defined in machine.h
    */

    if( vpn > machine->pageTableSize || !machine->NachOSpageTable[vpn].valid || machine->NachOSpageTable[vpn].physicalPage > NumPhysPages)
     machine->WriteRegister(2, -1);
   else
     machine->WriteRegister( 2, machine->NachOSpageTable[vpn].physicalPage*PageSize + offset );

   machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
   machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
   machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
 }

 else if((which == SyscallException) && (type == SYScall_GetPID)){

  //Returns the pid of the current thread defined in thread.h
   machine->WriteRegister(2,currentThread->getpid());
   machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
   machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
   machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
 }

 else if((which == SyscallException) && (type == SYScall_GetPPID)){

   //Returns the ppid of the current thread defined in thread.h
   machine->WriteRegister(2,currentThread->getppid());
   machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
   machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
   machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
 }

 else if((which == SyscallException) && (type == SYScall_Time)){

  //Returns the total time ticks stored as a variable in stats class 
   machine->WriteRegister(2, stats->totalTicks );
   //Advance Program Counters
   machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
   machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
   machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
 }

 else if((which == SyscallException) && (type == SYScall_Sleep)){
  //Advance program counter to read the input 
  machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
  machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
  machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
  int sleepTicks = machine->ReadRegister(4);
  int t = stats->totalTicks + sleepTicks;

  //The current thread just yields the cpu and is put to the end of the Ready Queue without sending it to sleep
  if(sleepTicks == 0)
    currentThread->YieldCPU();
  else
  {
    //SleepQueue is an instance of the list class declared in system.cc
    //It stores all the threads currently in sleep state and their wake up time
    SleepQueue->SortedInsert((void*)currentThread,t);  
    IntStatus prevStatus = interrupt->SetLevel(IntOff);
    currentThread->PutThreadToSleep();
    interrupt->SetLevel(prevStatus);  
  }
}

else if((which == SyscallException) && (type == SYScall_Join)){
  //Advance program counter to read the input
  machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
  machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
  machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
  int pid = machine->ReadRegister(4);

  //status 0 implies that the thread with the passed pid is not a child of the current thread
  if(currentThread->children_Status[pid] == 0)
    machine->WriteRegister(2, -1);
  //status 1 implies that the thread with the passed pid is a terminated(zombie) child of the current thread
  else if(currentThread->children_Status[pid] == 1)
    machine->WriteRegister(2, currentThread->children_Values[pid]);
  //status 2 is used for the active child threads of a process declared in fork system call
  else{
    WaitQueue->SortedInsert((void *)currentThread, pid);
    IntStatus prevStatus = interrupt->SetLevel(IntOff);
    currentThread->PutThreadToSleep();
    interrupt->SetLevel(prevStatus);
  }
}

else if((which == SyscallException) && (type == SYScall_Yield)){

 machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
 machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
 machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
 //The current thread yields the CPU and is put at the tail of the Ready Queue
 currentThread->YieldCPU();
}

else if((which == SyscallException) && (type == SYScall_NumInstr)){

  /*num_of_instructions is an instruction counter defined in thread class 
  /to count the number of executed instructions of the current thread */
  machine->WriteRegister(2, currentThread->num_of_instructions );
  
  machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
  machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
  machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
}


else if( (which == SyscallException) && (type == SYScall_Fork) ){

 machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
 machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
 machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);

 DEBUG('t', "Trying To create a child");

 NachOSThread *t = new NachOSThread("forked thread");    
 t->parent = currentThread ;
 int temp_pid = t->getpid() ;
 currentThread->children_Status[temp_pid] = 2 ;

 // printf("Here by %d\n", currentThread->getpid());

 currentThread->childpointer->SortedInsert((void *)t, temp_pid) ;
 ProcessAddrSpace *space1;   	      	

 space1 = new ProcessAddrSpace(currentThread->space->PagesinVM(),currentThread->space->StartAddress() );
 t->space = space1 ;

 machine->WriteRegister(2, 0);
 t->SaveUserState() ;
 machine->WriteRegister(2,temp_pid);
 t->ThreadFork( GlobalThreadRun, 0); 

}  

else if( (which == SyscallException) && (type == SYScall_Exit) ){    


 currentThread->aliveprocesscount =  currentThread->aliveprocesscount - 1;
 if( currentThread->aliveprocesscount == 0) {
  // currentThread->FinishThread();
  printf("\nNo more process to run, calling halt :)  \n");
  DEBUG('a', "Shutdown, initiated by user program.\n");
  interrupt->Halt();   
}
else{
  int pid = currentThread->getpid(); 
  currentThread->childpointer->killedparent() ;
  if (currentThread->parent == NULL){  
    currentThread->FinishThread();
  }
  else{
    /* invokeParent definrd in list.cc invokes the parent of the current terminating thread
     from sleep if it had been waiting for this thread to complete*/
    WaitQueue->invokeParent(pid); 
    NachOSThread *parent = currentThread->parent ;
    parent->children_Status[pid]= 1 ; 
      parent->children_Values[pid]= machine->ReadRegister(2) ; // *****  SHUBHAM *******     
      parent->childpointer->killedchild(pid);
      currentThread->childpointer->~List() ;
      currentThread->FinishThread();
    }
  } 
}

else  if ( (which == SyscallException) && (type == SYScall_Exec)  ){

  char filename[100];
  int i=0;

  vaddr = machine->ReadRegister(4);
  machine->ReadMem(vaddr, 1, &memval);
  while ((*(char*)&memval) != '\0') {
    filename[i]  = (char)memval;
    ++i;
    vaddr++;
    machine->ReadMem(vaddr, 1, &memval);
  }
  filename[i]  = (char)memval;

  OpenFile *file_var=fileSystem->Open(filename);
  ProcessAddrSpace *space;

  if (file_var == NULL) {
    printf("Unable to open file %s\n", filename);
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
    return;
  }
  int x=currentThread->getpid();
  int y=currentThread->getppid();
  space = new ProcessAddrSpace(file_var); 
  currentThread->space=space;
  currentThread->ppid = y ;
  currentThread->pid = x ;

  machine->WriteRegister(2, currentThread->getpid());
      //assign program counter to starting


  delete file_var;
      space->InitUserCPURegisters();		// set the initial register values
      space->RestoreStateOnSwitch();		// load page table register

      machine->Run();
      // ASSERT(FALSE);
    } 	
    else {
     printf("Unexpected user mode exception %d %d\n", which, type);
     ASSERT(FALSE);
   }
 }
