#include <regdef.h>
#define ON 0
#define OS 1
#define OE 2
#define OW 3
.data
ime: .asciiz "TP1 2Q2019: Implement me!"
rotation_rules:	.word	3, 2, 2, 3, 0, 1, 1, 0
allowed_forward: .word 0, 1, 2, 3		# steps
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
	# y calculo indice en el array
	mul	t0, a0, 8	# indice orientation x 4bytes = word
	mul	t1, a1, 4
	add	t0, t0, t1
	la	t2, rotation_rules(t0)

	# return new orientation
	lw	v0, (t2)

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
    #la a0, ime
    #jal doPanic
    subu	sp, sp, 40				#allocate stack frame(SRA=8 LTA=16 ABA=16)
    sw		fp, 36(sp)
    sw		gp, 32(sp)
    sw		a1, 28(sp)				# relevants_bouds[3] = width
    sw		a1, 24(sp)				# relevants_bouds[2] = width
    sw		a2, 20(sp)				# relevants_bouds[1] = height
    sw		a2, 16(sp)				# relevants_bouds[0] = height
   	sw		a2, 8(sp)
   	sw		a1, 4(sp)
   	sw		a0, 0(sp)
    lw		t3, 8(a0)				# load ant_t->o = index
    sll		t3, t3, 2				# index*4
    addi	t4, sp, 16				# align stack
    add		t4, t3, t4				# base + index*4
    lw		t4, 0(t4)				# = bound
    la		t5, allowed_forward		# base array address
    add		t5, t5, t3				# base + index*4
	lw		t5, 0(t5)				# = step_fun
	bne		t5, ON, step_south		# step_north
	lw		t6, 4(a0)				# ant->y
	addi	t6, t6, -1				# ant->y - 1
	div		t6, t4					# ant->y / height
	mfhi	t6						# remainder moved into t6
	sw		t6, 4(a0)				# &ant->y
	j		done
step_south:
	bne		t5, OS, step_east
	lw		t6, 4(a0)				# ant->y
	addi	t6, t6, 1				# ant->y + 1
	div		t6, t4					# ant->y / height
	mfhi	t6						# remainder moved into t6
	sw		t6, 4(a0)				# &ant->y
	j		done
step_east:
	bne		t5, OE, step_west
	lw		t6, 0(a0)				# ant->x
	addi	t6, t6, 1				# ant->x + 1
	div		t6, t4					# ant->x / width
	mfhi	t6						# remainder moved into t6
	sw		t6, 0(a0)				# &ant->x
	j		done
step_west:
	lw		t6, 0(a0)				# ant->x
	addi	t6, t6, -1				# ant->x - 1
	div		t6, t4					# ant->x / width
	mfhi	t6						# remainder moved into t6
	sw		t6, 0(a0)				# &ant->x
	j		done
done:
    lw		fp, 36(sp)
    lw		gp, 32(sp)
    lw		a2, 8(sp)
   	lw		a1, 4(sp)
   	lw		a0, 0(sp)
    addi	sp, sp, 40
    move	v0, a0
    jr		ra
.end move_forward
#fin
