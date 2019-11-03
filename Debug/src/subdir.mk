################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/AprsAsioThread.cpp \
../src/AprsPacket.cpp \
../src/AprsThread.cpp \
../src/AprsThreadConfig.cpp \
../src/AprsWXData.cpp \
../src/ConnectionTimeoutEx.cpp \
../src/DataPresentation.cpp \
../src/MySqlConnInterface.cpp \
../src/ProgramConfig.cpp \
../src/ProgramConfig_test.cpp \
../src/SerialAsioThread.cpp \
../src/SerialAsioThread_test.cpp \
../src/SerialConfig.cpp \
../src/SlewRateLimiter.cpp \
../src/Telemetry.cpp \
../src/main.cpp 

OBJS += \
./src/AprsAsioThread.o \
./src/AprsPacket.o \
./src/AprsThread.o \
./src/AprsThreadConfig.o \
./src/AprsWXData.o \
./src/ConnectionTimeoutEx.o \
./src/DataPresentation.o \
./src/MySqlConnInterface.o \
./src/ProgramConfig.o \
./src/ProgramConfig_test.o \
./src/SerialAsioThread.o \
./src/SerialAsioThread_test.o \
./src/SerialConfig.o \
./src/SlewRateLimiter.o \
./src/Telemetry.o \
./src/main.o 

CPP_DEPS += \
./src/AprsAsioThread.d \
./src/AprsPacket.d \
./src/AprsThread.d \
./src/AprsThreadConfig.d \
./src/AprsWXData.d \
./src/ConnectionTimeoutEx.d \
./src/DataPresentation.d \
./src/MySqlConnInterface.d \
./src/ProgramConfig.d \
./src/ProgramConfig_test.d \
./src/SerialAsioThread.d \
./src/SerialAsioThread_test.d \
./src/SerialConfig.d \
./src/SlewRateLimiter.d \
./src/Telemetry.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/usr/include/mysql++ -I/usr/include/mysql -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


