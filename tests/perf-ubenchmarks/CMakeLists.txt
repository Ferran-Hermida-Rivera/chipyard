########################################################################################################################
# file:  CMakeLists.txt
#
# usage: 
#   Edit "VARIABLES"-section to suit project requirements.
#
#   Baremetal Build (Default):
#     cmake -S ./ -B ./build_baremetal/ -D CMAKE_BUILD_TYPE=Debug
#     cmake --build ./build_baremetal/ --target all
#
#   Linux Build:
#     cmake -S ./ -B ./build_linux/ -D CMAKE_BUILD_TYPE=Debug -DBUILD_FOR_LINUX=ON
#     cmake --build ./build_linux/ --target all
#
########################################################################################################################
cmake_minimum_required(VERSION 3.10)

project(chipyard-tests LANGUAGES C CXX)

# <<< NEW >>>
# Add an option to select the build target. Default is OFF (baremetal).
option(BUILD_FOR_LINUX "Build for Linux user-space" OFF)


if(BUILD_FOR_LINUX)
    #################################
    # Linux Toolchain (NEW SECTION)
    #################################
    message(STATUS "Configuring for Linux build")

    set(CMAKE_SYSTEM_NAME       "Linux" CACHE STRING "")
    set(CMAKE_SYSTEM_PROCESSOR  "riscv" CACHE STRING "")

    set(TOOLCHAIN_PREFIX    "riscv64-unknown-linux-gnu-")

    set(CMAKE_AR            "${TOOLCHAIN_PREFIX}ar")
    set(CMAKE_ASM_COMPILER  "${TOOLCHAIN_PREFIX}gcc")
    set(CMAKE_C_COMPILER    "${TOOLCHAIN_PREFIX}gcc")
    set(CMAKE_CXX_COMPILER  "${TOOLCHAIN_PREFIX}g++")
    set(CMAKE_LINKER        "${TOOLCHAIN_PREFIX}ld")
    set(CMAKE_OBJCOPY       "${TOOLCHAIN_PREFIX}objcopy")
    set(CMAKE_OBJDUMP       "${TOOLCHAIN_PREFIX}objdump")
    set(CMAKE_SIZE          "${TOOLCHAIN_PREFIX}size")

    set(CMAKE_EXECUTABLE_SUFFIX ".linux") # Suffix for Linux executables

    #################################
    # Linux Flags (NEW SECTION)
    #################################

    # CPU architecture
    set(ARCH            "rv64imafd_zifencei")
    set(ABI             "lp64d")
    set(CMODEL          "medany")
    set(ARCH_FLAGS      -march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL})

    add_compile_options(${ARCH_FLAGS})
    add_compile_options(-O0 -std=gnu99)
    add_compile_options(-Wall -Wextra)
    # Note: No -march, -mabi, or -mcmodel; the linux-gnu toolchain handles this.
    # The toolchain will automatically define __linux__ for your C code.

    # Link with the real-time library for clock_gettime()
    add_link_options(-lrt)

else()
    #################################
    # RISCV Toolchain (Original)
    #################################
    message(STATUS "Configuring for Baremetal build")

    set(CMAKE_SYSTEM_NAME       "Generic" CACHE STRING "")
    set(CMAKE_SYSTEM_PROCESSOR  "riscv"   CACHE STRING "")

    set(TOOLCHAIN_PREFIX    "riscv64-unknown-elf-")

    set(CMAKE_AR            "${TOOLCHAIN_PREFIX}ar")
    set(CMAKE_ASM_COMPILER  "${TOOLCHAIN_PREFIX}gcc")
    set(CMAKE_C_COMPILER    "${TOOLCHAIN_PREFIX}gcc")
    set(CMAKE_CXX_COMPILER  "${TOOLCHAIN_PREFIX}g++")
    set(CMAKE_LINKER        "${TOOLCHAIN_PREFIX}ld")
    set(CMAKE_OBJCOPY       "${TOOLCHAIN_PREFIX}objcopy")
    set(CMAKE_OBJDUMP       "${TOOLCHAIN_PREFIX}objdump")
    set(CMAKE_SIZE          "${TOOLCHAIN_PREFIX}size")

    set(CMAKE_EXECUTABLE_SUFFIX ".riscv")

    #################################
    # Baremetal Flags (Original)
    #################################

    # CPU architecture
    set(ARCH            "rv64imafd_zifencei")
    set(ABI             "lp64d")
    set(CMODEL          "medany")
    set(ARCH_FLAGS      -march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL})

    # spec
    set(SPECS           "htif_nano.specs")
    set(SPEC_FLAGS      -specs=${SPECS})

    # linker script
    set(LINKER_SCRIPT   "htif.ld")

    add_compile_options(-std=gnu99)
    add_compile_options(-O2 -Wall -Wextra)
    add_compile_options(-fno-common -fno-builtin-printf)
    add_compile_options(${ARCH_FLAGS})
    add_compile_options(${SPEC_FLAGS})

    add_link_options(-static)
    add_link_options(${SPEC_FLAGS})
    add_link_options(-T ${LINKER_SCRIPT})

endif()


# Add target to generate executable
add_executable(prefetcher prefetcher.c)

# Add a target to generate all disassemblies
add_custom_target(dump ALL)

# Function to add disassembly target for an executable
function(add_dump_target target_name)
    add_custom_target(${target_name}-dump
        BYPRODUCTS ${CMAKE_BINARY_DIR}/${target_name}.dump
        COMMAND ${CMAKE_OBJDUMP} -D $<TARGET_FILE:${target_name}> > ${CMAKE_BINARY_DIR}/${target_name}.dump
        DEPENDS ${target_name}
        COMMENT "Generating disassembly for ${target_name}"
    )
    add_dependencies(${target_name}-dump ${target_name})
    add_dependencies(dump ${target_name}-dump)
endfunction()

add_dump_target(prefetcher)


#################################
# spiflash.img target (Unchanged)
#################################

# Add custom command to generate spiflash.img from spiflash.py
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/../spiflash.img
    COMMAND python3 ${CMAKE_SOURCE_DIR}/../spiflash.py --outfile ${CMAKE_SOURCE_DIR}/../spiflash.img
    DEPENDS ${CMAKE_SOURCE_DIR}/../spiflash.py
    COMMENT "Generating spiflash.img"
)

# Add a target for spiflash.img
add_custom_target(spiflash_img ALL
    DEPENDS ${CMAKE_BINARY_DIR}/../spiflash.img
)
