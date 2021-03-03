#include <gtest/gtest.h>
#include <Position.hh>
#include <tgmath.h>

#define AnglePidConverter(CURRENT,EXPECTED) \
    Position(100.0, -23.5, CURRENT).angleDiff(Position(100.0, -23.5, EXPECTED), 1.0)

TEST(angleDiff, current_50_expected_70_disslocation){
    EXPECT_DOUBLE_EQ(Position(0.0, 0.0, 0.0).angleDiff(Position(0.0, 5.0, 270.0)), 90.0);
}

TEST(angleDiff, current_20_expected_280_disslocation){
    constexpr double d = 4.0;
    EXPECT_DOUBLE_EQ(Position(10.0, 20.0, 20.0).angleDiff(Position(10.0 - d, 20.0 + d, 280.0)), -65.0);
}

TEST(angleDiff, current_20_expected_280_disslocation_2){
    constexpr double d = 4.0;
    EXPECT_DOUBLE_EQ(Position(10.0, 20.0, 10.0).angleDiff(Position(10.0 - d, 20.0 - d, 90.0)), 35.0);
}

TEST(angleDiff, current_50_expected_70){
    EXPECT_DOUBLE_EQ(20.0, AnglePidConverter(50, 70));
}

TEST(angleDiff, current_350_expected_270){
    EXPECT_DOUBLE_EQ(-80.0, AnglePidConverter(350, 270));
}

TEST(angleDiff, current_350_expected_10){
    EXPECT_DOUBLE_EQ(20.0, AnglePidConverter(350, 10));
}

TEST(angleDiff, current_20_expected_280){
    EXPECT_DOUBLE_EQ(-100.0, AnglePidConverter(20, 280));
}

TEST(angleDiff, current_60_expected_240){
    EXPECT_DOUBLE_EQ(180.0, AnglePidConverter(60, 240));
}

TEST(angleDiff, current_60_expected_241){
    EXPECT_DOUBLE_EQ(-179.9, AnglePidConverter(60, 240.1));
}

TEST(Dislocation, x0y0_x1y1_a45){
    EXPECT_DOUBLE_EQ(sqrt(2.0), Position(0.0, 0.0, 45.0).dislocation(Position(1.0, 1.0, 0.0)));
}

TEST(Dislocation, x0y0_x1y1_a0){
    EXPECT_DOUBLE_EQ(1.0, Position(0.0, 0.0, 0.0).dislocation(Position(1.0, 1.0, 0.0)));
}

TEST(Dislocation, x0y0_x1y1_a90){
    EXPECT_DOUBLE_EQ(1.0, Position(0.0, 0.0, 0.0).dislocation(Position(1.0, 1.0, 0.0)));
}

TEST(Dislocation, x0y0_x1y1_a135){
    EXPECT_NEAR(0.0, Position(0.0, 0.0, 135.0).dislocation(Position(1.0, 1.0, 0.0)), 0.0001);
}

TEST(Dislocation, x0y0_x1y1_a225){
    EXPECT_DOUBLE_EQ(-sqrt(2.0), Position(0.0, 0.0, 225.0).dislocation(Position(1.0, 1.0, 0.0)));
}

TEST(Dislocation, x10y_20_x_4y_18_a0){
    EXPECT_DOUBLE_EQ(-14.0, Position(10.0, -20.0, 0.0).dislocation(Position(-4.0, -18.0, 0.0)));
}

TEST(Dislocation, x10y_20_x_4y_18_a90){
    EXPECT_DOUBLE_EQ(2.0, Position(10.0, -20.0, 90.0).dislocation(Position(-4.0, -18.0, 0.0)));
}

TEST(Dislocation, x123y222_x120y226_a90){
    EXPECT_NEAR(-5.0, Position(123.5, 222.4, 60.0).dislocation(Position(120.5, 218.4, 0.0)), 0.05);
}
