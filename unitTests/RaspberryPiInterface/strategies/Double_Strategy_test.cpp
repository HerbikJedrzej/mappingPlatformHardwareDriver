#include <gtest/gtest.h>
#include <Double_Strategy.hh>
#include <AllDataUsed_Strategy.hh>
#include <TransmissionKey_Strategy.hh>
#include <memory>
#include <vector>

TEST(Double_Strategy, get_set){
    constexpr unsigned int size = 9;
    uint8_t data[size];
    double setValue = -1456.53;
    double getValue = 0.0;
    StrategySpi::StrategySpiData spiData{data, size};
    StrategySpi::StrategySpiIfc* lastRunStrategy = nullptr;
    StrategySpi::StrategySpiIfc* dataChecked = new StrategySpi::AllDataUsedStrategy(nullptr, &data[size - 1]);
    std::shared_ptr<StrategySpi::StrategySpiIfc> strategy(new StrategySpi::TransmissionKeyStrategy(0x25, 0x25, nullptr,
        new StrategySpi::DoubleStrategy(&setValue, &getValue)));
    strategy->addNextStrategy(dataChecked);
    ASSERT_EQ(strategy->getSize(), size);
    ASSERT_TRUE(strategy->set(spiData, &lastRunStrategy));
    ASSERT_EQ(lastRunStrategy, dataChecked);
    spiData = {data, size};
    ASSERT_TRUE(strategy->get(spiData, &lastRunStrategy));
    ASSERT_EQ(lastRunStrategy, dataChecked);
    for(uint8_t i = 0; i < size; i++)
        EXPECT_NE(data[i], 0);
    EXPECT_DOUBLE_EQ(setValue, getValue);
}

TEST(Double_Strategy, get_set_empty){
    constexpr unsigned int size = 9;
    uint8_t data[size];
    double getValue = -1456.53;
    StrategySpi::StrategySpiData spiData{data, size};
    StrategySpi::StrategySpiIfc* lastRunStrategy = nullptr;
    StrategySpi::StrategySpiIfc* dataChecked = new StrategySpi::AllDataUsedStrategy(nullptr, &data[size - 1]);
    std::shared_ptr<StrategySpi::StrategySpiIfc> strategy(new StrategySpi::TransmissionKeyStrategy(0x25, 0x25, nullptr,
        new StrategySpi::DoubleStrategy(nullptr, &getValue)));
    strategy->addNextStrategy(dataChecked);
    ASSERT_EQ(strategy->getSize(), size);
    ASSERT_TRUE(strategy->set(spiData, &lastRunStrategy));
    ASSERT_EQ(lastRunStrategy, dataChecked);
    spiData = {data, size};
    ASSERT_TRUE(strategy->get(spiData, &lastRunStrategy));
    ASSERT_EQ(lastRunStrategy, dataChecked);
    for(uint8_t i = 1; i < size; i++)
        EXPECT_EQ(data[i], 0);
    EXPECT_DOUBLE_EQ(0.0, getValue);
}

TEST(Double_Strategy, getSize){
    double X;
    std::shared_ptr<StrategySpi::StrategySpiIfc> strategy(new StrategySpi::DoubleStrategy(&X, &X));
    EXPECT_EQ(strategy->getSize(), 8);
}
