; G8RTOS_SchedulerASM.s
; Holds all ASM functions needed for the scheduler
; Note: If you have an h file, do not have a C file and an S file of the same name

	; Functions Defined
	.def G8RTOS_Start, PendSV_Handler

	; Dependencies
	.ref CurrentlyRunningThread, G8RTOS_Scheduler

	.thumb		; Set to thumb mode
	.align 2	; Align by 2 bytes (thumb mode uses allignment by 2 or 4)
	.text		; Text section

; Need to have the address defined in file 
; (label needs to be close enough to asm code to be reached with PC relative addressing)
RunningPtr: .field CurrentlyRunningThread, 32


; G8RTOS_Start
;	Sets the first thread to be the currently running thread
;	Starts the currently running thread by setting Link Register to tcb's Program Counter
G8RTOS_Start:

	.asmfunc
									;
	LDR R0, RunningPtr				; point to the initial tcb
	LDR R1, [R0]
	LDR SP, [R1]					; load SP with the initial thread's stack address
	POP {R4-R11}					; POP the registers in the order imitating a context switch
	POP {R0-R3}
	POP {R12}
	ADD SP, SP, #4 					; since we dont want the dummy LR, skip LR in the stack (discard/skip LR from initial stack)
	POP {LR} 						; SP is now pointing to the function pointer PC in the stack - POP LR so we enter thr first thread when we branch
	ADD SP, SP, #4 					; discard/skip PSR
	CPSIE I 						; Enable interrupts at processor level
	BX LR 							; start first thread

	.endasmfunc

; PendSV_Handler
; - Performs a context switch in G8RTOS
; 	- Saves remaining registers into thread stack
;	- Saves current stack pointer to tcb
;	- Calls G8RTOS_Scheduler to get new tcb
;	- Set stack pointer to new stack pointer from new tcb
;	- Pops registers from thread stack
PendSV_Handler:
	
	.asmfunc
									; R0-R3, R12, PC, LR, and PSR get auto pushed
	CPSID I							; prevent interrupt during switch		$$$ can replace with BL StartCritical Section
	PUSH {R4-R11}					; save remaining registers
	LDR R0, RunningPtr 				; R0 = pointer to CurrentlyRunningThread - points to the tcb to be switched (old tcb)
	LDR R1, [R0]					; R1 = RunningPtr (dereference twice bc RunningPtr is a double pointer)
	STR SP, [R1]					; Stores the old thread's SP into its tcb - how this thread knows where its stack is

	PUSH {R0,LR}
	BL G8RTOS_Scheduler				; context switch
	POP {R0,LR}

	LDR R1, [R0]
	LDR SP, [R1]					; load SP with the new thread's stack address

	POP {R4-R11}
	CPSIE I							; reenable interrupts 				$$$ can replace with BL EndCriticalSection
	BX LR							; restores R0-R3, R12, LR, PC, PSR

	.endasmfunc
	
	; end of the asm file
	.align
	.end
