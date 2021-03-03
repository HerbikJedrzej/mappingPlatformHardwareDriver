#include <gtest/gtest.h>
#include <Log.hh>
#include <UART_Ifc.hh>

namespace {

void TestDelay(uint32_t){}
class TestUart : public Drivers::UART_Ifc{
    std::string stringToCompare;
    unsigned int lineForInformation;
public:
    void setStringToCompare(const std::string& txt, unsigned int line){
        lineForInformation = line;
        stringToCompare = txt;
    }
    TestUart():
        Drivers::UART_Ifc(TestDelay){}
    Drivers::BusStatus read(uint8_t*, uint16_t) override{
        return Drivers::BusStatus::OK;
    }
    Drivers::BusStatus write(uint8_t*, uint16_t) override{
        return Drivers::BusStatus::OK;
    }
    Drivers::BusStatus writeDMAwithoutAlocate(uint8_t *Data, uint16_t Size, DriverIfc* _callMe) override{
        return writeDMA(Data, Size, _callMe);
    }
    Drivers::BusStatus writeDMA(uint8_t *Data, uint16_t Size, DriverIfc*) override{
        std::string original((const char*)Data);
        std::string part(original.substr(0, Size));
        EXPECT_STREQ(part.c_str(), stringToCompare.c_str()) << "Occured in line:" << lineForInformation << std::endl;
        return Drivers::BusStatus::OK;
    }
    bool isBussy() override{
        return true;
    }
};

TEST(Log, noneLogs){
    unsigned int timer = 0;
    PlatformDrivers::Log log(nullptr, PlatformDrivers::LogingLevel::noneLogs, timer, 60);
    log.error("first = %2f4, second = %3d, third = %d fourth = %d.", 20.24, 65, 7, 16);
    log.warning("1'st = %d, 2'nd = %d, 3'rd = %2f2 4'th = %d.", 522, 654, 7.2, 6);
    log.info("one = %d, two = %d, thre = %d four = %f1.", 2, 61, 17, 6.2);
}

TEST(Log, errorLogs){
    TestUart uart;
    unsigned int timer = 0;
    PlatformDrivers::Log log(&uart, PlatformDrivers::LogingLevel::errorLogs, timer, 70);
    uart.setStringToCompare("E    0: first = 20.2400004, second =  65, third = 7 fourth = 16.\n", __LINE__);
    log.error("first = %2f4, second = %3d, third = %d fourth = %d.", 20.24, 65, 7, 16);
    log.warning("1'st = %d, 2'nd = %d, 3'rd = %2f2 4'th = %d.", 522, 654, 7.2, 6);
    log.info("one = %d, two = %d, thre = %d four = %f1.", 2, 61, 17, 6.2);
}

TEST(Log, warningLogs){
    TestUart uart;
    unsigned int timer = 120;
    PlatformDrivers::Log log(&uart, PlatformDrivers::LogingLevel::warningLogs, timer, 70);
    uart.setStringToCompare("E  120: first = 20.2400004, second =  65, third = 7 fourth = 16.\n", __LINE__);
    log.error("first = %2f4, second = %3d, third = %d fourth = %d.", 20.24, 65, 7, 16);
    uart.setStringToCompare("W  121: 1'st = 522, 2'nd = 654, 3'rd = 7.2000002 4'th = 6.\n", __LINE__);
    timer++;
    log.warning("1'st = %d, 2'nd = %d, 3'rd = %2f2 4'th = %d.", 522, 654, 7.2, 6);
    log.info("one = %d, two = %d, thre = %d four = %f1.", 2, 61, 17, 6.2);
}

TEST(Log, infoLogs){
    TestUart uart;
    unsigned int timer = 10;
    PlatformDrivers::Log log(&uart, PlatformDrivers::LogingLevel::infoLogs, timer, 70);
    uart.setStringToCompare("E   10: first = 20.2400004, second =  65, third = 7 fourth = 16.\n", __LINE__);
    log.error("first = %2f4, second = %3d, third = %d fourth = %d.", 20.24, 65, 7, 16);
    uart.setStringToCompare("W   10: 1'st = 522, 2'nd = 654, 3'rd = 7.2000002 4'th = 6.\n", __LINE__);
    log.warning("1'st = %d, 2'nd = %d, 3'rd = %2f2 4'th = %d.", 522, 654, 7.2, 6);
    uart.setStringToCompare("I   10: one = 2, two = 61, thre = 17 four = 6.2000001.\n", __LINE__);
    log.info("one = %d, two = %d, thre = %d four = %f1.", 2, 61, 17, 6.2);
}

}