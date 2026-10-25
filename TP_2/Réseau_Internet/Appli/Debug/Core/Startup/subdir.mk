################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32n657x0hxq.s 

OBJS += \
./Core/Startup/startup_stm32n657x0hxq.o 

S_DEPS += \
./Core/Startup/startup_stm32n657x0hxq.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m55 -g3 -DDEBUG -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_SECURE=1 -c -I../NetXDuo/App -I../NetXDuo/Target -I../Core/Inc -I../AZURE_RTOS/App -I../../Secure_nsclib -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/BSP/Components/lan8742 -I../../Middlewares/ST/netxduo/tsn/inc -I../../Middlewares/ST/netxduo/addons/dhcp -I../../Middlewares/ST/netxduo/common/drivers/ethernet -I../../Middlewares/ST/threadx/common/inc -I../../Middlewares/ST/netxduo/common/inc -I../../Middlewares/ST/netxduo/ports/cortex_m55/gnu/inc -I../../Middlewares/ST/netxduo/addons/dns -I../../Middlewares/ST/threadx/ports/cortex_m55/gnu/inc -I../../Drivers/CMSIS/Include -I"/home/perrettea/Documents/IA_et_Objets_Connectes/ETRS923-IA-et-Objets-Connect-s/TP_2/Réseau_Internet/Appli/Drivers/STMems" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32n657x0hxq.d ./Core/Startup/startup_stm32n657x0hxq.o

.PHONY: clean-Core-2f-Startup

