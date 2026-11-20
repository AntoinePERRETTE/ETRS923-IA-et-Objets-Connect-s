################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/User/Meteo/lps22df_reg.c \
../Application/User/Meteo/meteo_app.c \
../Application/User/Meteo/platform.c \
../Application/User/Meteo/sensor_task.c \
../Application/User/Meteo/sht40ad1b_reg.c \
../Application/User/Meteo/stts22h_reg.c \
../Application/User/Meteo/thingspeak.c 

OBJS += \
./Application/User/Meteo/lps22df_reg.o \
./Application/User/Meteo/meteo_app.o \
./Application/User/Meteo/platform.o \
./Application/User/Meteo/sensor_task.o \
./Application/User/Meteo/sht40ad1b_reg.o \
./Application/User/Meteo/stts22h_reg.o \
./Application/User/Meteo/thingspeak.o 

C_DEPS += \
./Application/User/Meteo/lps22df_reg.d \
./Application/User/Meteo/meteo_app.d \
./Application/User/Meteo/platform.d \
./Application/User/Meteo/sensor_task.d \
./Application/User/Meteo/sht40ad1b_reg.d \
./Application/User/Meteo/stts22h_reg.d \
./Application/User/Meteo/thingspeak.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/Meteo/%.o Application/User/Meteo/%.su Application/User/Meteo/%.cyclo: ../Application/User/Meteo/%.c Application/User/Meteo/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -DHAVE_NETWORK_INFO -DNDEBUG -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_EB_DBG_INFO -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -DATON_STAI_API -DPB_C99_STATIC_ASSERT -DNX_INCLUDE_USER_DEFINE_FILE -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_SECURE=1 -c -I../../../Appli/Core/Inc -I../../../Secure_nsclib -I../../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../Drivers/CMSIS/Include -I../../../Middlewares/ST/AI/Inc -I../../../Middlewares/ST/AI/Validation/Inc -I../../../Appli/AI/App -I../../../Middlewares/ST/AI/Npu -I../../../Middlewares/ST/AI/Misc/Inc -I../../../Appli/NetXDuo/App -I../../../Appli/NetXDuo/Target -I../../../Appli/AZURE_RTOS/App -I../../../Drivers/BSP/Components/lan8742 -I../../../Middlewares/ST/netxduo/tsn/inc -I../../../Middlewares/ST/netxduo/addons/dhcp -I../../../Middlewares/ST/netxduo/common/drivers/ethernet -I../../../Middlewares/ST/threadx/common/inc -I../../../Middlewares/ST/netxduo/common/inc -I../../../Middlewares/ST/netxduo/ports/cortex_m55/gnu/inc -I../../../Middlewares/ST/netxduo/addons/dns -I../../../Middlewares/ST/threadx/ports/cortex_m55/gnu/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User-2f-Meteo

clean-Application-2f-User-2f-Meteo:
	-$(RM) ./Application/User/Meteo/lps22df_reg.cyclo ./Application/User/Meteo/lps22df_reg.d ./Application/User/Meteo/lps22df_reg.o ./Application/User/Meteo/lps22df_reg.su ./Application/User/Meteo/meteo_app.cyclo ./Application/User/Meteo/meteo_app.d ./Application/User/Meteo/meteo_app.o ./Application/User/Meteo/meteo_app.su ./Application/User/Meteo/platform.cyclo ./Application/User/Meteo/platform.d ./Application/User/Meteo/platform.o ./Application/User/Meteo/platform.su ./Application/User/Meteo/sensor_task.cyclo ./Application/User/Meteo/sensor_task.d ./Application/User/Meteo/sensor_task.o ./Application/User/Meteo/sensor_task.su ./Application/User/Meteo/sht40ad1b_reg.cyclo ./Application/User/Meteo/sht40ad1b_reg.d ./Application/User/Meteo/sht40ad1b_reg.o ./Application/User/Meteo/sht40ad1b_reg.su ./Application/User/Meteo/stts22h_reg.cyclo ./Application/User/Meteo/stts22h_reg.d ./Application/User/Meteo/stts22h_reg.o ./Application/User/Meteo/stts22h_reg.su ./Application/User/Meteo/thingspeak.cyclo ./Application/User/Meteo/thingspeak.d ./Application/User/Meteo/thingspeak.o ./Application/User/Meteo/thingspeak.su

.PHONY: clean-Application-2f-User-2f-Meteo

