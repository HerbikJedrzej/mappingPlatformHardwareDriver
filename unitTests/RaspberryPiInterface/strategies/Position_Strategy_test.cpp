#include <gtest/gtest.h>
#include <Position_Strategy.hh>
#include <AllDataUsed_Strategy.hh>
#include <TransmissionKey_Strategy.hh>
#include <memory>
#include <vector>

TEST(Position_Strategy, get){
    constexpr unsigned int size = 11;
    uint8_t data[size];
    double actualX, actualY;
    double expectedX, expectedY;
    RotationAngle actualAngle, expectedAngle;
    bool apply = true;
    StrategySpi::StrategySpiData spiData{data, size};
    StrategySpi::StrategySpiIfc* lastRunStrategy = nullptr;
    StrategySpi::StrategySpiIfc* dataChecked = new StrategySpi::AllDataUsedStrategy(nullptr, &data[size - 1]);
    std::shared_ptr<StrategySpi::StrategySpiIfc> strategy(new StrategySpi::TransmissionKeyStrategy(0x25, 0x26, nullptr,
        new StrategySpi::PositionStrategy(
            &actualX, &actualY, &actualAngle, &expectedX, &expectedY, &expectedAngle, &apply, dataChecked)
    ));
    ASSERT_EQ(strategy->getSize(), size);
    data[0] = 0x25;
    data[1] = 0xff;
    data[2] = 0xff;
    data[3] = 0xff;
    data[4] = 250;
    data[5] = 0xff;
    data[6] = 0xff;
    data[7] = 0xff;
    data[8] = 50;
    data[9] = 0;
    data[10] = 50;
    ASSERT_TRUE(strategy->get(spiData, &lastRunStrategy));
    ASSERT_EQ(lastRunStrategy, dataChecked);
    EXPECT_DOUBLE_EQ(expectedX, -0.006);
    EXPECT_DOUBLE_EQ(expectedY, -0.206);
    EXPECT_FALSE(apply);
    EXPECT_NEAR(expectedAngle, 50.0 / double(0x7fff) * 360.0, 0.0001);
}

TEST(Position_Strategy, set){
    constexpr unsigned int size = 11;
    uint8_t data[size];
    double actualX, actualY;
    double expectedX, expectedY;
    RotationAngle actualAngle, expectedAngle;
    bool apply = true;
    StrategySpi::StrategySpiData spiData{data, size};
    StrategySpi::StrategySpiIfc* lastRunStrategy = nullptr;
    StrategySpi::StrategySpiIfc* dataChecked = new StrategySpi::AllDataUsedStrategy(nullptr, &data[size - 1]);
    std::shared_ptr<StrategySpi::StrategySpiIfc> strategy(new StrategySpi::TransmissionKeyStrategy(0x25, 0x26));
    strategy->addNextStrategy(new StrategySpi::PositionStrategy(&actualX, &actualY, &actualAngle, &expectedX, &expectedY, &expectedAngle, &apply));
    strategy->addNextStrategy(dataChecked);
    actualX = 10.02;
    actualY = 20.02;
    actualAngle = 90;
    ASSERT_EQ(strategy->getSize(), size);
    ASSERT_TRUE(strategy->set(spiData, &lastRunStrategy));
    ASSERT_EQ(lastRunStrategy, dataChecked);
    EXPECT_EQ(data[0], 0x26);
    EXPECT_EQ(data[1], 0);
    EXPECT_EQ(data[2], 0);
    EXPECT_EQ(data[3], 39);
    EXPECT_EQ(data[4], 36);
    EXPECT_EQ(data[5], 0);
    EXPECT_EQ(data[6], 0);
    EXPECT_EQ(data[7], 78);
    EXPECT_EQ(data[8], 52);
    EXPECT_EQ(data[9], 0x80 | 32);
    EXPECT_EQ(data[10], 0);
}

TEST(Position_Strategy, getSize){
    double X;
    RotationAngle angle;
    bool apply;
    std::shared_ptr<StrategySpi::StrategySpiIfc> strategy(new StrategySpi::PositionStrategy(
        &X, &X, &angle, &X, &X, &angle, &apply
    ));
    EXPECT_EQ(strategy->getSize(), 10);
}
