	.def date

	.ref Modulus255

	.thumb
	.align 2
	.text

date:

	.asmfunc

	PUSH {R2 - R9}
	PUSH {LR}
	MOV R4, R0		; current struct
	MOV R5, R1		; new year struct
	MOV R10, R2		; result

	; R0 passed in * data
	; R1 passed in count

	MOV R6, #0		; current data
	MOV R7, #0		; new year data
	MOV R8, #0		; result data
	MOV R9, #4		; count (there are 4 values to calculate)


; body of function

LOOP:
	LDR R6, [R4]			; load R6 with data from the address in R4
	AND R6, R6, #0x000000FF	; bitmask to isolate first element in array
	LDR R7, [R5]
	AND R7, R7, #0x000000FF	; bitmask to isolate first element in array
	SUB R8, R7, R6			; new year - current = result stored in R8
	STR R8, [R10]


	ADD R4, R4, #1			; increment current data address
	ADD R5, R5, #1			; increment new year data address
	ADD R10, R10, #1
	SUB R9, R9, #1			; decrement count
	CMP R9, #0				; see if count has reached 0
	BNE LOOP


	POP {LR}
	POP {R2 - R9}

	BX LR

	.endasmfunc

	.align
	.end


