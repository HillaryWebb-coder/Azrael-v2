################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/tasks/dateTimeTask.c \
../Src/tasks/settings.c \
../Src/tasks/task_air_race.c \
../Src/tasks/task_pingpong.c 

OBJS += \
./Src/tasks/dateTimeTask.o \
./Src/tasks/settings.o \
./Src/tasks/task_air_race.o \
./Src/tasks/task_pingpong.o 

C_DEPS += \
./Src/tasks/dateTimeTask.d \
./Src/tasks/settings.d \
./Src/tasks/task_air_race.d \
./Src/tasks/task_pingpong.d 


# Each subdirectory must supply rules for building sources it contributes
Src/tasks/%.o Src/tasks/%.su Src/tasks/%.cyclo: ../Src/tasks/%.c Src/tasks/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F411xE -DSTM32 -DSTM32F4 -DSTM32F411CEUx -c -I../Inc -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/CMSIS/Include" -I../STM32F411xE -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/st7735" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/AHT10" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/Nunchuck" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS/include" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS/portable/Common" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS/portable/MemMang" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/FreeRTOS" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/assets" -I"C:/Users/hilla/STM32CubeIDE/Azrael/Azrael v2/Drivers/AT24CXX" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-tasks

clean-Src-2f-tasks:
	-$(RM) ./Src/tasks/dateTimeTask.cyclo ./Src/tasks/dateTimeTask.d ./Src/tasks/dateTimeTask.o ./Src/tasks/dateTimeTask.su ./Src/tasks/settings.cyclo ./Src/tasks/settings.d ./Src/tasks/settings.o ./Src/tasks/settings.su ./Src/tasks/task_air_race.cyclo ./Src/tasks/task_air_race.d ./Src/tasks/task_air_race.o ./Src/tasks/task_air_race.su ./Src/tasks/task_pingpong.cyclo ./Src/tasks/task_pingpong.d ./Src/tasks/task_pingpong.o ./Src/tasks/task_pingpong.su

.PHONY: clean-Src-2f-tasks

