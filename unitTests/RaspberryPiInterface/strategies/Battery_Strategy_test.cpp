#include <gtest/gtest.h>
#include <Battery_Strategy.hh>
#include <AllDataUsed_Strategy.hh>
#include <TransmissionKey_Strategy.hh>
#include <memory>
#include <vector>

TEST(Battery_Strategy, set_get){
    constexpr unsigned int size = 3;
    uint8_t data[size];
    double setVoltage[9] = {3.55, 2.25, 1.75, 1.5, 5.8, 15.6, 6.7, 25.8, 10.0};
    double getVoltage[9] = { 0.0,  0.0,  0.0, 0.0, 0.0,  0.0, 0.0,  0.0,  0.0};
    StrategySpi::StrategySpiData spiData{data, size};
    StrategySpi::StrategySpiIfc* lastRunStrategy = nullptr;
    StrategySpi::StrategySpiIfc* dataChecked = new StrategySpi::AllDataUsedStrategy(nullptr, &data[size - 1]);
    std::shared_ptr<StrategySpi::StrategySpiIfc> strategy(new StrategySpi::TransmissionKeyStrategy(0x25, 0x25, nullptr,
        new StrategySpi::BatteryStrategy(setVoltage, getVoltage, 8)));
    strategy->addNextStrategy(dataChecked);
    ASSERT_EQ(strategy->getSize(), size);

    for(uint16_t j = 0; j < 10; j++){
        spiData = {data, size};
        ASSERT_TRUE(strategy->set(spiData, &lastRunStrategy));
        ASSERT_EQ(lastRunStrategy, dataChecked);
        spiData = {data, size};
        ASSERT_TRUE(strategy->get(spiData, &lastRunStrategy));
        ASSERT_EQ(lastRunStrategy, dataChecked);
        for(uint8_t i = 0; i < size; i++)
            EXPECT_NE(data[i], 0);
        for(uint16_t i = 0; i <= j && i < 8; i++)
            EXPECT_DOUBLE_EQ(setVoltage[i], getVoltage[i]) << "j = " << j << " i = " << i;
        for(uint16_t i = j + 1; i < 8; i++)
            EXPECT_NE(setVoltage[i], getVoltage[i]) << "j = " << j << " i = " << i;
    }
    EXPECT_NE(setVoltage[8], getVoltage[8]);
}

TEST(Battery_Strategy, getSize){
    double X;
    std::shared_ptr<StrategySpi::StrategySpiIfc> strategy(new StrategySpi::BatteryStrategy(&X, &X, 1));
    EXPECT_EQ(strategy->getSize(), 2);
}
