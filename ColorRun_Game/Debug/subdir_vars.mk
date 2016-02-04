################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../EK_TM4C123GXL.cmd 

CFG_SRCS += \
../empty.cfg 

LIB_SRCS += \
E:/PRO/TivaWare_C_Series-1.1/grlib/ccs/Debug/grlib.lib \
E:/PRO/TivaWare_C_Series-1.1/usblib/ccs/Debug/usblib.lib 

C_SRCS += \
../2048.c \
../EK_TM4C123GXL.c \
../Kentec320x240x16_ssd2119_8bit.c \
../UARTUtils.c \
../USBCDCD_LoggerIdle.c \
../empty.c \
../grimages.c \
../images.c \
../touch.c \
E:/PRO/TivaWare_C_Series-1.1/utils/ustdlib.c 

OBJS += \
./2048.obj \
./EK_TM4C123GXL.obj \
./Kentec320x240x16_ssd2119_8bit.obj \
./UARTUtils.obj \
./USBCDCD_LoggerIdle.obj \
./empty.obj \
./grimages.obj \
./images.obj \
./touch.obj \
./ustdlib.obj 

C_DEPS += \
./2048.pp \
./EK_TM4C123GXL.pp \
./Kentec320x240x16_ssd2119_8bit.pp \
./UARTUtils.pp \
./USBCDCD_LoggerIdle.pp \
./empty.pp \
./grimages.pp \
./images.pp \
./touch.pp \
./ustdlib.pp 

GEN_MISC_DIRS += \
./configPkg/ 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_OPTS += \
./configPkg/compiler.opt 

GEN_FILES += \
./configPkg/linker.cmd \
./configPkg/compiler.opt 

GEN_FILES__QUOTED += \
"configPkg\linker.cmd" \
"configPkg\compiler.opt" 

GEN_MISC_DIRS__QUOTED += \
"configPkg\" 

C_DEPS__QUOTED += \
"2048.pp" \
"EK_TM4C123GXL.pp" \
"Kentec320x240x16_ssd2119_8bit.pp" \
"UARTUtils.pp" \
"USBCDCD_LoggerIdle.pp" \
"empty.pp" \
"grimages.pp" \
"images.pp" \
"touch.pp" \
"ustdlib.pp" 

OBJS__QUOTED += \
"2048.obj" \
"EK_TM4C123GXL.obj" \
"Kentec320x240x16_ssd2119_8bit.obj" \
"UARTUtils.obj" \
"USBCDCD_LoggerIdle.obj" \
"empty.obj" \
"grimages.obj" \
"images.obj" \
"touch.obj" \
"ustdlib.obj" 

C_SRCS__QUOTED += \
"../2048.c" \
"../EK_TM4C123GXL.c" \
"../Kentec320x240x16_ssd2119_8bit.c" \
"../UARTUtils.c" \
"../USBCDCD_LoggerIdle.c" \
"../empty.c" \
"../grimages.c" \
"../images.c" \
"../touch.c" \
"E:/PRO/TivaWare_C_Series-1.1/utils/ustdlib.c" 

GEN_CMDS__FLAG += \
-l"./configPkg/linker.cmd" 

GEN_OPTS__FLAG += \
--cmd_file="./configPkg/compiler.opt" 


