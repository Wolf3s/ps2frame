# 1 "crt0.S"
# 1 "crt0.S"
# 1 "crt0.S"
# 1 "crt0.S"
.set noat
.set noreorder
.global _start

.global FlushCache
.global Exit
.global SignalSema
#.global _start
.global	_exit

.text
	nop
	nop
FlushCache:
	li	$3,100
	syscall
	jr	$31
	nop
Exit:
	li	$3,4
	syscall
	jr	$31
	nop
SignalSema:
	li	$3,66
	syscall
	jr	$31
	nop

	nop
	nop
_start:
   	lui	$2,%hi(_args_ptr)
   	addiu	$2,$2, %lo(_args_ptr)
    	sw	$4,($2)
#Clear bss
zerobss:
	lui	$2,%hi(_fbss)
	lui	$3,%hi(_end)
	addiu	$2,$2,%lo(_fbss)
	addiu	$3,$3,%lo(_end)
loop:
	nop
	nop
	sq	$0,($2)
	sltu	$1,$2,$3
	bne	$1,$0,loop
	addiu	$2,$2,16

#Thread
	lui	$4,%hi(_gp)
	addiu	$4,$4,%lo(_gp)
	lui	$5,%hi(_stack)
	addiu	$5,$5,%lo(_stack)
	lui	$6,%hi(_stack_size)
	addiu	$6,$6,%lo(_stack_size)
	lui	$7,%hi(_args)
	addiu	$7,$7,%lo(_args)
	lui	$8,%hi(_root)
	addiu	$8,$8,%lo(_root)
	move	$28,$4
	addiu	$3,$0,60
	syscall
	move	$29, $2

#Heap
	addiu	$3,$0,61
	lui	$4,%hi(_end)
	addiu	$4,$4,%lo(_end)
	lui	$5,%hi(_heap_size)
	addiu	$5,$5,%lo(_heap_size)
	syscall
	nop

#Cache
	jal	FlushCache
	move	$4,$0

#Jump main
	ei

 	lui	$2,%hi(_args_ptr)
 	addiu	$2,$2,%lo(_args_ptr)
 	lw	$3,($2)
	lui	$2,%hi(_args)
	addiu	$2,$2,%lo(_args)

	lw	$4, ($2)
	jal	main
	addiu	$5, $2, 4
_root:
_exit:
#???
	lui	$2,%hi(_args_ptr)
	addiu	$2,$2,%lo(_args_ptr)
	lw	$3,($2)
	jal	SignalSema
	lw	$4,($3)
#Exit
	addiu	$3,$0,35
	syscall
	nop

#The following is take from 3stars by Duke:

		.globl Init_GS
		.align 4

Init_GS:
                lui     $2,0x1fc8
                lb      $2,-0xae($2)
                li      $3,'E'
                li      $6,2                  # 2=NTSC, 3=PAL
                beql    $2,$3,pal_mode
                addiu   $6,1
pal_mode:
		li	$5,1			# 0=NON, 1=INTERLACE
		li	$7,0			# 1=frame, 2=field


		addu	$17,$5,$0
		addu	$18,$6,$0
		addu	$19,$7,$0
		li	$3,0x12001000
		li	$2,0x00000200
		sd	$2,0($3)	# write 0x200 to 0x12001000 (=GS reset)
		li	$4,0x0000FF00
		ld	$2,0($3)
		dsrl	$2,16
		andi	$2,0xFF
		li	$3,0x71		# PutIMR(kernel)
		syscall
		nop
		andi	$4,$17,1
		andi	$5,$18,0xFF
		andi	$6,$19,1
		li	$3,2		# SetGsCrt(kernel)
		syscall
		jr	$31
		nop


	.bss
	.align	6
_args: .space	256+16*4+4

	.data
_args_ptr:
	.space 4
