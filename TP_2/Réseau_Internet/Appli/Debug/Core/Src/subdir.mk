################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/app_threadx.c \
../Core/Src/main.c \
../Core/Src/secure_nsc.c \
../Core/Src/sensor_task.c \
../Core/Src/stm32n6xx_hal_msp.c \
../Core/Src/stm32n6xx_hal_timebase_tim.c \
../Core/Src/stm32n6xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32n6xx_s.c 

S_UPPER_SRCS += \
../Core/Src/tx_initialize_low_level.S 

OBJS += \
./Core/Src/app_threadx.o \
./Core/Src/main.o \
./Core/Src/secure_nsc.o \
./Core/Src/sensor_task.o \
./Core/Src/stm32n6xx_hal_msp.o \
./Core/Src/stm32n6xx_hal_timebase_tim.o \
./Core/Src/stm32n6xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32n6xx_s.o \
./Core/Src/tx_initialize_low_level.o 

S_UPPER_DEPS += \
./Core/Src/tx_initialize_low_level.d 

C_DEPS += \
./Core/Src/app_threadx.d \
./Core/Src/main.d \
./Core/Src/secure_nsc.d \
./Core/Src/sensor_task.d \
./Core/Src/stm32n6xx_hal_msp.d \
./Core/Src/stm32n6xx_hal_timebase_tim.d \
./Core/Src/stm32n6xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32n6xx_s.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -DNX_INCLUDE_USER_DEFINE_FILE -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_SECURE=1 -c -I../NetXDuo/App -I../NetXDuo/Target -I../Core/Inc -I../AZURE_RTOS/App -I../../Secure_nsclib -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/BSP/Components/lan8742 -I../../Middlewares/ST/netxduo/tsn/inc -I../../Middlewares/ST/netxduo/addons/dhcp -I../../Middlewares/ST/netxduo/common/drivers/ethernet -I../../Middlewares/ST/threadx/common/inc -I../../Middlewares/ST/netxduo/common/inc -I../../Middlewares/ST/netxduo/ports/cortex_m55/gnu/inc -I../../Middlewares/ST/netxduo/addons/dns -I../../Middlewares/ST/threadx/ports/cortex_m55/gnu/inc -I../../Drivers/CMSIS/Include -I"/home/perrettea/Documents/IA_et_Objets_Connectes/ETRS923-IA-et-Objets-Connect-s/TP_2/Réseau_Internet/Appli/Drivers/STMems" -O0 -ffunction-sections -fdata-sections -Wall -Wextra -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/%.o: ../Core/Src/%.S Core/Src/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m55 -g3 -DDEBUG -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_SECURE=1 -c -I../NetXDuo/App -I../NetXDuo/Target -I../Core/Inc -I../AZURE_RTOS/App -I../../Secure_nsclib -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/BSP/Components/lan8742 -I../../Middlewares/ST/netxduo/tsn/inc -I../../Middlewares/ST/netxduo/addons/dhcp -I../../Middlewares/ST/netxduo/common/drivers/ethernet -I../../Middlewares/ST/threadx/common/inc -I../../Middlewares/ST/netxduo/common/inc -I../../Middlewares/ST/netxduo/ports/cortex_m55/gnu/inc -I../../Middlewares/ST/netxduo/addons/dns -I../../Middlewares/ST/threadx/ports/cortex_m55/gnu/inc -I../../Drivers/CMSIS/Include -I"/home/perrettea/Documents/IA_et_Objets_Connectes/ETRS923-IA-et-Objets-Connect-s/TP_2/Réseau_Internet/Appli/Drivers/STMems" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/app_threadx.cyclo ./Core/Src/app_threadx.d ./Core/Src/app_threadx.o ./Core/Src/app_threadx.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/secure_nsc.cyclo ./Core/Src/secure_nsc.d ./Core/Src/secure_nsc.o ./Core/Src/secure_nsc.su ./Core/Src/sensor_task.cyclo ./Core/Src/sensor_task.d ./Core/Src/sensor_task.o ./Core/Src/sensor_task.su ./Core/Src/stm32n6xx_hal_msp.cyclo ./Core/Src/stm32n6xx_hal_msp.d ./Core/Src/stm32n6xx_hal_msp.o ./Core/Src/stm32n6xx_hal_msp.su ./Core/Src/stm32n6xx_hal_timebase_tim.cyclo ./Core/Src/stm32n6xx_hal_timebase_tim.d ./Core/Src/stm32n6xx_hal_timebase_tim.o ./Core/Src/stm32n6xx_hal_timebase_tim.su ./Core/Src/stm32n6xx_it.cyclo ./Core/Src/stm32n6xx_it.d ./Core/Src/stm32n6xx_it.o ./Core/Src/stm32n6xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32n6xx_s.cyclo ./Core/Src/system_stm32n6xx_s.d ./Core/Src/system_stm32n6xx_s.o ./Core/Src/system_stm32n6xx_s.su ./Core/Src/tx_initialize_low_level.d ./Core/Src/tx_initialize_low_level.o

.PHONY: clean-Core-2f-Src

