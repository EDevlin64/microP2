################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
G8RTOS/%.obj: ../G8RTOS/%.s $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.2.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/ti/ccs1020/ccs/ccs_base/arm/include" --include_path="C:/Users/Eddie/workspace_v10/Lab2/G8RTOS" --include_path="C:/Users/Eddie/workspace_v10/Lab2/BoardSupportPackage/inc" --include_path="C:/Users/Eddie/workspace_v10/Lab2/BoardSupportPackage/DriverLib" --include_path="C:/ti/ccs1020/ccs/ccs_base/arm/include/CMSIS" --include_path="C:/Users/Eddie/workspace_v10/Lab2" --include_path="C:/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.2.LTS/include" --advice:power=all --define=__MSP432P401R__ --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="G8RTOS/$(basename $(<F)).d_raw" --obj_directory="G8RTOS" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

G8RTOS/%.obj: ../G8RTOS/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.2.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/ti/ccs1020/ccs/ccs_base/arm/include" --include_path="C:/Users/Eddie/workspace_v10/Lab2/G8RTOS" --include_path="C:/Users/Eddie/workspace_v10/Lab2/BoardSupportPackage/inc" --include_path="C:/Users/Eddie/workspace_v10/Lab2/BoardSupportPackage/DriverLib" --include_path="C:/ti/ccs1020/ccs/ccs_base/arm/include/CMSIS" --include_path="C:/Users/Eddie/workspace_v10/Lab2" --include_path="C:/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.2.LTS/include" --advice:power=all --define=__MSP432P401R__ --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="G8RTOS/$(basename $(<F)).d_raw" --obj_directory="G8RTOS" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


