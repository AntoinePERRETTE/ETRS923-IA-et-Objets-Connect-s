################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/threadx/Core/subdir.mk
-include Middlewares/netxduo/NX\ Core/subdir.mk
-include Middlewares/netxduo/LAN8742\ Phy\ Interface/subdir.mk
-include Middlewares/netxduo/Ethernet\ Interface/subdir.mk
-include Middlewares/netxduo/Addons\ DNS/subdir.mk
-include Middlewares/netxduo/Addons\ DHCP\ Client/subdir.mk
-include Middlewares/ST/AI/Validation/Src/subdir.mk
-include Middlewares/ST/AI/Npu/subdir.mk
-include Middlewares/ST/AI/Misc/Src/subdir.mk
-include Drivers/STM32N6xx_HAL_Driver/subdir.mk
-include Drivers/CMSIS/subdir.mk
-include Drivers/BSP/Components/subdir.mk
-include Application/User/Startup/subdir.mk
-include Application/User/NetXDuo/App/subdir.mk
-include Application/User/Meteo/subdir.mk
-include Application/User/Core/subdir.mk
-include Application/User/AZURE_RTOS/App/subdir.mk
-include Appli/AI/App/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := ia_test_run_20261119_234738_Appli
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
ia_test_run_20261119_234738_Appli.elf \

MAP_FILES += \
ia_test_run_20261119_234738_Appli.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
ia_test_run_20261119_234738_Appli.list \

OBJCOPY_BIN += \
ia_test_run_20261119_234738_Appli.bin \


# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: ia_test_run_20261119_234738_Appli.elf secondary-outputs

# Tool invocations
ia_test_run_20261119_234738_Appli.elf ia_test_run_20261119_234738_Appli.map: $(OBJS) $(USER_OBJS) C:\Users\Jerem\Documents\IA\ de\ con\code_cube_ide\ia_test_run\STM32CubeIDE\Appli\STM32N657X0HXQ_ROMxspi2.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "ia_test_run_20261119_234738_Appli.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m55 -T"C:\Users\Jerem\Documents\IA de con\code_cube_ide\ia_test_run\STM32CubeIDE\Appli\STM32N657X0HXQ_ROMxspi2.ld" --specs=nosys.specs -Wl,-Map="ia_test_run_20261119_234738_Appli.map" -Wl,--gc-sections -static -Wl,--cmse-implib -Wl,--out-implib=./secure_nsclib.o -L../../../Middlewares/ST/AI/Lib --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

ia_test_run_20261119_234738_Appli.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "ia_test_run_20261119_234738_Appli.list"
	@echo 'Finished building: $@'
	@echo ' '

ia_test_run_20261119_234738_Appli.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "ia_test_run_20261119_234738_Appli.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) default.size.stdout ia_test_run_20261119_234738_Appli.bin ia_test_run_20261119_234738_Appli.elf ia_test_run_20261119_234738_Appli.list ia_test_run_20261119_234738_Appli.map
	-@echo ' '

post-build:
	"C:/ST/STM32CubeProgrammer/bin/STM32_SigningTool_CLI.exe" -bin "ia_test_run_20261119_234738_Appli.bin" -nk -of 0x80000000 -t fsbl -o "ia_test_run_20261119_234738_Appli-Trusted.bin" -hv 2.3 -align -dump "ia_test_run_20261119_234738_Appli-Trusted.bin"
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified post-build

-include ../makefile.targets
