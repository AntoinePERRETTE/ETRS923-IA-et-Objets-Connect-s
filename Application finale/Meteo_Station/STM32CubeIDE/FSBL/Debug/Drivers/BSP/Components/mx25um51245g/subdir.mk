################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Jerem/Documents/IA\ de\ con/code_cube_ide/ia_test_run/Drivers/BSP/Components/mx25um51245g/mx25um51245g.c 

OBJS += \
./Drivers/BSP/Components/mx25um51245g/mx25um51245g.o 

C_DEPS += \
./Drivers/BSP/Components/mx25um51245g/mx25um51245g.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/mx25um51245g/mx25um51245g.o: C:/Users/Jerem/Documents/IA\ de\ con/code_cube_ide/ia_test_run/Drivers/BSP/Components/mx25um51245g/mx25um51245g.c Drivers/BSP/Components/mx25um51245g/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_SECURE=1 -c -I../../../FSBL/Core/Inc -I../../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../Drivers/CMSIS/Include -I../../../Drivers/BSP/Components/mx25um51245g -I../../../Drivers/BSP/STM32N6xx_Nucleo -I../../../Middlewares/ST/threadx/common/inc -I../../../Middlewares/ST/threadx/ports/cortex_m55/gnu/inc -I../../../Middlewares/ST/STM32_ExtMem_Manager -I../../../Middlewares/ST/STM32_ExtMem_Manager/boot -I../../../Middlewares/ST/STM32_ExtMem_Manager/sal -I../../../Middlewares/ST/STM32_ExtMem_Manager/nor_sfdp -I../../../Middlewares/ST/STM32_ExtMem_Manager/psram -I../../../Middlewares/ST/STM32_ExtMem_Manager/sdcard -I../../../Middlewares/ST/STM32_ExtMem_Manager/user -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"Drivers/BSP/Components/mx25um51245g/mx25um51245g.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components-2f-mx25um51245g

clean-Drivers-2f-BSP-2f-Components-2f-mx25um51245g:
	-$(RM) ./Drivers/BSP/Components/mx25um51245g/mx25um51245g.cyclo ./Drivers/BSP/Components/mx25um51245g/mx25um51245g.d ./Drivers/BSP/Components/mx25um51245g/mx25um51245g.o ./Drivers/BSP/Components/mx25um51245g/mx25um51245g.su

.PHONY: clean-Drivers-2f-BSP-2f-Components-2f-mx25um51245g

