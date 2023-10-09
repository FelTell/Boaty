cmake_minimum_required(VERSION 3.22)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(PROJECT_NAME Aula8ProgEmb)
set(EXE_NAME ${PROJECT_NAME}.elf)
set(OPENOCD_CFG_NAME ${PROJECT_NAME}.cfg)

# CMAKE project name, languages and version
project(${PROJECT_NAME}
    # C and assembly
    LANGUAGES C CXX ASM
    VERSION 1.0
)

# Reduce binary size. Used to fit code in the MCU
option(GARBAGE_COLLECT_SECTIONS "Use -f{function,data}-sections and -Wl,--gc-sections." TRUE)
if (${GARBAGE_COLLECT_SECTIONS})
add_compile_options(-ffunction-sections -fdata-sections)
add_link_options(-Wl,--gc-sections)
endif()

# separate ST HAL Drivers library to ignore warnings from generated files
add_library(ST STATIC)

FILE(GLOB_RECURSE HAL_DRIVERS_SOURCES
    Drivers/*/*.c
)

# Add HAL sources files
target_sources(ST PRIVATE
    "${HAL_DRIVERS_SOURCES}"
)
    
# Add include directory for HAL 
target_include_directories(ST PUBLIC
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32F4xx/Include/
    Drivers/STM32F4xx_HAL_Driver/Inc
    Middlewares/Third_Party/FreeRTOS/Source/include
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
    Core/Inc
)

# Add separate compile commands that ignore some warnings for generated files
target_compile_options(ST PRIVATE 
    -Wno-unused-parameter
    -Wno-shadow
)

target_compile_definitions(ST PUBLIC 
    USE_HAL_DRIVER
    STM32F446xx
)

add_executable(${EXE_NAME})

# Generated files that we also want to ignore warnings
set(ST_CONFIGURATION_SOURCES
    Core/Src/stm32f4xx_hal_msp.c
    Core/Src/stm32f4xx_it.c
    Core/Src/syscalls.c
    Core/Src/sysmem.c
    Core/Src/system_stm32f4xx.c
)

# The correct way to get the sources is placing the path to each file, 
# but this is a repo used by a lot of users, which the minority will use 
# CMake integration, so to avoid having to update this file in every pull
# let's grab all files from the Src folder
FILE(GLOB ALL_USER_FILES
    Core/Src/*.c
    Core/Src/*/*.c
)

# Add the rest of the files.
target_sources(${EXE_NAME} PRIVATE
    "${ST_CONFIGURATION_SOURCES}"
    
    Core/Startup/startup_stm32f446retx.s
    
    Core/Src/main.c
    Core/Src/stm32f4xx_it.c

    "${ALL_USER_FILES}"
)

target_include_directories(${EXE_NAME} PRIVATE
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32F4xx/Include/
    Drivers/STM32F4xx_HAL_Driver/Inc
    Middlewares/Third_Party/FreeRTOS/Source/include
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
    Core/Inc
)

# Ignore warnings in generated files
set_source_files_properties(${ST_CONFIGURATION_SOURCES}
    PROPERTIES COMPILE_OPTIONS -Wno-unused-parameter)

target_compile_options(${EXE_NAME} PRIVATE
    -Wall
    -Wextra
    -Wshadow
)

target_link_options(${EXE_NAME} PRIVATE
    -T${CMAKE_CURRENT_SOURCE_DIR}/STM32F446RETX_FLASH.ld
    -Wl,--gc-sections,--no-warn-rwx-segment
    -specs=nano.specs
)

target_compile_definitions(${EXE_NAME} PUBLIC
    USE_HAL_DRIVER
    STM32H743xx
)

target_link_libraries(${EXE_NAME} PRIVATE
    ST
)

add_custom_target(flash
    openocd -f ${CMAKE_CURRENT_SOURCE_DIR}/${OPENOCD_CFG_NAME} -c "program ${EXE_NAME} reset exit"
    VERBATIM
)
