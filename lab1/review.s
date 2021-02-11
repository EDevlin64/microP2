	.def sort

	.thumb
	.align 2
	.text

sort:

	.asmfunc

	PUSH {R3 - R9}
	PUSH {LR}
	MOV R7, R0		; R0 = uint8_t *data	address of in table
	MOV R4, R1		; R1 = int count		count
	MOV R8, R2		; out table address

	; R0 passed in * data
	; R1 passed in count
	MOV R3, #0		; element1
	MOV R5, #0		; element2
	MOV R6, #0		;
	MOV R9, #0		;

; body of function

LOOP:
	LDR R3, [R7]			; load R3 with data from the address in R7
	AND R3, R3, #0x000000FF	; bitmask to isolate first element in array
	LDR R3, [R7, #4]

	POP {LR}
	POP {R3 - R9}

	BX LR

	.endasmfunc

	.align
	.end


