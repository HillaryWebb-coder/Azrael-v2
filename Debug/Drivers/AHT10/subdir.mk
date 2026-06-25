################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/AHT10/aht10.c 

OBJS += \
./Drivers/AHT10/aht10.o 

C_DEPS += \
./Drivers/AHT10/aht10.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/AHT10/%.o Drivers/AHT10/%.su Drivers/AHT10/%.cyclo: ../Drivers/AHT10/%.c Drivers/AHT10/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F411xE -DSTM32 -DSTM32F4 -DSTM32F411CEUx -c -I../Inc -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/CMSIS/Include" -I../STM32F411xE -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/st7735" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/AHT10" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/Nunchuck" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS/include" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS/portable/Common" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS/portable/MemMang" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/assets" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/AT24CXX" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-AHT10

clean-Drivers-2f-AHT10:
	-$(RM) ./Drivers/AHT10/aht10.cyclo ./Drivers/AHT10/aht10.d ./Drivers/AHT10/aht10.o ./Drivers/AHT10/aht10.su

.PHONY: clean-Drivers-2f-AHT10

