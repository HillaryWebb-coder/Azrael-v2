################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/FreeRTOS/portable/GCC/ARM_CM4F/port.c 

OBJS += \
./Drivers/FreeRTOS/portable/GCC/ARM_CM4F/port.o 

C_DEPS += \
./Drivers/FreeRTOS/portable/GCC/ARM_CM4F/port.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/FreeRTOS/portable/GCC/ARM_CM4F/%.o Drivers/FreeRTOS/portable/GCC/ARM_CM4F/%.su Drivers/FreeRTOS/portable/GCC/ARM_CM4F/%.cyclo: ../Drivers/FreeRTOS/portable/GCC/ARM_CM4F/%.c Drivers/FreeRTOS/portable/GCC/ARM_CM4F/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F411xE -DSTM32 -DSTM32F4 -DSTM32F411CEUx -c -I../Inc -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/CMSIS/Include" -I../STM32F411xE -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/st7735" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/AHT10" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/Nunchuck" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS/include" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS/portable/Common" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS/portable/MemMang" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/assets" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/AT24CXX" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-FreeRTOS-2f-portable-2f-GCC-2f-ARM_CM4F

clean-Drivers-2f-FreeRTOS-2f-portable-2f-GCC-2f-ARM_CM4F:
	-$(RM) ./Drivers/FreeRTOS/portable/GCC/ARM_CM4F/port.cyclo ./Drivers/FreeRTOS/portable/GCC/ARM_CM4F/port.d ./Drivers/FreeRTOS/portable/GCC/ARM_CM4F/port.o ./Drivers/FreeRTOS/portable/GCC/ARM_CM4F/port.su

.PHONY: clean-Drivers-2f-FreeRTOS-2f-portable-2f-GCC-2f-ARM_CM4F

