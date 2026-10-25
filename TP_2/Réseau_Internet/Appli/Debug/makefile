################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include NetXDuo/App/subdir.mk
-include Middlewares/ST/threadx/Core/subdir.mk
-include Middlewares/ST/netxduo/NX\ Core/subdir.mk
-include Middlewares/ST/netxduo/LAN8742\ Phy\ Interface/subdir.mk
-include Middlewares/ST/netxduo/Ethernet\ Interface/subdir.mk
-include Middlewares/ST/netxduo/Addons\ DNS/subdir.mk
-include Middlewares/ST/netxduo/Addons\ DHCP\ Client/subdir.mk
-include Drivers/STMems/subdir.mk
-include Drivers/STM32N6xx_HAL_Driver/subdir.mk
-include Drivers/BSP/Components/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include AZURE_RTOS/App/subdir.mk
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


BUILD_ARTIFACT_NAME := Réseau_Internet_Appli
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Réseau_Internet_Appli.elf \

MAP_FILES += \
Réseau_Internet_Appli.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Réseau_Internet_Appli.list \

OBJCOPY_BIN += \
Réseau_Internet_Appli.bin \


# All Target
all: main-build

# Main-build Target
main-build: Réseau_Internet_Appli.elf secondary-outputs

# Tool invocations
Réseau_Internet_Appli.elf Réseau_Internet_Appli.map: $(OBJS) $(USER_OBJS) /home/perrettea/Documents/IA_et_Objets_Connectes/ETRS923-IA-et-Objets-Connect-s/TP_2/Réseau_Internet/Appli/STM32N657X0HXQ_LRUN.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "Réseau_Internet_Appli.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m55 -T"/home/perrettea/Documents/IA_et_Objets_Connectes/ETRS923-IA-et-Objets-Connect-s/TP_2/Réseau_Internet/Appli/STM32N657X0HXQ_LRUN.ld" --specs=nosys.specs -Wl,-Map="Réseau_Internet_Appli.map" -Wl,--gc-sections -static -Wl,--cmse-implib -Wl,--out-implib=./secure_nsclib.o --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Réseau_Internet_Appli.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Réseau_Internet_Appli.list"
	@echo 'Finished building: $@'
	@echo ' '

Réseau_Internet_Appli.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "Réseau_Internet_Appli.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Réseau_Internet_Appli.bin Réseau_Internet_Appli.elf Réseau_Internet_Appli.list Réseau_Internet_Appli.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
