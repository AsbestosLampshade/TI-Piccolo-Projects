################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/ti/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./F2802x_GlobalVariableDefs.obj" \
"./f2802x_codestartbranch.obj" \
"./f2802x_cputimers.obj" \
"./f2802x_defaultisr.obj" \
"./f2802x_piectrl.obj" \
"./f2802x_pievect.obj" \
"./f2802x_sysctrl.obj" \
"./f2802x_usdelay.obj" \
"./gpio.obj" \
"./main.obj" \
"../F2802x_Headers_nonBIOS.cmd" \
"../F2802x_generic_flash.cmd" \
$(GEN_CMDS__FLAG) \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
Template.out 

EXE_OUTPUTS__QUOTED += \
"Template.out" 


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "Template.out"

# Tool invocations
Template.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C2000 Linker'
	"D:/ti/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --opt_for_speed=5 -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi -z -m"Template.map" --stack_size=0x300 --warn_sections -i"D:/ti/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/lib" -i"D:/ti/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="Template_linkInfo.xml" --rom_model -o "Template.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)
	-$(RM) "F2802x_GlobalVariableDefs.obj" "f2802x_codestartbranch.obj" "f2802x_cputimers.obj" "f2802x_defaultisr.obj" "f2802x_piectrl.obj" "f2802x_pievect.obj" "f2802x_sysctrl.obj" "f2802x_usdelay.obj" "gpio.obj" "main.obj" 
	-$(RM) "F2802x_GlobalVariableDefs.d" "f2802x_cputimers.d" "f2802x_defaultisr.d" "f2802x_piectrl.d" "f2802x_pievect.d" "f2802x_sysctrl.d" "gpio.d" "main.d" 
	-$(RM) "f2802x_codestartbranch.d" "f2802x_usdelay.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

