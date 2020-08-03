; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB


;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec PROC
    EXPORT   LCD_OutDec
	PUSH{R1-R5, lr}
		MOV R1, R0
		
		MOV R3, #10
		MOV R4, #0
		
loopdec	UDIV R0, R1, R3		; R1%10 in R0
		MUL R0, R0, R3
		SUB R0, R1, R0
		
		ADD R0, #0x30
		
		PUSH {R0, R10}
		ADD R4, #1
		
		UDIV R1, R1, R3		;Update R1/10
		CMP R1, #0
		BNE loopdec

popdec	CMP R4, #0
		BEQ donedec
		SUB R4, #1
		POP {R0, R10}
		BL ST7735_OutChar
		
		B popdec
		
donedec	POP{R1-R5, LR}

    BX LR
    ENDP
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.01, range 0.00 to 9.99
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.00 "
;       R0=3,    then output "0.03 "
;       R0=89,   then output "0.89 "
;       R0=123,  then output "1.23 "
;       R0=999,  then output "9.99 "
;       R0>999,  then output "*.** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix PROC
         EXPORT   LCD_OutFix
	PUSH{R1-R5, LR}
		MOV R1, #999
		CMP R0, R1
		BHI err
		
		MOV R2, R0
		
		MOV R1, #100
		UDIV R3, R2, R1		; in/100 (Output 100s place)
		ADD R0, R3, #0x30
		PUSH {R2, R3}
		BL ST7735_OutChar
		POP{R2,R3}
		 
		MOV R1, #100
		MUL R3, R3, R1		; in= in%100
		SUB R2, R2, R3
		
		MOV R0, #0x2E
		PUSH{R2,R3}
		BL ST7735_OutChar
		POP{R2,R3}
		
		MOV R1, #10
		UDIV R3, R2, R1		; in/10 (10s place)
		ADD R0, R3, #0x30
		PUSH{R2,R3}
		BL ST7735_OutChar
		POP{R2,R3}
		
		MOV R1, #10
		MUL R3, R3, R1		; in= in%10
		SUB R2, R2, R3
		ADD R0, R2, #0x30
		PUSH{R2,R3}
		BL ST7735_OutChar
		POP{R2,R3}
		
		B fixdone
		
	
err		MOV R0, #0x2A
		BL ST7735_OutChar
		MOV R0, #0x2E
		BL ST7735_OutChar
		MOV R0, #0x2A
		BL ST7735_OutChar
		MOV R0, #0x2A
		BL ST7735_OutChar
		
fixdone	POP {R1-R5, LR}

         BX LR
         ENDP

         ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file
