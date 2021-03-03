#include <gtest/gtest.h>
#include <BatteryObserver.hh>
#include <uartToFile.hh>

#define TEST_UART Tools::UartLogFileManager::getUartInstance(std::string(test_info_->test_case_name()) + "." + std::string(test_info_->name())).get()

TEST(BatteryObserver, batteryVoltageVal){
    unsigned int timer = 0;
    PlatformDrivers::Log log(TEST_UART, PlatformDrivers::LogingLevel::infoLogs, timer, 100);
    uint16_t measurements[3] = {0, 0, 0};
    BatteryConfiguration batteryConfiguration{
        3, new uint32_t[3]{500, 2500, 3000}, new uint32_t[3]{2000, 2000, 1000}, 1000, 5.0
    };
    BatteryObserver battery(log, measurements, batteryConfiguration);
    measurements[0] = 500;
    measurements[1] = 300;
    measurements[2] = 400;
    EXPECT_DOUBLE_EQ(battery.batteryVoltageVal(0), 3.125);
    EXPECT_DOUBLE_EQ(battery.batteryVoltageVal(1), 0.25);
    EXPECT_DOUBLE_EQ(battery.batteryVoltageVal(2), 4.625);
    EXPECT_DOUBLE_EQ(battery.batteryVoltageVal(), 8.0);
}

TEST(BatteryObserver, countUpperResistor){
    unsigned int timer = 0;
    PlatformDrivers::Log log(TEST_UART, PlatformDrivers::LogingLevel::infoLogs, timer, 100);
    uint16_t measurements[3] = {0, 0, 0};
    BatteryConfiguration batteryConfiguration{
        3, new uint32_t[3]{1234, 225, 325}, new uint32_t[3]{2000, 2000, 1000}, 1000, 4.0
    };
    BatteryObserver battery(log, measurements, batteryConfiguration);
    measurements[0] = 500;
    measurements[1] = 300;
    measurements[2] = 400;
    EXPECT_NE(battery.batteryVoltageVal(0), 2.5);
    EXPECT_NE(battery.batteryVoltageVal(1), 0.2);
    EXPECT_NE(battery.batteryVoltageVal(2), 3.7);
    battery.countUpperResistor(0, 2.5);
    battery.countUpperResistor(1, 2.7);
    battery.countUpperResistor(2, 6.4);
    batteryConfiguration.upperResistors_Ohm[0] = 500;
    batteryConfiguration.upperResistors_Ohm[1] = 2500;
    batteryConfiguration.upperResistors_Ohm[2] = 3000;
    EXPECT_NEAR(battery.batteryVoltageVal(0), 2.5, 0.000001);
    EXPECT_NEAR(battery.batteryVoltageVal(1), 0.2, 0.000001);
    EXPECT_NEAR(battery.batteryVoltageVal(2), 3.7, 0.000001);
}

