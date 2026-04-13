################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"D:/ti/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --opt_for_speed=3 --include_path="D:/ti/jivaSc/Template" --include_path="D:/ti/C2000Ware_6_00_01_00/device_support/f2802x/common/include" --include_path="D:/ti/C2000Ware_6_00_01_00/device_support/f2802x/headers/include" --include_path="D:/ti/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"D:/ti/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --opt_for_speed=3 --include_path="D:/ti/jivaSc/Template" --include_path="D:/ti/C2000Ware_6_00_01_00/device_support/f2802x/common/include" --include_path="D:/ti/C2000Ware_6_00_01_00/device_support/f2802x/headers/include" --include_path="D:/ti/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


