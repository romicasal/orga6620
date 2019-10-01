#include <regdef.h>

.data
ime: .asciiz "TP1 2Q2019: Implement me!"
rotation_rules:	.word	3, 2, 2, 3, 0, 1, 1, 0

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
	# y calculo indice en el array
	mul	$t0, $a0, 8	# indice orientation x 4bytes = word	
	mul	$t1, $a1, 4
	add	$t0, $t0, $t1
	la	$t2, rotation_rules($t0)

	# return new orientation
	lw	$v0, ($t2)

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
