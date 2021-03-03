#include <gtest/gtest.h>
#include <ConfigurationProvider.hh>
#include <uartToFile.hh>
#include <vector>
#include <queue>

#define TEST_UART Tools::UartLogFileManager::getUartInstance(std::string(test_info_->test_case_name()) + "." + std::string(test_info_->name())).get()

namespace {

class I2C_mock : Drivers::I2C_Ifc{
public:
    struct I2Ccell{
        uint16_t addr;
        std::vector<uint8_t> data;
    };
    void setBuffor(const std::vector<I2Ccell>& data){
        for(const auto& elem : data)
            buffor.push(elem);
    }
    bool ClearAndCheck(){
        if(buffor.empty())
            return true;
        while(!buffor.empty())
            buffor.pop();
        return false;
    }
    I2C_mock(const uint8_t address):
        Drivers::I2C_Ifc([](uint32_t){}),
        addr(address){}
    Drivers::BusStatus read(uint8_t DevAddress, uint16_t MemAddress, bool MemAddrIs16b, uint8_t *pData, uint16_t Size)override{
        return read(DevAddress, MemAddress, MemAddrIs16b, pData, Size, nullptr);
    }
    Drivers::BusStatus write(uint8_t DevAddress, uint16_t MemAddress, bool MemAddrIs16b, uint8_t *pData, uint16_t Size)override{
        return write(DevAddress, MemAddress, MemAddrIs16b, pData, Size, nullptr);
    }
    Drivers::BusStatus read(uint8_t, uint16_t MemAddress, bool, uint8_t *pData, uint16_t Size, Drivers::DriverIfc*)override{
        if(buffor.empty())
            throw std::out_of_range("Too less cells in buffor.");
        auto elem = buffor.front();
        buffor.pop();
        EXPECT_EQ(elem.addr, MemAddress);
        EXPECT_EQ(elem.data.size(), Size) << "Cell addr: " << elem.addr;
        if(elem.data.size() != Size)
            return Drivers::BusStatus::ERR;
        for(unsigned int i = 0; i < Size; i++)
            pData[i] = elem.data[i];
        return Drivers::BusStatus::OK;
    }
    Drivers::BusStatus write(uint8_t, uint16_t, bool, uint8_t*, uint16_t, Drivers::DriverIfc*)override{
        throw std::invalid_argument("I2C writing cannot be used.");
    }
private:
    const uint8_t addr;
    std::queue<I2Ccell> buffor;
};

#define READ_PARAMETER(ADDR,VARIABLE) if(!getFromMemory(ADDR,VARIABLE,#VARIABLE))return false;

class ConfigurationProvider_testHelper : public ConfigurationProvider{
    PlatformDrivers::Log log;
    I2C_mock i2c;
    const unsigned int timer;
public:
    ConfigurationProvider_testHelper(Drivers::UART_Ifc* uart, const std::vector<I2C_mock::I2Ccell>& data):
        ConfigurationProvider(log, 0x56, (Drivers::I2C_Ifc*)(&i2c)),
        log(uart, PlatformDrivers::LogingLevel::infoLogs, timer, 200),
        i2c(0x56),
        timer(0){
            i2c.setBuffor(data);
        }
    ~ConfigurationProvider_testHelper(){
        EXPECT_TRUE(i2c.ClearAndCheck());
    }
    bool readingList(Configuration::Configuration& config) override{
        READ_PARAMETER(0x000a, config.spiKeyTx);
        READ_PARAMETER(0x000b, config.spiKeyRx);
        READ_PARAMETER(0x000c, config.mode);
        READ_PARAMETER(0x0100, config.pidForward.p);
        READ_PARAMETER(0x0108, config.pidForward.i);
        READ_PARAMETER(0x0110, config.pidForward.d);
        READ_PARAMETER(0x0118, config.pidForward.antiWindUp);
        READ_PARAMETER(0x0120, config.pidRotation.p);
        READ_PARAMETER(0x0128, config.pidRotation.i);
        READ_PARAMETER(0x0130, config.pidRotation.d);
        READ_PARAMETER(0x0138, config.pidRotation.antiWindUp);
        return true;
    }
};

}

TEST(ConfigurationProvider, runCorrectKey){
    ConfigurationProvider_testHelper provider(TEST_UART, {
        {0x0000, {0x15, 0x26, 0x37, 0x48, 0x59, 0x6a, 0x7b, 0x8c, 0x9d, 0xae}},
        {0x000a, {10}},
        {0x000b, {11}},
        {0x000c, {200}},
        {0x0100, {0,0,0,0,20,20,0,0xc1}},
        {0x0108, {0,0,0,0,10,20,0,0xc1}},
        {0x0110, {0,0,0,0,20,30,0,0xc1}},
        {0x0118, {0,0,0,0,30,40,0,0xc1}},
        {0x0120, {0,0,0,0,40,50,0,0xc1}},
        {0x0128, {0,0,0,0,50,60,0,0xc1}},
        {0x0130, {0,0,0,0,60,70,0,0xc1}},
        {0x0138, {0,0,0,0,70,80,0,0xc1}},
    });
    Configuration::Configuration configuration;
    EXPECT_TRUE(provider.run(configuration));
    EXPECT_EQ(configuration.spiKeyTx, 10);
    EXPECT_EQ(configuration.spiKeyRx, 11);
    EXPECT_EQ(configuration.mode, 200);
    EXPECT_DOUBLE_EQ(configuration.pidForward.p, -131714.5);
    EXPECT_DOUBLE_EQ(configuration.pidForward.i, -131713.25);
    EXPECT_DOUBLE_EQ(configuration.pidForward.d, -132034.5);
    EXPECT_DOUBLE_EQ(configuration.pidForward.antiWindUp, -132355.75);
    EXPECT_DOUBLE_EQ(configuration.pidRotation.p, -132677.0);
    EXPECT_DOUBLE_EQ(configuration.pidRotation.i, -132998.25);
    EXPECT_DOUBLE_EQ(configuration.pidRotation.d, -133319.5);
    EXPECT_DOUBLE_EQ(configuration.pidRotation.antiWindUp, -133640.75);
}

TEST(ConfigurationProvider, runWrongKey){
    ConfigurationProvider_testHelper provider(TEST_UART, {
        {0x0000, {0x15, 0x26, 0x37, 0x48, 0x59, 0x6a, 0x7b, 0x8d, 0x9d, 0xae}},
    });
    Configuration::Configuration configuration;
    EXPECT_FALSE(provider.run(configuration));
}
