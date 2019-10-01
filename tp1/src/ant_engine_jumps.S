#include <regdef.h>

.data
ime: .asciiz "TP1 2Q2019: Implement me!"

.text
.align 2
.globl new_orientation
.ent new_orientation

new_orientation:
#    la a0, ime
#    jal doPanic
#armo stack
	subu	$sp, $sp, 24
	sw	$ra, 16($sp)
	sw	$fp, 12($sp)
	sw	$gp, 8($sp)
	move 	$fp, $sp

	# guardo parametros
	sw	$a0, 0($sp)	# orientation
	sw	$a1, 4($sp)	# rule
	# cargo parametros en registros
	move	$t0, $a0
	move	$t1, $a1
	
	# orientacion?
	
norte:	bne	$t0, 0, sur
	bne	$t1, $zero, norte_derecha	# RL = 0
	addi	$t2, $zero, 3			# OW = 3
	j	return_value
norte_derecha:
	addi	$t2, $zero, 2			# OE = 2
	j	return_value
sur:	bne	$t0, 1, este
	bne	$t1, $zero, sur_derecha
	addi	$t2, $zero, 2
	j	return_value
sur_derecha:
	addi	$t2, $zero, 3
	j	return_value
este:	bne	$t0, 2, oeste
	bne	$t1, $zero, este_derecha
	addi	$t2, $zero, 0			# ON = 0
	j	return_value
este_derecha:
	addi	$t2, $zero, 1			# OS = 1
	j	return_value
			
oeste:	bne	$t1, $zero, oeste_derecha
	addi	$t2, $zero, 1
	j	return_value
oeste_derecha:
	addi	$t2, $zero, 0
	
return_value:
	# return new orientation
	move	$v0, $t2
			
	#borro stack
	lw	$gp, 8($sp)
	lw	$fp, 12($sp)
	lw	$ra, 16($sp)
	addu	$sp, $sp, 24
	jr	$ra


.end new_orientation

.text
.align 2
.globl move_forward
.ent move_forward
move_forward:
    la a0, ime
    jal doPanic
.end move_forward
