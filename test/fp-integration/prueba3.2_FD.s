# Datos
.data
.align 8

a:  .float 1.0
b:  .float 2.0
c:  .float 3.0
d:  .float 4.0

va: .double 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0
vb: .double 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0
vc: .double 21.0, 22.0, 23.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0
vd: .double 31.0, 32.0, 33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0, 40.0

z:  .zero 80

# Código
# Valores esperados para los diez elementos de z.
.align 8
resultados_esperados:
    .dword 0x406a400000000000 # 210.0
    .dword 0x406b800000000000 # 220.0
    .dword 0x406cc00000000000 # 230.0
    .dword 0x406e000000000000 # 240.0
    .dword 0x406f400000000000 # 250.0
    .dword 0x4070400000000000 # 260.0
    .dword 0x4070e00000000000 # 270.0
    .dword 0x4071800000000000 # 280.0
    .dword 0x4072200000000000 # 290.0
    .dword 0x4072c00000000000 # 300.0

.text
.globl main

main:
    la      t1, va          # Dirección del vector va
    la      s0, vb          # Final de va; equivalente al $t7 de MIPS
    la      t2, vb          # Dirección del vector vb
    la      t3, vc          # Dirección del vector vc
    la      t4, vd          # Dirección del vector vd
    la      t5, z           # Dirección del vector z

    la      t6, a
    flw     f1, 0(t6)       # Carga a en precisión simple
    fcvt.d.s f0, f1         # Convierte a double

    la      t6, b
    flw     f3, 0(t6)       # Carga b en precisión simple
    fcvt.d.s f2, f3         # Convierte b a double

    la      t6, c
    flw     f5, 0(t6)       # Carga c en precisión simple
    fcvt.d.s f4, f5         # Convierte c a double

    la      t6, d
    flw     f7, 0(t6)       # Carga d en precisión simple
    fcvt.d.s f6, f7         # Convierte d a double

loop:
    fld     f8,  0(t1)      # f8  = va[i]
    fld     f10, 0(t2)      # f10 = vb[i]
    fld     f12, 0(t3)      # f12 = vc[i]
    fld     f14, 0(t4)      # f14 = vd[i]

    fmul.d  f8,  f8,  f0    # f8  = a * va[i]
    fmul.d  f10, f10, f2    # f10 = b * vb[i]
    fmul.d  f12, f12, f4    # f12 = c * vc[i]
    fmul.d  f14, f14, f6    # f14 = d * vd[i]

    addi    t1, t1, 8
    addi    t2, t2, 8
    addi    t3, t3, 8
    addi    t4, t4, 8
    addi    t5, t5, 8

    fadd.d  f16, f8,  f10   # f16 = a*va[i] + b*vb[i]
    fadd.d  f18, f12, f14   # f18 = c*vc[i] + d*vd[i]
    fadd.d  f20, f16, f18   # f20 = suma completa

    fsd     f20, -8(t5)     # Almacena los 64 bits de z[i]
    bne     t1, s0, loop
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
    ld t2, 0(t0)        # Bits del resultado almacenado
    ld t3, 0(t1)        # Bits del resultado esperado
    mv gp, t5
    bne t2, t3, resultado_incorrecto
    nop
    nop
    nop
    addi t0, t0, 8
    addi t1, t1, 8
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
