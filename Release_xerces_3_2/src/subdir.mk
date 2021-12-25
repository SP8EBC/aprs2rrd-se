################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/AprsAsioThread.cpp \
../src/AprsPacket.cpp \
../src/AprsThreadConfig.cpp \
../src/AprsWXData.cpp \
../src/Ax25Decoder.cpp \
../src/ConnectionTimeoutEx.cpp \
../src/DataPresentation.cpp \
../src/DataSourceConfig.cpp \
../src/DiffCalculator.cpp \
../src/HolfuyClient.cpp \
../src/HolfuyClientConfig.cpp \
../src/Locale.cpp \
../src/MySqlConnInterface.cpp \
../src/PlotFileDefinition.cpp \
../src/PressureCalculator.cpp \
../src/ProgramConfig.cpp \
../src/RRDFileDefinition.cpp \
../src/SerialAsioThread.cpp \
../src/SerialConfig.cpp \
../src/SlewRateLimiter.cpp \
../src/Telemetry.cpp \
../src/XMLMemoryHandler.cpp \
../src/ZywiecMeteo.cpp \
../src/main.cpp 

OBJS += \
./src/AprsAsioThread.o \
./src/AprsPacket.o \
./src/AprsThreadConfig.o \
./src/AprsWXData.o \
./src/Ax25Decoder.o \
./src/ConnectionTimeoutEx.o \
./src/DataPresentation.o \
./src/DataSourceConfig.o \
./src/DiffCalculator.o \
./src/HolfuyClient.o \
./src/HolfuyClientConfig.o \
./src/Locale.o \
./src/MySqlConnInterface.o \
./src/PlotFileDefinition.o \
./src/PressureCalculator.o \
./src/ProgramConfig.o \
./src/RRDFileDefinition.o \
./src/SerialAsioThread.o \
./src/SerialConfig.o \
./src/SlewRateLimiter.o \
./src/Telemetry.o \
./src/XMLMemoryHandler.o \
./src/ZywiecMeteo.o \
./src/main.o 

CPP_DEPS += \
./src/AprsAsioThread.d \
./src/AprsPacket.d \
./src/AprsThreadConfig.d \
./src/AprsWXData.d \
./src/Ax25Decoder.d \
./src/ConnectionTimeoutEx.d \
./src/DataPresentation.d \
./src/DataSourceConfig.d \
./src/DiffCalculator.d \
./src/HolfuyClient.d \
./src/HolfuyClientConfig.d \
./src/Locale.d \
./src/MySqlConnInterface.d \
./src/PlotFileDefinition.d \
./src/PressureCalculator.d \
./src/ProgramConfig.d \
./src/RRDFileDefinition.d \
./src/SerialAsioThread.d \
./src/SerialConfig.d \
./src/SlewRateLimiter.d \
./src/Telemetry.d \
./src/XMLMemoryHandler.d \
./src/ZywiecMeteo.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -DUSE_XERCES_3_2 -I/usr/include/mysql++ -I../lib -I/usr/include/xercesc -I/usr/include/mysql -I/usr/include/curl -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


