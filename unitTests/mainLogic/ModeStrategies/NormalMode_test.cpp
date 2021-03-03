#include <gtest/gtest.h>
#include <uartToFile.hh>
#include <StrategyManager.hh>
#include <EncoderParser.hh>

#define TEST_UART Tools::UartLogFileManager::getUartInstance(std::string(test_info_->test_case_name()) + "." + std::string(test_info_->name())).get()

TEST(Strategy_NormalMode, run){
    unsigned int timer = 0;
    PlatformDrivers::Log log(TEST_UART, PlatformDrivers::LogingLevel::infoLogs, timer, 100);
    Drivers::GPIO gpio(
        [](OutputList, bool)->void{}, [](OutputList)->bool{return true;}, [](InputList)->bool{return true;}, [](uint32_t)->void{}
    );
    volatile uint32_t timerRegister = 0;
    Drivers::BothChannelEngineDriver<volatile uint32_t> engine(&gpio, OutputList::none, OutputList::none, &timerRegister, 100);
    PlatformDrivers::EncoderParser<volatile uint32_t> encoder(timerRegister, 10000, 1.0);
    PlatformDrivers::EncoderMenager encoderMenager(&encoder, &encoder, {80.0, 125.0});
    Drivers::Timer testTimer(100000000, 49999, 49, [](uint32_t)->void{});
    Drivers::US_015 distance(testTimer, &gpio, OutputList::Trig1, InputList::Echo1, InterruptInputList::EchoIrq1, [](uint32_t)->void{});
    double akcelerometr[3], gyroskope[3];
    AHRS ahrs(akcelerometr, gyroskope, nullptr);
    PID pid;
    uint16_t measurements[2];
    BatteryConfiguration batteryConfiguration{
        2, new uint32_t[2]{500, 2500}, new uint32_t[2]{2000, 2000}, 1000, 5.0
    };
    BatteryObserver batteryObserver(log, measurements, batteryConfiguration);

    PlatformDrivers::DriversPlatform driversPlatform{
        &engine, &engine, &encoderMenager, &distance, &distance, &distance, &gpio, &ahrs, &pid, &pid, &batteryObserver
    };
    InterfaceSpiData spiInterfaceData{log,
        0x25,
        0x26,
        0,
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        true,
        3,
        new double[3]{0.0, 0.0, 0.0},
        2,
        new double[2]{0.0, 0.0},
        new double[2]{0.0, 0.0},
        0.0,
        0.0
    };
    PlatformDrivers::ModeManager manager(driversPlatform, spiInterfaceData);

    manager.runSpiInterfaceLoop();
    manager.runPlatformLoop();
}