# Pruebas funcionales del subconjunto F/D

Suite independiente: `tst_riscv_fp_functional`. No modifica ni sustituye `tst_riscv`.

## Cobertura y configuración

Se ejecutan tres programas sobre siete modelos (21 funciones de Qt Test):

| Programa | ISA y extensiones habilitadas | Casos por ejecución |
| --- | --- | ---: |
| `instrucciones_F_RV32I.s` | RV32I + F | 33 |
| `instrucciones_F_RV64I.s` | RV64I + F + D | 33 |
| `instrucciones_D_RV64I.s` | RV64I + F + D | 29 |

Modelos: uniciclo y cinco etapas con uno, dos y tres slots, cada uno con
predicción de salto no tomado o salto retardado. En cinco etapas se fija
una instancia segmentada por tipo, con latencias 4 (suma/resta), 7
(multiplicación) y 10 (división). El uniciclo conserva su ejecución habitual.
Las conversiones y demás operaciones utilizan la asignación de unidades
definida por el simulador.

Los casos comparan los bits almacenados con constantes esperadas. Los dos
programas F incluyen sNaN de ambos signos en transferencias, manipulación
del signo y mínimo/máximo con un operando numérico. No comprueban las banderas
de excepción ni el repertorio completo de F/D ni verifican directamente todos
los bits superiores del NaN-boxing. No se cubren todas las configuraciones
posibles de latencias e instancias.

## Resultado

El programa debe solicitar el servicio 93 con `a0=0`. Un valor distinto de
cero indica el caso fallido, identificado también por `gp`. La suite falla
ante errores de ensamblado, servicios inesperados o al superar 100000 ciclos.
Los tres NOP después de cada BNE protegen sus comprobaciones en los modelos
con hasta tres slots de retardo. Los ajustes se guardan en una carpeta temporal
para no cambiar las preferencias del usuario.

## Compilación y ejecución (PowerShell)

Con `RIPES_BUILD_TESTS=ON`, desde la raíz del repositorio:

```powershell
cmake --build .\build\Desktop_Qt_6_11_1_MSVC2022_64bit-Debug --target tst_riscv_fp_functional

$env:Path = "C:\Qt\6.11.1\msvc2022_64\bin;$env:Path"
Set-Location .\build\Desktop_Qt_6_11_1_MSVC2022_64bit-Debug\test

# Listar las 21 combinaciones
.\tst_riscv_fp_functional.exe -functions

# Ejecutar todas
.\tst_riscv_fp_functional.exe

# Ejecutar una combinación concreta
.\tst_riscv_fp_functional.exe test_RV64I_D_5etapas_3slots_retardado
```

El ejecutable no necesita que la aplicación gráfica de Ripes esté abierta.
Los ficheros se copian de los programas propios preparados en `TFG_tests`,
normalizando su codificación a UTF-8 y sus saltos de línea para el ensamblador.
