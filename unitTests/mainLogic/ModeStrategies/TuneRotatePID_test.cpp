#include <gtest/gtest.h>
#include <uartToFile.hh>
#include <StrategyManager.hh>
#include <EncoderParser.hh>

#define TEST_UART Tools::UartLogFileManager::getUartInstance(std::string(test_info_->test_case_name()) + "." + std::string(test_info_->name())).get()

TEST(Strategy_TuneRotatePID, run){
    unsigned int timer = 0;
    PlatformDrivers::Log log(TEST_UART, PlatformDrivers::LogingLevel::infoLogs, timer, 100);
    Drivers::GPIO gpio(
        [](OutputList, bool)->void{}, [](OutputList)->bool{return true;}, [](InputList)->bool{return true;}, [](uint32_t)->void{}
    );
    volatile uint32_t timerRegister = 0;
    Drivers::BothChannelEngineDriver<volatile uint32_t> engineR(&gpio, OutputList::none, OutputList::none, &timerRegister, 100);
    Drivers::BothChannelEngineDriver<volatile uint32_t> engineL(&gpio, OutputList::none, OutputList::none, &timerRegister, 100);
    PlatformDrivers::EncoderParser<volatile uint32_t> encoderR(timerRegister, 10000,  10.0);
    PlatformDrivers::EncoderParser<volatile uint32_t> encoderL(timerRegister, 10000, -10.0);
    PlatformDrivers::EncoderMenager encoderMenager(&encoderR, &encoderL, {80.0, 125.0});
    uint16_t measurements[2];
    BatteryConfiguration batteryConfiguration{
        2, new uint32_t[2]{500, 2500}, new uint32_t[2]{2000, 2000}, 1000, 5.0
    };
    BatteryObserver batteryObserver(log, measurements, batteryConfiguration);

    PlatformDrivers::DriversPlatform driversPlatform{
        &engineR, &engineL, &encoderMenager, nullptr, nullptr, nullptr, &gpio, nullptr, nullptr, nullptr, &batteryObserver
    };
    InterfaceSpiData spiInterfaceData{log,
        0x25,
        0x26,
        1,
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        true,
        0,
        nullptr,
        2,
        new double[2]{0.0, 0.0},
        new double[2]{0.0, 0.0},
        0.0,
        0.0
    };
    PlatformDrivers::ModeManager manager(driversPlatform, spiInterfaceData);

    manager.runSpiInterfaceLoop();
    manager.runPlatformLoop();
    spiInterfaceData.getValuePID = 50.5;
    manager.runPlatformLoop();
    manager.runSpiInterfaceLoop();
    EXPECT_EQ(timerRegister, 0);
    manager.runPlatformLoop();
    EXPECT_NE(timerRegister, 0);
    manager.runPlatformLoop();
    EXPECT_DOUBLE_EQ(spiInterfaceData.setValuePID, 0.0);
    manager.runSpiInterfaceLoop();
    EXPECT_NE(spiInterfaceData.setValuePID, 0.0);
}