################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/AmbigiousDataSourceConfig.cpp \
../src/AmbigiousDataSourceConfig_test.cpp \
../src/AprsAsioThread.cpp \
../src/AprsPacket.cpp \
../src/AprsThreadConfig.cpp \
../src/AprsWXData.cpp \
../src/AprsWXDataPositionless.cpp \
../src/AprxLogParser.cpp \
../src/AprxLogParserConfig.cpp \
../src/AprxLogParserStaticStuff.cpp \
../src/Ax25Decoder.cpp \
../src/BannerCreator.cpp \
../src/BannerCreatorConfig.cpp \
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
../src/Thingspeak.cpp \
../src/TimeTools.cpp \
../src/WeatherlinkClient.cpp \
../src/XMLMemoryHandler.cpp \
../src/ZywiecMeteo.cpp \
../src/main.cpp 

CPP_DEPS += \
./src/AmbigiousDataSourceConfig.d \
./src/AmbigiousDataSourceConfig_test.d \
./src/AprsAsioThread.d \
./src/AprsPacket.d \
./src/AprsThreadConfig.d \
./src/AprsWXData.d \
./src/AprsWXDataPositionless.d \
./src/AprxLogParser.d \
./src/AprxLogParserConfig.d \
./src/AprxLogParserStaticStuff.d \
./src/Ax25Decoder.d \
./src/BannerCreator.d \
./src/BannerCreatorConfig.d \
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
./src/Thingspeak.d \
./src/TimeTools.d \
./src/WeatherlinkClient.d \
./src/XMLMemoryHandler.d \
./src/ZywiecMeteo.d \
./src/main.d 

OBJS += \
./src/AmbigiousDataSourceConfig.o \
./src/AmbigiousDataSourceConfig_test.o \
./src/AprsAsioThread.o \
./src/AprsPacket.o \
./src/AprsThreadConfig.o \
./src/AprsWXData.o \
./src/AprsWXDataPositionless.o \
./src/AprxLogParser.o \
./src/AprxLogParserConfig.o \
./src/AprxLogParserStaticStuff.o \
./src/Ax25Decoder.o \
./src/BannerCreator.o \
./src/BannerCreatorConfig.o \
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
./src/Thingspeak.o \
./src/TimeTools.o \
./src/WeatherlinkClient.o \
./src/XMLMemoryHandler.o \
./src/ZywiecMeteo.o \
./src/main.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -DUSE_XERCES_3_2 -DXUBUNTU2404 -DMAGICKCORE_QUANTUM_DEPTH=16 -I/usr/include/mysql++ -I/usr/include/GraphicsMagick/ -I../lib/nlohman_json_3_11_3/include/ -I/usr/include/xercesc -I/usr/include/mysql -I/usr/include/curl -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/AmbigiousDataSourceConfig.d ./src/AmbigiousDataSourceConfig.o ./src/AmbigiousDataSourceConfig_test.d ./src/AmbigiousDataSourceConfig_test.o ./src/AprsAsioThread.d ./src/AprsAsioThread.o ./src/AprsPacket.d ./src/AprsPacket.o ./src/AprsThreadConfig.d ./src/AprsThreadConfig.o ./src/AprsWXData.d ./src/AprsWXData.o ./src/AprsWXDataPositionless.d ./src/AprsWXDataPositionless.o ./src/AprxLogParser.d ./src/AprxLogParser.o ./src/AprxLogParserConfig.d ./src/AprxLogParserConfig.o ./src/AprxLogParserStaticStuff.d ./src/AprxLogParserStaticStuff.o ./src/Ax25Decoder.d ./src/Ax25Decoder.o ./src/BannerCreator.d ./src/BannerCreator.o ./src/BannerCreatorConfig.d ./src/BannerCreatorConfig.o ./src/ConnectionTimeoutEx.d ./src/ConnectionTimeoutEx.o ./src/DataPresentation.d ./src/DataPresentation.o ./src/DataSourceConfig.d ./src/DataSourceConfig.o ./src/DiffCalculator.d ./src/DiffCalculator.o ./src/HolfuyClient.d ./src/HolfuyClient.o ./src/HolfuyClientConfig.d ./src/HolfuyClientConfig.o ./src/Locale.d ./src/Locale.o ./src/MySqlConnInterface.d ./src/MySqlConnInterface.o ./src/PlotFileDefinition.d ./src/PlotFileDefinition.o ./src/PressureCalculator.d ./src/PressureCalculator.o ./src/ProgramConfig.d ./src/ProgramConfig.o ./src/RRDFileDefinition.d ./src/RRDFileDefinition.o ./src/SerialAsioThread.d ./src/SerialAsioThread.o ./src/SerialConfig.d ./src/SerialConfig.o ./src/SlewRateLimiter.d ./src/SlewRateLimiter.o ./src/Telemetry.d ./src/Telemetry.o ./src/Thingspeak.d ./src/Thingspeak.o ./src/TimeTools.d ./src/TimeTools.o ./src/WeatherlinkClient.d ./src/WeatherlinkClient.o ./src/XMLMemoryHandler.d ./src/XMLMemoryHandler.o ./src/ZywiecMeteo.d ./src/ZywiecMeteo.o ./src/main.d ./src/main.o

.PHONY: clean-src

