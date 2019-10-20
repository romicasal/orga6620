#include <regdef.h>
#define ON 0
#define OS 1
#define OE 2
#define OW 3


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
	subu	sp, sp, 24
	sw	ra, 16(sp)
	sw	fp, 12(sp)
	sw	gp, 8(sp)
	move 	fp, sp

	# guardo parametros
	sw	a0, 0(sp)	# orientation
	sw	a1, 4(sp)	# rule
	# cargo parametros en registros
	move	t0, a0
	move	t1, a1
	
	# orientacion?
	
norte:	bne	t0, 0, sur
	bne	t1, zero, norte_derecha	# RL = 0
	addi	t2, zero, 3			# OW = 3
	j	return_value
norte_derecha:
	addi	t2, zero, 2			# OE = 2
	j	return_value
sur:	bne	t0, 1, este
	bne	t1, zero, sur_derecha
	addi	t2, zero, 2
	j	return_value
sur_derecha:
	addi	t2, zero, 3
	j	return_value
este:	bne	t0, 2, oeste
	bne	t1, zero, este_derecha
	addi	t2, zero, 0			# ON = 0
	j	return_value
este_derecha:
	addi	t2, zero, 1			# OS = 1
	j	return_value
			
oeste:	bne	t1, zero, oeste_derecha
	addi	t2, zero, 1
	j	return_value
oeste_derecha:
	addi	t2, zero, 0
	
return_value:
	# return new orientation
	move	v0, t2
			
	#borro stack
	lw	gp, 8(sp)
	lw	fp, 12(sp)
	lw	ra, 16(sp)
	addu	sp, sp, 24
	jr	ra


.end new_orientation

.text
.align 2
.globl move_forward
.ent move_forward
move_forward:
	#    la a0, ime
	#   jal doPanic
	# allocate stack frame(SRA=8 LTA=0 ABA=16)
	subu	sp, sp, 24
	sw		fp, 20(sp)
	sw		gp, 16(sp)
   	sw		a2, 8(sp)
   	sw		a1, 4(sp)
   	sw		a0, 0(sp)
	# load struct ant_t
	lw		t0, 0(a0)			# x
	lw		t1, 4(a0)			# y
	lw		t2, 8(a0)			# o
	# switch statement
	bne		t2, ON, case_south
	# adjust(&ant->y, ant->y - 1, height);
	addi	t1, t1, -1			# ant->y - 1
	div		t1, a2				# ant->y / heigth
	mfhi	t5					# remainder moved into t5
	sw		t5, 4(a0)			# &ant->y
	j		done
case_south:
	bne		t2, OS, case_east
	addi	t1, t1, 1			# ant->y + 1
	div		t1, a2				# ant->y / height
	mfhi	t5					# remainder moved into t5
	sw		t5, 4(a0)			# &ant->y
	j		done
case_east:
	bne		t2, OE, case_west
	addi	t0, t0, 1			# ant->x + 1
	div		t0, a1				# ant->x / width
	mfhi	t5					# remainder moved into t5
	sw		t5, 0(a0)			# &ant->x
	j		done
case_west:
	addi	t0, t0, -1			# ant->x + 1
	div		t0, a1				# ant->x / width
	mfhi	t5					# remainder moved into t5
	sw		t5, 0(a0)			# &ant->x
	j		done
done:
	lw		fp, 20(sp)
	lw		gp, 16(sp)
   	lw		a2, 8(sp)
   	lw		a1, 4(sp)
   	lw		a0, 0(sp)
	addi	sp, sp, 24
	move	v0, a0				# Save the return value in v0
	jr		ra
.end move_forward
