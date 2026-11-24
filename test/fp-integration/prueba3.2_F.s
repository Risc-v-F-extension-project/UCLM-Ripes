# Datos
.data
.align 4
a:  .float 1
b:  .float 2
c:  .float 3
d:  .float 4

va: .float 1,2,3,4,5,6,7,8,9,10
vb: .float 11,12,13,14,15,16,17,18,19,20
vc: .float 21,22,23,24,25,26,27,28,29,30
vd: .float 31,32,33,34,35,36,37,38,39,40
z:  .zero 40              # Diez resultados de precisión simple

# z[i] = a*va[i] + b*vb[i] + c*vc[i] + d*vd[i]
# Valores esperados para los diez elementos de z.
.align 4
resultados_esperados:
    .word 0x43520000 # 210.0
    .word 0x435c0000 # 220.0
    .word 0x43660000 # 230.0
    .word 0x43700000 # 240.0
    .word 0x437a0000 # 250.0
    .word 0x43820000 # 260.0
    .word 0x43870000 # 270.0
    .word 0x438c0000 # 280.0
    .word 0x43910000 # 290.0
    .word 0x43960000 # 300.0

.text
.globl main
main:
la t0, va                # Puntero a va
la t6, vb                # Dirección posterior al último elemento de va
la t1, vb                # Puntero a vb
la t2, vc                # Puntero a vc
la t3, vd                # Puntero a vd
la t4, z                 # Puntero a los resultados

la t5, a
flw f0, 0(t5)            # f0 = a = 1.0
la t5, b
flw f2, 0(t5)            # f2 = b = 2.0
la t5, c
flw f4, 0(t5)            # f4 = c = 3.0
la t5, d
flw f6, 0(t5)            # f6 = d = 4.0

loop:
flw f8, 0(t0)            # f8  = va[i]
flw f10, 0(t1)           # f10 = vb[i]
flw f12, 0(t2)           # f12 = vc[i]
flw f14, 0(t3)           # f14 = vd[i]

fmul.s f8, f8, f0        # f8  = a*va[i]
fmul.s f10, f10, f2      # f10 = b*vb[i]
fmul.s f12, f12, f4      # f12 = c*vc[i]
fmul.s f14, f14, f6      # f14 = d*vd[i]

addi t0, t0, 4           # Avanza los punteros al siguiente elemento
addi t1, t1, 4
addi t2, t2, 4
addi t3, t3, 4
addi t4, t4, 4

fadd.s f16, f8, f10      # f16 = a*va[i] + b*vb[i]
fadd.s f18, f12, f14     # f18 = c*vc[i] + d*vd[i]
fadd.s f20, f16, f18     # f20 = suma de los cuatro productos
fsw f20, -4(t4)          # Guarda z[i]
bne t0, t6, loop         # Repite mientras queden elementos
nop
nop
nop


# Comprobar todos los resultados de z.
# Salida: a0=0 si todos son correctos; si falla, a0=posicion (desde 1).
verificar_resultados:
    la t0, z
    li t4, 10          # Elementos pendientes
    li t5, 1             # Posicion del elemento, comenzando por 1
    la t1, resultados_esperados

verificar_elemento:
    lw t2, 0(t0)        # Bits del resultado almacenado
    lw t3, 0(t1)        # Bits del resultado esperado
    mv gp, t5
    bne t2, t3, resultado_incorrecto
    nop
    nop
    nop
    addi t0, t0, 4
    addi t1, t1, 4
    addi t5, t5, 1
    addi t4, t4, -1
    bne t4, zero, verificar_elemento
    nop
    nop
    nop

resultados_correctos:
    li a0, 0             # Exito
    li a7, 93
    ecall

resultado_incorrecto:
    mv a0, t5            # Fallo: posicion del primer resultado incorrecto
    li a7, 93
    ecall
