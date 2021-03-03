#include <gtest/gtest.h>
#include <SPIparser.hh>
#include <uartToFile.hh>
#include <SPIprotocol.hh>

#define TEST_UART Tools::UartLogFileManager::getUartInstance(std::string(test_info_->test_case_name()) + "." + std::string(test_info_->name())).get()

namespace {

class TestingPlatform : public SPIparser{
    PlatformDrivers::Log& log;
public:
    TestingPlatform(SPIprotocol& spi, InterfaceSpiData& _interfaceData):
        SPIparser(&spi, _interfaceData),
        log(_interfaceData.log){
        }
    StrategySpi::StrategySpiIfc* getStrategiesList_test(){
        return strategies;
    }
    void parseIncomingData_test(){
        parseIncomingData();
    }
    void parseOutgoingData_test(){
        parseOutgoingData();
    }
};

}

TEST(SPIparser, onlyStrategies){
    SPIprotocol spi;
    unsigned int timer = 0;
    PlatformDrivers::Log log(TEST_UART, PlatformDrivers::LogingLevel::infoLogs, timer, 100);
    InterfaceSpiData interfaceData{log, 0x25, 0x26, 0, {}, {}, false, 0, nullptr, 0, nullptr, nullptr, 0};
    TestingPlatform platform(spi, interfaceData);
    auto strategies = platform.getStrategiesList_test();
    const unsigned int size = strategies->getSize();
    uint8_t* data;
    data = new uint8_t[size];
    {
        interfaceData.actualPosition.x = 124.51;
        interfaceData.actualPosition.y = -4.51;
        interfaceData.actualPosition.rotation = 180.0;
        for(uint8_t i = 0; i < 100; i++){
            StrategySpi::StrategySpiData strategyData = {data, size};
            StrategySpi::StrategySpiIfc* lastRun = strategies;
            EXPECT_FALSE(strategies->set(strategyData, &lastRun));
            EXPECT_STREQ("AllDataUsedStrategy", lastRun->getName());
        }
    }
    data[0]++;
    {
        EXPECT_DOUBLE_EQ(interfaceData.expectedPosition.x, 0.0);
        EXPECT_DOUBLE_EQ(interfaceData.expectedPosition.y, 0.0);
        EXPECT_DOUBLE_EQ(interfaceData.expectedPosition.rotation, 0.0);
        for(uint8_t i = 0; i < 100; i++){
            StrategySpi::StrategySpiData strategyData = {data, size};
            StrategySpi::StrategySpiIfc* lastRun = strategies;
            EXPECT_FALSE(strategies->get(strategyData, &lastRun));
            EXPECT_STREQ("AllDataUsedStrategy", lastRun->getName());
            EXPECT_DOUBLE_EQ(interfaceData.expectedPosition.x, 124.51);
            EXPECT_DOUBLE_EQ(interfaceData.expectedPosition.y, -4.51);
            EXPECT_DOUBLE_EQ(interfaceData.expectedPosition.rotation, 180.0);
        }
    }
    delete [] data;
}

TEST(SPIparser, checkParser){
    SPIprotocol spi;
    unsigned int timer = 0;
    PlatformDrivers::Log log(TEST_UART, PlatformDrivers::LogingLevel::infoLogs, timer, 100);
    InterfaceSpiData interfaceData{log, 0x25, 0x26, 0, {}, {}, false, 0, nullptr, 0, nullptr, nullptr, 0};
    SPIparser parser(&spi, interfaceData);
    for(uint8_t i = 0; i < 100; i++){
        interfaceData.actualPosition.x = 124.51;
        interfaceData.actualPosition.y = -4.51;
        interfaceData.actualPosition.rotation = 180.0;
        spi.markUsedData();
        spi.unmarkUsedData();
        parser.run();
        parser.run();
        for(unsigned int i = 0; i < spi.size(); i++)
            spi.getRxData()[i] = spi.getTxData()[i];
        spi.getRxData()[0]++;
        interfaceData.expectedPosition.x = 0;
        interfaceData.expectedPosition.y = 0;
        interfaceData.expectedPosition.rotation = 0;
        EXPECT_DOUBLE_EQ(interfaceData.expectedPosition.x, 0.0);
        EXPECT_DOUBLE_EQ(interfaceData.expectedPosition.y, 0.0);
        EXPECT_DOUBLE_EQ(interfaceData.expectedPosition.rotation, 0.0);
        spi.markUsedData();
        spi.unmarkUsedData();
        parser.run();
        parser.run();
        EXPECT_DOUBLE_EQ(interfaceData.expectedPosition.x, 124.51);
        EXPECT_DOUBLE_EQ(interfaceData.expectedPosition.y, -4.51);
        EXPECT_DOUBLE_EQ(interfaceData.expectedPosition.rotation, 180.0);
    }
}
