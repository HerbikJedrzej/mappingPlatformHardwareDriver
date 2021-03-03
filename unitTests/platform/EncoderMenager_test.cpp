#include <gtest/gtest.h>
#include <EncoderMenager.hh>
#include <tgmath.h>

namespace {

class EncoderMock : public PlatformDrivers::EncoderParserIfc{
public:
    double angle = {0};
    double run() override{
        return angle;
    }
};

#define _PI 3.14159265359
#define NEAR_PRECISION 0.0000001
#define SQRT_OF_2 1.4142135623730950488016887242097

TEST(EncoderMenager, moveForward){
    EncoderMock encoderRight;
    EncoderMock encoderLeft;
    Position position;
    PlatformDrivers::EncoderMenager menager(&encoderRight, &encoderLeft, {1.0/_PI, 2.0});
    encoderRight.angle = 360.0;
    encoderLeft.angle = 360.0;
    for(uint16_t i = 0; i < 10; i++){
        EXPECT_NEAR(position.x, double(i), NEAR_PRECISION);
        EXPECT_DOUBLE_EQ(position.y, 0.0);
        EXPECT_DOUBLE_EQ(position.rotation, 0.0);
        menager.run();
        position = menager.get();
    }
    EXPECT_NEAR(position.x, 10.0, NEAR_PRECISION);
    EXPECT_DOUBLE_EQ(position.y, 0.0);
    EXPECT_DOUBLE_EQ(position.rotation, 0.0);
}

TEST(EncoderMenager, moveBackwardRotated){
    EncoderMock encoderRight;
    EncoderMock encoderLeft;
    Position position;
    PlatformDrivers::EncoderMenager menager(&encoderRight, &encoderLeft, {1.0/_PI, 2.0});
    encoderRight.angle = -360.0;
    encoderLeft.angle = -360.0;
    menager.setPointData({2.0, 1.0, 90.0});
    position = menager.get();
    for(uint16_t i = 0; i < 10; i++){
        EXPECT_NEAR(position.x, 2.0, NEAR_PRECISION);
        EXPECT_NEAR(position.y, double(1 - i), NEAR_PRECISION);
        EXPECT_NEAR(position.rotation, 90.0, NEAR_PRECISION);
        menager.run();
        position = menager.get();
    }
    EXPECT_NEAR(position.x, 2.0, NEAR_PRECISION);
    EXPECT_NEAR(position.y, -9.0, NEAR_PRECISION);
    EXPECT_NEAR(position.rotation, 90.0, NEAR_PRECISION);
}

TEST(EncoderMenager, moveForwardRotatedAtSlant){
    EncoderMock encoderRight;
    EncoderMock encoderLeft;
    Position position;
    PlatformDrivers::EncoderMenager menager(&encoderRight, &encoderLeft, {SQRT_OF_2/_PI, 2.0});
    encoderRight.angle = 360.0;
    encoderLeft.angle = 360.0;
    menager.setPointData({10.0, 10.0, -45.0});
    position = menager.get();
    for(uint16_t i = 0; i < 10; i++){
        EXPECT_NEAR(position.x, double(10 + i), NEAR_PRECISION);
        EXPECT_NEAR(position.y, double(10 - i), NEAR_PRECISION);
        EXPECT_NEAR(position.rotation, 315.0, NEAR_PRECISION);
        menager.run();
        position = menager.get();
    }
    EXPECT_NEAR(position.x, 20.0, NEAR_PRECISION);
    EXPECT_NEAR(position.y, 0.0, NEAR_PRECISION);
    EXPECT_NEAR(position.rotation, 315.0, NEAR_PRECISION);
}

TEST(EncoderMenager, turnInPlaceBy90Degees){
    EncoderMock encoderRight;
    EncoderMock encoderLeft;
    Position position;
    PlatformDrivers::EncoderMenager menager(&encoderRight, &encoderLeft, {0.2, 8.0});
    encoderRight.angle = 360.0;
    encoderLeft.angle = -360.0;
    for(uint16_t i = 0; i < 10; i++){
        EXPECT_NEAR(position.x, 0.0, NEAR_PRECISION);
        EXPECT_NEAR(position.y, 0.0, NEAR_PRECISION);
        EXPECT_NEAR(position.rotation, double(9 * i), NEAR_PRECISION);
        menager.run();
        position = menager.get();
    }
    EXPECT_NEAR(position.x, 0.0, NEAR_PRECISION);
    EXPECT_NEAR(position.y, 0.0, NEAR_PRECISION);
    EXPECT_NEAR(position.rotation, 90.0, NEAR_PRECISION);
}

TEST(EncoderMenager, turnInPlaceBy414Degees){
    EncoderMock encoderRight;
    EncoderMock encoderLeft;
    Position position;
    PlatformDrivers::EncoderMenager menager(&encoderRight, &encoderLeft, {0.2, 8.0});
    encoderRight.angle = 360.0;
    encoderLeft.angle = -360.0;
    for(uint16_t i = 0; i < 46; i++){
        EXPECT_NEAR(position.x, 0.0, NEAR_PRECISION);
        EXPECT_NEAR(position.y, 0.0, NEAR_PRECISION);
        EXPECT_NEAR(position.rotation, double((9 * i) % 360), NEAR_PRECISION);
        menager.run();
        position = menager.get();
    }
    EXPECT_NEAR(position.x, 0.0, NEAR_PRECISION);
    EXPECT_NEAR(position.y, 0.0, NEAR_PRECISION);
    EXPECT_NEAR(position.rotation, 54.0, NEAR_PRECISION);
}

TEST(EncoderMenager, curveBy90Degrees){
    auto circle = [](const double& x, const double& centerX, const double& centerY, const double& radius)->double{
        const double tmpX = x - centerX;
        return sqrt(radius * radius - tmpX * tmpX) + centerY;
    };
    EncoderMock encoderRight;
    EncoderMock encoderLeft;
    Position position;
    PlatformDrivers::EncoderMenager menager(&encoderRight, &encoderLeft, {0.2, 4.0});
    menager.setPointData({2.0, 0.0, 90.0});
    menager.run();
    position = menager.get();
    EXPECT_DOUBLE_EQ(position.x, 2.0);
    EXPECT_DOUBLE_EQ(position.y, 0.0);
    EXPECT_DOUBLE_EQ(position.rotation, 90.0);
    encoderRight.angle = 360.0;
    for(uint16_t i = 0; i < 10; i++){
        menager.run();
        position = menager.get();
        EXPECT_NEAR(position.y, circle(position.x, 0.0, 0.0, 2.0), NEAR_PRECISION);
    }
    EXPECT_NEAR(position.x, 0.0, NEAR_PRECISION);
    EXPECT_NEAR(position.y, 2.0, NEAR_PRECISION);
    EXPECT_NEAR(position.rotation, 180.0, NEAR_PRECISION);
}

TEST(EncoderMenager, curveBy450Degrees){
    auto circle1 = [](const double& x, const double& centerX, const double& centerY, const double& radius)->double{
        const double tmpX = x - centerX;
        return sqrt(radius * radius - tmpX * tmpX) + centerY;
    };
    auto circle2 = [](const double& x, const double& centerX, const double& centerY, const double& radius)->double{
        const double tmpX = x - centerX;
        return centerY - sqrt(radius * radius - tmpX * tmpX);
    };
    EncoderMock encoderRight;
    EncoderMock encoderLeft;
    Position position;
    PlatformDrivers::EncoderMenager menager(&encoderRight, &encoderLeft, {0.2, 4.0});
    menager.setPointData({2.0, 0.0, 90.0});
    menager.run();
    position = menager.get();
    EXPECT_DOUBLE_EQ(position.x, 2.0);
    EXPECT_DOUBLE_EQ(position.y, 0.0);
    EXPECT_DOUBLE_EQ(position.rotation, 90.0);
    encoderRight.angle = 360.0;
    for(uint16_t i = 0; i < 20; i++){
        EXPECT_NEAR(position.y, circle1(position.x, 0.0, 0.0, 2.0), NEAR_PRECISION);
        menager.run();
        position = menager.get();
    }
    EXPECT_NEAR(position.x, -2.0, NEAR_PRECISION);
    EXPECT_NEAR(position.y, 0.0, NEAR_PRECISION);
    EXPECT_NEAR(position.rotation, 270.0, NEAR_PRECISION);
    const double positionX = position.x + 2;
    const double positionY = position.y;
    for(uint16_t i = 0; i < 20; i++){
        EXPECT_NEAR(position.y, circle2(position.x, positionX, positionY, 2.0), NEAR_PRECISION) << "i = " << i;
        menager.run();
        position = menager.get();
    }
    EXPECT_NEAR(position.x, 2.0, NEAR_PRECISION);
    EXPECT_NEAR(position.y, 0.0, NEAR_PRECISION);
    EXPECT_NEAR(position.rotation, 90.0, NEAR_PRECISION);
    for(uint16_t i = 0; i < 10; i++){
        menager.run();
        position = menager.get();
        EXPECT_NEAR(position.y, circle1(position.x, 0.0, 0.0, 2.0), NEAR_PRECISION);
    }
    EXPECT_NEAR(position.x, 0.0, NEAR_PRECISION);
    EXPECT_NEAR(position.y, 2.0, NEAR_PRECISION);
    EXPECT_NEAR(position.rotation, 180.0, NEAR_PRECISION);
}

TEST(EncoderMenager, curveForwardAndCurve){
    auto circle1 = [](const double& x, const double& centerX, const double& centerY, const double& radius)->double{
        const double tmpX = x - centerX;
        return centerY - sqrt(radius * radius - tmpX * tmpX);
    };
    auto circle2 = [](const double& x, const double& centerX, const double& centerY, const double& radius)->double{
        const double tmpX = x - centerX;
        return sqrt(radius * radius - tmpX * tmpX) + centerY;
    };
    EncoderMock encoderRight;
    EncoderMock encoderLeft;
    Position position;
    PlatformDrivers::EncoderMenager menager(&encoderRight, &encoderLeft, {0.2, 4.0});
    menager.run();
    encoderRight.angle = 360.0;
    for(uint16_t i = 0; i < 10; i++){
        EXPECT_NEAR(position.y, circle1(position.x, 0.0, 2.0, 2.0), NEAR_PRECISION);
        menager.run();
        position = menager.get();
    }
    EXPECT_NEAR(position.x, 2.0, NEAR_PRECISION);
    EXPECT_NEAR(position.y, 2.0, NEAR_PRECISION);
    EXPECT_NEAR(position.rotation, 90.0, NEAR_PRECISION);
    encoderRight.angle = 360.0/ _PI;
    encoderLeft.angle = 360.0/ _PI;
    for(uint16_t i = 0; i < 10; i++){
        EXPECT_NEAR(position.x, 2.0, NEAR_PRECISION);
        EXPECT_NEAR(position.y, 2.0 + 0.2 * double(i), NEAR_PRECISION);
        EXPECT_NEAR(position.rotation, 90.0, NEAR_PRECISION);
        menager.run();
        position = menager.get();
    }
    EXPECT_NEAR(position.x, 2.0, NEAR_PRECISION);
    EXPECT_NEAR(position.y, 4.0, NEAR_PRECISION);
    EXPECT_NEAR(position.rotation, 90.0, NEAR_PRECISION);
    encoderRight.angle = 0.0;
    encoderLeft.angle = 360.0;
    const double positionX = position.x + 2;
    const double positionY = position.y;
    for(uint16_t i = 0; i < 10; i++){
        EXPECT_NEAR(position.y, circle2(position.x, positionX, positionY, 2.0), NEAR_PRECISION);
        menager.run();
        position = menager.get();
    }
    EXPECT_NEAR(position.x, 4.0, NEAR_PRECISION);
    EXPECT_NEAR(position.y, 6.0, NEAR_PRECISION);
    EXPECT_NEAR(position.rotation, 0.0, NEAR_PRECISION);
}

TEST(EncoderMenager, angle700convertTo340){
    EncoderMock encoderRight;
    EncoderMock encoderLeft;
    Position position;
    PlatformDrivers::EncoderMenager menager(&encoderRight, &encoderLeft, {0.2, 8.0});
    menager.setPointData(Position(0.0, 0.0, 610));
    encoderRight.angle = 360.0;
    encoderLeft.angle = -360.0;
    position = menager.get();
    EXPECT_NEAR(position.x, 0.0, NEAR_PRECISION);
    EXPECT_NEAR(position.y, 0.0, NEAR_PRECISION);
    EXPECT_NEAR(position.rotation, 250.0, NEAR_PRECISION);
    for(uint16_t i = 0; i < 10; i++)
        menager.run();
    position = menager.get();
    EXPECT_NEAR(position.x, 0.0, NEAR_PRECISION);
    EXPECT_NEAR(position.y, 0.0, NEAR_PRECISION);
    EXPECT_NEAR(position.rotation, 340.0, NEAR_PRECISION);
}

TEST(EncoderMenager, angleMinus700convertTo20){
    EncoderMock encoderRight;
    EncoderMock encoderLeft;
    Position position;
    PlatformDrivers::EncoderMenager menager(&encoderRight, &encoderLeft, {0.2, 8.0});
    menager.setPointData(Position(0.0, 0.0, -790));
    encoderRight.angle = 360.0;
    encoderLeft.angle = -360.0;
    position = menager.get();
    EXPECT_NEAR(position.x, 0.0, NEAR_PRECISION);
    EXPECT_NEAR(position.y, 0.0, NEAR_PRECISION);
    EXPECT_NEAR(position.rotation, 290.0, NEAR_PRECISION);
    for(uint16_t i = 0; i < 10; i++)
        menager.run();
    position = menager.get();
    EXPECT_NEAR(position.x, 0.0, NEAR_PRECISION);
    EXPECT_NEAR(position.y, 0.0, NEAR_PRECISION);
    EXPECT_NEAR(position.rotation, 20.0, NEAR_PRECISION);
}

}