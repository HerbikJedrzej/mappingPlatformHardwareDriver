#include <gtest/gtest.h>
#include <TransmissionKey_Strategy.hh>
#include <AllDataUsed_Strategy.hh>
#include <uartToFile.hh>
#include <memory>
#include <vector>

#define TEST_UART Tools::UartLogFileManager::getUartInstance(std::string(test_info_->test_case_name()) + "." + std::string(test_info_->name())).get()

TEST(TransmissionKey_Strategy, get){
    constexpr unsigned int size = 1;
    uint8_t data[size];
    unsigned int timer = 0;
    StrategySpi::StrategySpiData spiData{data, size};
    StrategySpi::StrategySpiIfc* lastRunStrategy = nullptr;
    StrategySpi::StrategySpiIfc* dataChecker = new StrategySpi::AllDataUsedStrategy(nullptr, &data[size - 1]);
    PlatformDrivers::Log log(TEST_UART, PlatformDrivers::LogingLevel::infoLogs, timer, 100);
    std::shared_ptr<StrategySpi::StrategySpiIfc> strategy(new StrategySpi::TransmissionKeyStrategy(0x25, 0x26,
        &log, dataChecker
    ));
    ASSERT_EQ(strategy->getSize(), size);
    data[0] = 0x25;
    ASSERT_TRUE(strategy->get(spiData, &lastRunStrategy));
    ASSERT_EQ(lastRunStrategy, dataChecker);
    spiData = {data, size};
    data[0] = 0x26;
    ASSERT_FALSE(strategy->get(spiData, &lastRunStrategy));
    ASSERT_EQ(lastRunStrategy, strategy.get());
}

TEST(TransmissionKey_Strategy, set){
    constexpr unsigned int size = 1;
    uint8_t data[size];
    StrategySpi::StrategySpiData spiData{data, size};
    StrategySpi::StrategySpiIfc* lastRunStrategy = nullptr;
    StrategySpi::StrategySpiIfc* dataChecker = new StrategySpi::AllDataUsedStrategy(&data[size - 1]);
    std::shared_ptr<StrategySpi::StrategySpiIfc> strategy(new StrategySpi::TransmissionKeyStrategy(0x25, 0x26,
        nullptr, dataChecker
    ));
    ASSERT_EQ(strategy->getSize(), size);
    ASSERT_TRUE(strategy->set(spiData, &lastRunStrategy));
    ASSERT_EQ(data[0], 0x26);
    ASSERT_EQ(lastRunStrategy, dataChecker);
}

TEST(TransmissionKey_Strategy, getSize){
    std::shared_ptr<StrategySpi::StrategySpiIfc> strategy(new StrategySpi::TransmissionKeyStrategy(0x25, 0x26));
    EXPECT_EQ(strategy->getSize(), 1);
}
