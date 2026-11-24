# Datos
.data

.align 4
a: .float 10.0
b: .float 5.0

# Vector x: 100 valores double = 800 bytes
.align 8
x:  .double 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
    .double 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
x1: .double 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
    .double 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
x2: .double 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
    .double 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
x3: .double 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
    .double 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
x4: .double 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
    .double 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0

# Vector y: 100 valores double = 800 bytes
y:  .double 10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0
    .double 10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0
y1: .double 10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0
    .double 10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0
y2: .double 10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0
    .double 10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0
y3: .double 10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0
    .double 10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0
y4: .double 10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0
    .double 10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0

# Vector z: 100 doubles
z: .zero 800

# Código: z[i] = (a * x[i] + y[i]) / b
.text
.globl main

main:
    la      t1, x          # Dirección de x[0]
    la      t2, y          # Dirección de y[0]
    la      t3, z          # Dirección de z[0]
    la      t4, a          # Dirección de a
    la      t5, b          # Dirección de b

    addi    t6, t1, 800    # Dirección posterior al último elemento de x

    flw     f0, 0(t4)      # Cargar a como float
    fcvt.d.s f0, f0        # Convertir a de float a double

    flw     f2, 0(t5)      # Cargar b como float
    fcvt.d.s f2, f2        # Convertir b de float a double

loop:
    fld     f4, 0(t1)      # f4 = x[i]
    fld     f6, 0(t2)      # f6 = y[i]

    fmul.d  f8, f0, f4     # f8 = a * x[i]
    fadd.d  f8, f8, f6     # f8 = a * x[i] + y[i]
    fdiv.d  f8, f8, f2     # f8 = (a * x[i] + y[i]) / b

    fsd     f8, 0(t3)      # z[i] = resultado double completo

    addi    t1, t1, 8      # Avanzar al siguiente x[i]
    addi    t2, t2, 8      # Avanzar al siguiente y[i]
    addi    t3, t3, 8      # Avanzar al siguiente z[i]

    bne     t1, t6, loop
    nop
    nop
    nop


# Comprobar todos los resultados de z.
# Salida: a0=0 si todos son correctos; si falla, a0=posicion (desde 1).
verificar_resultados:
    la t0, z
    li t4, 100          # Elementos pendientes
    li t5, 1             # Posicion del elemento, comenzando por 1
    li t3, 0x4010000000000000 # Bits de 4.0

verificar_elemento:
    ld t2, 0(t0)        # Bits del resultado almacenado
    mv gp, t5
    bne t2, t3, resultado_incorrecto
    nop
    nop
    nop
    addi t0, t0, 8
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
