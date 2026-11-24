cmake_minimum_required(VERSION 3.9)

find_package(Qt6 COMPONENTS Test REQUIRED)

# Point to bundled tests within source directory
set(RISCV32_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/riscv-tests)
set(RISCV64_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/riscv-tests-64)
set(RISCV32_C_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/riscv-tests-c)
set(RISCV64_C_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/riscv-tests-c-64)
add_definitions(-DRISCV32_TEST_DIR="${RISCV32_TEST_DIR}")
add_definitions(-DRISCV64_TEST_DIR="${RISCV64_TEST_DIR}")
add_definitions(-DRISCV32_C_TEST_DIR="${RISCV32_C_TEST_DIR}")
add_definitions(-DRISCV64_C_TEST_DIR="${RISCV64_C_TEST_DIR}")

macro(create_qtest name)
    add_executable(${name} ${name}.cpp programloader.h)
    add_test(${name} ${name})
    target_include_directories (${name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${name} Qt6::Core Qt6::Widgets Qt6::Test)
    target_link_libraries(${name} ripes_lib)
endmacro()

create_qtest(tst_riscv)
create_qtest(tst_assembler)
create_qtest(tst_expreval)
create_qtest(tst_cosimulate)
create_qtest(tst_reverse)
create_qtest(tst_cpu_selection)

# Independent functional suite for the implemented F/D subset.
create_qtest(tst_riscv_fp_functional)
target_compile_definitions(tst_riscv_fp_functional PRIVATE
    RIPES_FP_TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fp-functional")

# Control-flow checks for each of the six adapted five-stage configurations.
create_qtest(tst_riscv_fp_control)
target_compile_definitions(tst_riscv_fp_control PRIVATE
    RIPES_FP_CONTROL_TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fp-control")

# Complete FP programs with their result checks implemented in assembly.
create_qtest(tst_riscv_fp_integration)
target_compile_definitions(tst_riscv_fp_integration PRIVATE
    RIPES_FP_INTEGRATION_TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fp-integration")
