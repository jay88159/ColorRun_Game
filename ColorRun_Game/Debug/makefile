################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := E:/PRO/codecomposer/ccsv6/tools/compiler/ti-cgt-arm_5.2.4

ORDERED_OBJS += \
"./2048.obj" \
"./EK_TM4C123GXL.obj" \
"./Kentec320x240x16_ssd2119_8bit.obj" \
"./UARTUtils.obj" \
"./USBCDCD_LoggerIdle.obj" \
"./empty.obj" \
"./grimages.obj" \
"./images.obj" \
"./touch.obj" \
"./ustdlib.obj" \
"../EK_TM4C123GXL.cmd" \
"E:/PRO/TivaWare_C_Series-1.1/grlib/ccs/Debug/grlib.lib" \
"E:/PRO/TivaWare_C_Series-1.1/usblib/ccs/Debug/usblib.lib" \
$(GEN_CMDS__FLAG) \
-l"libc.a" \
-l"E:/PRO/codecomposer/tirtos_tivac_2_10_01_38/products/TivaWare_C_Series-2.1.0.12573c/driverlib/ccs/Debug/driverlib.lib" \
-l"E:/PRO/codecomposer/tirtos_tivac_2_10_01_38/products/TivaWare_C_Series-2.1.0.12573c/usblib/ccs/Debug/usblib.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
tonightProject2.out \

EXE_OUTPUTS__QUOTED += \
"tonightProject2.out" \

BIN_OUTPUTS += \
tonightProject2.hex \

BIN_OUTPUTS__QUOTED += \
"tonightProject2.hex" \


# All Target
all: tonightProject2.out

# Tool invocations
tonightProject2.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"E:/PRO/codecomposer/ccsv6/tools/compiler/ti-cgt-arm_5.2.4/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=ccs --define=TIVAWARE --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on -z -m"tonightProject2.map" --heap_size=0 --stack_size=512 -i"E:/PRO/codecomposer/ccsv6/tools/compiler/ti-cgt-arm_5.2.4/lib" -i"E:/PRO/codecomposer/ccsv6/tools/compiler/ti-cgt-arm_5.2.4/include" --reread_libs --warn_sections --display_error_number --diag_wrap=off --xml_link_info="tonightProject2_linkInfo.xml" --rom_model -o "tonightProject2.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

tonightProject2.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"E:/PRO/codecomposer/ccsv6/tools/compiler/ti-cgt-arm_5.2.4/bin/armhex"  -o "tonightProject2.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(BIN_OUTPUTS__QUOTED)$(GEN_OPTS__QUOTED)$(GEN_CMDS__QUOTED)
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-$(RM) "2048.pp" "EK_TM4C123GXL.pp" "Kentec320x240x16_ssd2119_8bit.pp" "UARTUtils.pp" "USBCDCD_LoggerIdle.pp" "empty.pp" "grimages.pp" "images.pp" "touch.pp" "ustdlib.pp" 
	-$(RM) "2048.obj" "EK_TM4C123GXL.obj" "Kentec320x240x16_ssd2119_8bit.obj" "UARTUtils.obj" "USBCDCD_LoggerIdle.obj" "empty.obj" "grimages.obj" "images.obj" "touch.obj" "ustdlib.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

