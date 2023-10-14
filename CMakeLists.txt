cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(PROJECT_NAME Boaty)
set(EXE_NAME ${PROJECT_NAME}.elf)

# CMAKE project name, languages and version
project(${PROJECT_NAME}
    # C and assembly
    LANGUAGES C ASM
    VERSION 1.0
)

add_executable(${EXE_NAME})

# Generated files that we also want to ignore warnings
FILE(GLOB ST_CONFIGURATION_SOURCES
    Core/Src/*.c
)
FILE(GLOB_RECURSE HAL_DRIVERS_SOURCES
    Drivers/*/*.c
)

# User files
FILE(GLOB_RECURSE ALL_USER_FILES
    Core/Src/*/*.c
)

# Add the rest of the files.
target_sources(${EXE_NAME} PRIVATE
    Core/Startup/startup_stm32f446retx.s
    
    "${ST_CONFIGURATION_SOURCES}"
    "${HAL_DRIVERS_SOURCES}"
        
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
set_source_files_properties(${ST_CONFIGURATION_SOURCES} ${HAL_DRIVERS_SOURCES}
    PROPERTIES COMPILE_OPTIONS -Wno-unused-parameter)

target_compile_options(${EXE_NAME} PRIVATE
    -Wall
    -Wextra
    -Wshadow
    -ffunction-sections 
    -fdata-sections
)

target_link_options(${EXE_NAME} PRIVATE
    -T${CMAKE_CURRENT_SOURCE_DIR}/STM32F446RETX_FLASH.ld
    -Wl,--gc-sections,--no-warn-rwx-segment
    -specs=nano.specs
)

target_compile_definitions(${EXE_NAME} PUBLIC
    USE_HAL_DRIVER
    STM32F446xx
)
