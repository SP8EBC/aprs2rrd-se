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
../src/Ax25Decoder.cpp \
../src/ConnectionTimeoutEx.cpp \
../src/DataPresentation.cpp \
../src/DataSourceConfig.cpp \
../src/HolfuyClient.cpp \
../src/HolfuyClientConfig.cpp \
../src/MySqlConnInterface.cpp \
../src/ProgramConfig.cpp \
../src/SerialAsioThread.cpp \
../src/SerialConfig.cpp \
../src/SlewRateLimiter.cpp \
../src/Telemetry.cpp \
../src/XMLMemoryHandler.cpp \
../src/main.cpp 

OBJS += \
./src/AprsAsioThread.o \
./src/AprsPacket.o \
./src/AprsThread.o \
./src/AprsThreadConfig.o \
./src/AprsWXData.o \
./src/Ax25Decoder.o \
./src/ConnectionTimeoutEx.o \
./src/DataPresentation.o \
./src/DataSourceConfig.o \
./src/HolfuyClient.o \
./src/HolfuyClientConfig.o \
./src/MySqlConnInterface.o \
./src/ProgramConfig.o \
./src/SerialAsioThread.o \
./src/SerialConfig.o \
./src/SlewRateLimiter.o \
./src/Telemetry.o \
./src/XMLMemoryHandler.o \
./src/main.o 

CPP_DEPS += \
./src/AprsAsioThread.d \
./src/AprsPacket.d \
./src/AprsThread.d \
./src/AprsThreadConfig.d \
./src/AprsWXData.d \
./src/Ax25Decoder.d \
./src/ConnectionTimeoutEx.d \
./src/DataPresentation.d \
./src/DataSourceConfig.d \
./src/HolfuyClient.d \
./src/HolfuyClientConfig.d \
./src/MySqlConnInterface.d \
./src/ProgramConfig.d \
./src/SerialAsioThread.d \
./src/SerialConfig.d \
./src/SlewRateLimiter.d \
./src/Telemetry.d \
./src/XMLMemoryHandler.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -I/usr/include/mysql++ -I/usr/include/xercesc -I/usr/include/mysql -I/usr/include/curl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


