# Control hazards: 2 slots - not taken
# Codigo de salida en a0: 0 indica exito; un valor distinto de 0 indica fallo.
# En caso de fallo, a0 recibe de gp el numero del primer caso que ha fallado.
# Los comentarios indican el estado esperado para esta configuracion.

.data
.align 4
four:      .float 4.0
two:       .float 2.0
integer7:  .word 7
fp_result: .word 0

.text
.globl main
main:
    la t0, four           # t0 = direccion de four
    flw f0, 0(t0)         # f0 = 4.0
    flw f1, 4(t0)         # f1 = 2.0

    # Test 1: salto tomado y control de instrucciones posteriores
    li gp, 1              # identificador del test = 1
    li t1, 1              # t1 = 1
    li t2, 1              # t2 = 1; BEQ sera tomado
    li s0, 0              # s0 = 0
    beq t1, t2, taken_1   # tomado
    ori s0, s0, 1         # descartada: s0 conserva su valor
    ori s0, s0, 2         # descartada: s0 conserva su valor
    ori s0, s0, 4         # descartada: s0 conserva su valor
taken_1:
    li t3, 0              # t3 = mascara esperada; s0 debe valer 0
    bne s0, t3, fail      # salta a fail si s0 != 0
    nop
    nop
    nop

    # Test 2: salto no tomado; toda la secuencia debe ejecutarse
    li gp, 2              # identificador del test = 2
    li s1, 0              # s1 = 0
    bne t1, t2, fail      # no tomado porque 1 == 1
    ori s1, s1, 1         # s1 = 1
    ori s1, s1, 2         # s1 = 3
    ori s1, s1, 4         # s1 = 7
    li t3, 7              # t3 = 7
    bne s1, t3, fail      # pasa si s1 = 7
    nop
    nop
    nop

    # Test 3: salto tomado con RAW entero LW/ADDI -> BEQ
    li gp, 3              # identificador del test = 3
    la t4, integer7       # t4 = direccion de integer7
    lw t1, 0(t4)          # t1 = 7
    addi t2, x0, 7        # t2 = 7
    li s2, 0              # s2 = 0
    beq t1, t2, taken_3   # tomado cuando ambos operandos disponibles
    ori s2, s2, 1         # descartada: s2 conserva su valor
    ori s2, s2, 2         # descartada: s2 conserva su valor
    ori s2, s2, 4         # descartada: s2 conserva su valor
taken_3:
    li t3, 0              # t3 = mascara esperada; s2 debe valer 0
    bne s2, t3, fail      # salta a fail si s2 != 0
    nop
    nop
    nop

    # Test 4: salto tomado coincidiendo con RAW FDIV -> FSW
    li gp, 4              # identificador del test = 4
    sw x0, 12(t0)         # fp_result = 0x00000000
    fdiv.s f2, f0, f1     # f2 = 4.0 / 2.0 = 2.0
    li s3, 0              # s3 = 0
    beq t1, t2, taken_4   # tomado; debe dominar el stall del FSW
    fsw f2, 12(t0)        # descartada: fp_result conserva 0x00000000
    ori s3, s3, 1         # descartada: s3 conserva su valor
    ori s3, s3, 2         # descartada: s3 conserva su valor
taken_4:
    lw t5, 12(t0)         # t5 = 0x00000000
    li t6, 0x00000000      # t6 = valor esperado de fp_result
    bne t5, t6, fail      # pasa si la escritura se descarto/conservo bien
    nop
    nop
    nop
    li t3, 0              # t3 = mascara esperada; s3 debe valer 0
    bne s3, t3, fail      # salta a fail si s3 != 0
    nop
    nop
    nop

    # Test 5: dos saltos tomados y dos grupos de slots
    li gp, 5              # identificador del test = 5
    li s4, 0              # s4 = 0
    beq t1, t2, taken_5a  # primer salto tomado
    ori s4, s4, 1         # descartada: s4 conserva su valor
    ori s4, s4, 2         # descartada: s4 conserva su valor
    ori s4, s4, 4         # descartada: s4 conserva su valor
taken_5a:
    beq t1, t2, taken_5b  # segundo salto tomado
    ori s4, s4, 8         # descartada: s4 no cambia
    ori s4, s4, 16        # descartada: s4 no cambia
    ori s4, s4, 32        # descartada: s4 no cambia
taken_5b:
    li t3, 0             # t3 = mascara final esperada; s4 = 0
    bne s4, t3, fail      # salta a fail si sobrevivieron slots incorrectos
    nop
    nop
    nop

pass:
    li a0, 0              # Exito: todas las comprobaciones han pasado.
    li a7, 93
    ecall

fail:
    mv a0, gp             # Fallo: a0 indica el numero del caso fallido.
    li a7, 93
    ecall

