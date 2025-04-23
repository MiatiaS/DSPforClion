################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Software/fft_disp.c \
../Core/Software/myfft.c 

OBJS += \
./Core/Software/fft_disp.o \
./Core/Software/myfft.o 

C_DEPS += \
./Core/Software/fft_disp.d \
./Core/Software/myfft.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Software/%.o Core/Software/%.su Core/Software/%.cyclo: ../Core/Software/%.c Core/Software/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/ST/ARM/DSP/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Software

clean-Core-2f-Software:
	-$(RM) ./Core/Software/fft_disp.cyclo ./Core/Software/fft_disp.d ./Core/Software/fft_disp.o ./Core/Software/fft_disp.su ./Core/Software/myfft.cyclo ./Core/Software/myfft.d ./Core/Software/myfft.o ./Core/Software/myfft.su

.PHONY: clean-Core-2f-Software

