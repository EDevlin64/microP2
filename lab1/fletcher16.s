	.def fletcher16

	.ref Modulus255

	.thumb
	.align 2
	.text

fletcher16:

	.asmfunc

	PUSH {R2 - R7}
	PUSH {LR}
	MOV R7, R0		; R0 = uint8_t *data	address of data
	MOV R4, R1		; R1 = int count		count

	; R0 passed in * data
	; R1 passed in count
	MOV R3, #0		; data
	MOV R5, #0		; sum1
	MOV R6, #0		; sum2


; body of function

LOOP:
	LDR R3, [R7]			; load R3 with data from the address in R7				$$$ need to pass the DATA not the address
	AND R3, R3, #0x000000FF	; bitmask to isolate first element in array
	ADD R0, R5, R3			; add the data to sum1
	BL Modulus255			; link to modulus function
	MOV R5, R0				; store result back in to R1

	ADD R0, R5, R6			; add sum1 to sum2
	BL Modulus255			; link to modulus function
	MOV R6, R0				; store result back into R2

	ADD R7, R7, #1			; increment the address
	SUB R4, R4, #1			; decrement count
	CMP R4, #0				; see if count has reached 0
	BNE LOOP

	LSL R6, R6, #8			; shift sum2 over by 8
	ORR R0, R6, R5			; logical or sum1 and sum2


	POP {LR}
	POP {R2 - R7}

	BX LR

	.endasmfunc

	.align
	.end


