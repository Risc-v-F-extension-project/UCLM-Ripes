#los datos
.data
.align 4
a: .float 10
b: .float 5
#vector x
x:  .float 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
x1: .float 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
x2: .float 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
x3: .float 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
x4: .float 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
#vector y
y:  .float 10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10
y1: .float 10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10
y2: .float 10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10
y3: .float 10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10
y4: .float 10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10
#vector z
z:  .zero 400

#el código: z[i]=(a*x[i]+y[i])/b
.text
.globl main
main:
la t1, x           #carga la dir de x[0] en t1
la t2, y           #carga la dir de y[0] en t2
la t3, z           #carga la dir de z[0] en t3

la t4, a           #carga la dir de "a" en t4
la t5, b           #carga la dir de "b" en t5
addi t6,t1,400     #preparamos el contador para detectar el final
flw f0, 0(t4)      #carga "a" en f0 como 10.0 float
flw f2, 0(t5)      #carga "b" en f2 como 5.0 float
loop:
flw f4, 0(t1)      #f4=x[i]              f4 = M[t1] = 1.0 float
flw f6, 0(t2)      #f6=y[i]              f6 = M[t2] = 10.0 float
fmul.s f8,f0,f4    #f8=a*x[i]            f8 = 10.0*1.0  = 10.0
fadd.s f8,f8,f6    #f8=a*x[i]+y[i]       f8 = 10.0+10.0 = 20.0
fdiv.s f8,f8,f2    #f8=(a*x[i]+y[i])/b   f8 = 20.0/5.0 = 4.0
fsw f8, 0(t3)      #z[i]=(a*x[i]+y[i])/b M[t3+0] = f8 = 4.0

addi t1,t1,4       #actualizamos todos los índices
addi t2,t2,4
addi t3,t3,4


#lw a0, -4(t3)
#li a7, 2
#ecall

#li a0, 32
#li a7, 11
#ecall

bne t1, t6, loop   #comprobamos si hemos terminado el bucle
nop
nop
nop


# Comprobar todos los resultados de z.
# Salida: a0=0 si todos son correctos; si falla, a0=posicion (desde 1).
verificar_resultados:
    la t0, z
    li t4, 100          # Elementos pendientes
    li t5, 1             # Posicion del elemento, comenzando por 1
    li t3, 0x40800000 # Bits de 4.0

verificar_elemento:
    lw t2, 0(t0)        # Bits del resultado almacenado
    mv gp, t5
    bne t2, t3, resultado_incorrecto
    nop
    nop
    nop
    addi t0, t0, 4
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
