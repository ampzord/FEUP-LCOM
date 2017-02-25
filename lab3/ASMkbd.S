/*#include "i8042.h"


.global _ASMkbd

.data
_make_code .string "ASM_Make Code: 0x%X\n"
_break_code .string "ASM_Break Code: 0x%X\n\n"

.lcomm : hasTwoBytes, 4


.text
_ASMkbd:
    			xor %ebx,%ebx					// Try to start with ebx with 0
    			in %KBD_OUT_BUF,%bl				// Read from outbuffer to bl (1byte)
    			cmp %KBD_TWO_BYTE,%bl			// Compare if bl is 0xE0 (two bytes)
    			je TWO_BYTES
    			cmp $1, hasTwoBytes				// Check if our variable hasTwoBytes is 1
				jne OUTPUT						// If hasTwoBytes is 0 means we can print now
				movb $KBD_TWO_BYTE, %bh			// If hasTwoBytes is 1 means we should add to bh to the most sign byte
				movl $0, hasTwoBytes			// Puts hasTwoBytes to 0
				jmp OUTPUT

TWO_BYTES:		movl $1, hasTwoBytes			// Put variable hasTwoBytes at 1
				xor %ebx,%ebx					// try with ebx 0			# Put ebx back to 0
				ret 							// Return with hasTwoBytes variable at 1

OUTPUT:  		push %ebx						// Put ebx in a stack
				cmp $ESC_BREAK, %bl				// Check if bl could be esc break code
				je ESC_FOUND					// If esc key pressed exit

PRINT_MKCODE:	push $make_code					// Put breakcode string in stack
				call _printf
				jmp AuxEnd						// Use printf to display break code

PRINT_BKCODE:	push $break_code				// Put breakcode string in stack
				call _printf
				jmp AuxEnd						// Use printf to display break code

ESC_FOUND:		mov -1, %ebx
				jmp FINALEND

TESTS:			test %PAR_ERR, %al
				je PRINT_BKCODE
				jmp PRINT_MKCODE

AuxEnd:			xor $ebx, $ebx					// Put ebx to 0

FINALEND:
				add $8, %esp					// Template
				ret
*/
