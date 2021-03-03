#include <gtest/gtest.h>
#include <DistanceMeasure_Strategy.hh>
#include <AllDataUsed_Strategy.hh>
#include <TransmissionKey_Strategy.hh>
#include <memory>
#include <vector>

TEST(DistanceMeasure_Strategy, get){
    constexpr unsigned int size = 3;
    double distance;
    uint8_t data[size];
    StrategySpi::StrategySpiData spiData{data, size};
    StrategySpi::StrategySpiIfc* lastRunStrategy = nullptr;
    StrategySpi::StrategySpiIfc* dataChecked = new StrategySpi::AllDataUsedStrategy(nullptr, &data[size - 1]);
    std::shared_ptr<StrategySpi::StrategySpiIfc> strategy(new StrategySpi::TransmissionKeyStrategy(0x25, 0x26, nullptr,
        new StrategySpi::DistanceMeasureStrategy(&distance)
    ));
    strategy->addNextStrategy(dataChecked);
    ASSERT_EQ(strategy->getSize(), size);
    data[0] = 0x25;
    data[1] = 50;
    data[2] = 50;
    ASSERT_TRUE(strategy->get(spiData, &lastRunStrategy));
    ASSERT_EQ(lastRunStrategy, dataChecked);
    EXPECT_NEAR(distance, 0.0, 0.0001);
    data[0] = 0x26;
    data[1] = 250;
    data[2] = 250;
    ASSERT_FALSE(strategy->get(spiData, &lastRunStrategy));
    ASSERT_EQ(lastRunStrategy, strategy.get());
    EXPECT_NEAR(distance, 0.0, 0.0001);
}

TEST(DistanceMeasure_Strategy, set){
    constexpr unsigned int size = 3;
    double distance;
    uint8_t data[size];
    StrategySpi::StrategySpiData spiData{data, size};
    StrategySpi::StrategySpiIfc* lastRunStrategy = nullptr;
    StrategySpi::StrategySpiIfc* dataChecked = new StrategySpi::AllDataUsedStrategy(nullptr, &data[size - 1]);
    std::shared_ptr<StrategySpi::StrategySpiIfc> strategy(new StrategySpi::TransmissionKeyStrategy(0x25, 0x26));
    strategy->addNextStrategy(new StrategySpi::DistanceMeasureStrategy(&distance));
    strategy->addNextStrategy(dataChecked);
    distance = 1246;
    ASSERT_EQ(strategy->getSize(), size);
    ASSERT_TRUE(strategy->set(spiData, &lastRunStrategy));
    ASSERT_EQ(lastRunStrategy, dataChecked);
    EXPECT_EQ(data[0], 0x26);
    EXPECT_EQ(uint16_t(data[1] << 8 | data[2]), uint16_t(1246.0 / 2500.0 * double(0xffff)));
}

TEST(DistanceMeasure_Strategy, getSize){
    double X;
    std::shared_ptr<StrategySpi::StrategySpiIfc> strategy(new StrategySpi::DistanceMeasureStrategy(&X));
    EXPECT_EQ(strategy->getSize(), 2);
}
