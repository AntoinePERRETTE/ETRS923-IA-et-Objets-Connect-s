################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Jerem/Documents/IA\ de\ con/code_cube_ide/ia_test_run/Appli/NetXDuo/App/app_netxduo.c 

OBJS += \
./Application/User/NetXDuo/App/app_netxduo.o 

C_DEPS += \
./Application/User/NetXDuo/App/app_netxduo.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/NetXDuo/App/app_netxduo.o: C:/Users/Jerem/Documents/IA\ de\ con/code_cube_ide/ia_test_run/Appli/NetXDuo/App/app_netxduo.c Application/User/NetXDuo/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -DHAVE_NETWORK_INFO -DNDEBUG -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -DATON_STAI_API -DPB_C99_STATIC_ASSERT -DNX_INCLUDE_USER_DEFINE_FILE -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_SECURE=1 -c -I../../../Appli/Core/Inc -I../../../Secure_nsclib -I../../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../Drivers/CMSIS/Include -I../../../Middlewares/ST/AI/Inc -I../../../Middlewares/ST/AI/Validation/Inc -I../../../Appli/AI/App -I../../../Middlewares/ST/AI/Npu -I../../../Middlewares/ST/AI/Misc/Inc -I../../../Appli/NetXDuo/App -I../../../Appli/NetXDuo/Target -I../../../Appli/AZURE_RTOS/App -I../../../Drivers/BSP/Components/lan8742 -I../../../Middlewares/ST/netxduo/tsn/inc -I../../../Middlewares/ST/netxduo/addons/dhcp -I../../../Middlewares/ST/netxduo/common/drivers/ethernet -I../../../Middlewares/ST/threadx/common/inc -I../../../Middlewares/ST/netxduo/common/inc -I../../../Middlewares/ST/netxduo/ports/cortex_m55/gnu/inc -I../../../Middlewares/ST/netxduo/addons/dns -I../../../Middlewares/ST/threadx/ports/cortex_m55/gnu/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"Application/User/NetXDuo/App/app_netxduo.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User-2f-NetXDuo-2f-App

clean-Application-2f-User-2f-NetXDuo-2f-App:
	-$(RM) ./Application/User/NetXDuo/App/app_netxduo.cyclo ./Application/User/NetXDuo/App/app_netxduo.d ./Application/User/NetXDuo/App/app_netxduo.o ./Application/User/NetXDuo/App/app_netxduo.su

.PHONY: clean-Application-2f-User-2f-NetXDuo-2f-App

