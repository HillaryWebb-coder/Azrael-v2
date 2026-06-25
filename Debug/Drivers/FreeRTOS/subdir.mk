################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/FreeRTOS/croutine.c \
../Drivers/FreeRTOS/event_groups.c \
../Drivers/FreeRTOS/list.c \
../Drivers/FreeRTOS/queue.c \
../Drivers/FreeRTOS/stream_buffer.c \
../Drivers/FreeRTOS/tasks.c \
../Drivers/FreeRTOS/timers.c 

OBJS += \
./Drivers/FreeRTOS/croutine.o \
./Drivers/FreeRTOS/event_groups.o \
./Drivers/FreeRTOS/list.o \
./Drivers/FreeRTOS/queue.o \
./Drivers/FreeRTOS/stream_buffer.o \
./Drivers/FreeRTOS/tasks.o \
./Drivers/FreeRTOS/timers.o 

C_DEPS += \
./Drivers/FreeRTOS/croutine.d \
./Drivers/FreeRTOS/event_groups.d \
./Drivers/FreeRTOS/list.d \
./Drivers/FreeRTOS/queue.d \
./Drivers/FreeRTOS/stream_buffer.d \
./Drivers/FreeRTOS/tasks.d \
./Drivers/FreeRTOS/timers.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/FreeRTOS/%.o Drivers/FreeRTOS/%.su Drivers/FreeRTOS/%.cyclo: ../Drivers/FreeRTOS/%.c Drivers/FreeRTOS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F411xE -DSTM32 -DSTM32F4 -DSTM32F411CEUx -c -I../Inc -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/CMSIS/Include" -I../STM32F411xE -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/st7735" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/AHT10" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/Nunchuck" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS/include" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS/portable/Common" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS/portable/MemMang" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/assets" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/AT24CXX" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-FreeRTOS

clean-Drivers-2f-FreeRTOS:
	-$(RM) ./Drivers/FreeRTOS/croutine.cyclo ./Drivers/FreeRTOS/croutine.d ./Drivers/FreeRTOS/croutine.o ./Drivers/FreeRTOS/croutine.su ./Drivers/FreeRTOS/event_groups.cyclo ./Drivers/FreeRTOS/event_groups.d ./Drivers/FreeRTOS/event_groups.o ./Drivers/FreeRTOS/event_groups.su ./Drivers/FreeRTOS/list.cyclo ./Drivers/FreeRTOS/list.d ./Drivers/FreeRTOS/list.o ./Drivers/FreeRTOS/list.su ./Drivers/FreeRTOS/queue.cyclo ./Drivers/FreeRTOS/queue.d ./Drivers/FreeRTOS/queue.o ./Drivers/FreeRTOS/queue.su ./Drivers/FreeRTOS/stream_buffer.cyclo ./Drivers/FreeRTOS/stream_buffer.d ./Drivers/FreeRTOS/stream_buffer.o ./Drivers/FreeRTOS/stream_buffer.su ./Drivers/FreeRTOS/tasks.cyclo ./Drivers/FreeRTOS/tasks.d ./Drivers/FreeRTOS/tasks.o ./Drivers/FreeRTOS/tasks.su ./Drivers/FreeRTOS/timers.cyclo ./Drivers/FreeRTOS/timers.d ./Drivers/FreeRTOS/timers.o ./Drivers/FreeRTOS/timers.su

.PHONY: clean-Drivers-2f-FreeRTOS

