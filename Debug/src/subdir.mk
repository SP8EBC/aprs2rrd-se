################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/AprsPacket.cpp \
../src/AprsThread.cpp \
../src/AprsThreadConfig.cpp \
../src/AprsWXData.cpp \
../src/DataPresence.cpp \
../src/MySqlConnInterface.cpp \
../src/Telemetry.cpp \
../src/main.cpp 

OBJS += \
./src/AprsPacket.o \
./src/AprsThread.o \
./src/AprsThreadConfig.o \
./src/AprsWXData.o \
./src/DataPresence.o \
./src/MySqlConnInterface.o \
./src/Telemetry.o \
./src/main.o 

CPP_DEPS += \
./src/AprsPacket.d \
./src/AprsThread.d \
./src/AprsThreadConfig.d \
./src/AprsWXData.d \
./src/DataPresence.d \
./src/MySqlConnInterface.d \
./src/Telemetry.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/usr/include/mysql++ -I/usr/include/mysql -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


