#include <gtest/gtest.h>
#include <EncoderParser.hh>

namespace {

TEST(Encoder, rotateBy40DegreesInitialisedBy0){
    uint8_t encoderRegister = 0;
    PlatformDrivers::EncoderParser<uint8_t> encoder(encoderRegister, 200, 9);
    EXPECT_DOUBLE_EQ(encoder.run(), 0.0);
    encoderRegister = 1;
    EXPECT_DOUBLE_EQ(encoder.run(), 40.0);
}

TEST(Encoder, rotateBy40DegreesInitialisedBy10){
    uint8_t encoderRegister = 10;
    PlatformDrivers::EncoderParser<uint8_t> encoder(encoderRegister, 200, 9);
    EXPECT_DOUBLE_EQ(encoder.run(), 0.0);
    encoderRegister = 11;
    EXPECT_DOUBLE_EQ(encoder.run(), 40.0);
}

TEST(Encoder, rotateBy40DegreesAnd80Degrees){
    uint8_t encoderRegister = 0;
    PlatformDrivers::EncoderParser<uint8_t> encoder(encoderRegister, 200, 9);
    EXPECT_DOUBLE_EQ(encoder.run(), 0.0);
    encoderRegister = 1;
    EXPECT_DOUBLE_EQ(encoder.run(), 40.0);
    encoderRegister = 3;
    EXPECT_DOUBLE_EQ(encoder.run(), 80.0);
}

TEST(Encoder, rotateByMinus40DegreesInitialisedBy10){
    uint8_t encoderRegister = 10;
    PlatformDrivers::EncoderParser<uint8_t> encoder(encoderRegister, 200, 9);
    EXPECT_DOUBLE_EQ(encoder.run(), 0.0);
    encoderRegister = 9;
    EXPECT_DOUBLE_EQ(encoder.run(), -40.0);
}

TEST(Encoder, rotateByMinus40DegreesAndMinus80Degrees){
    uint8_t encoderRegister = 10;
    PlatformDrivers::EncoderParser<uint8_t> encoder(encoderRegister, 200, 9);
    EXPECT_DOUBLE_EQ(encoder.run(), 0.0);
    encoderRegister = 9;
    EXPECT_DOUBLE_EQ(encoder.run(), -40.0);
    encoderRegister = 7;
    EXPECT_DOUBLE_EQ(encoder.run(), -80.0);
}

TEST(Encoder, overflowClockwise){
    uint8_t encoderRegister = 199;
    PlatformDrivers::EncoderParser<uint8_t> encoder(encoderRegister, 200, 9);
    EXPECT_DOUBLE_EQ(encoder.run(), 0.0);
    encoderRegister = 2;
    EXPECT_DOUBLE_EQ(encoder.run(), 160.0);
}

TEST(Encoder, overflowCunterClockwise){
    uint8_t encoderRegister = 1;
    PlatformDrivers::EncoderParser<uint8_t> encoder(encoderRegister, 200, 9);
    EXPECT_DOUBLE_EQ(encoder.run(), 0.0);
    encoderRegister = 199;
    EXPECT_DOUBLE_EQ(encoder.run(), -120.0);
}

TEST(Encoder, lognRunInBothDirectionAndCountTotalAngle){
    uint8_t encoderRegister = 0;
    PlatformDrivers::EncoderParser<uint8_t> encoder(encoderRegister, 200, 36);
    for(unsigned int step = 1; step <= 500; step++){
        encoderRegister = step % 201;
        EXPECT_DOUBLE_EQ(encoder.run(), 10.0);
    }
    for(unsigned int step = 499; step > 100; step--){
        encoderRegister = step % 201;
        EXPECT_DOUBLE_EQ(encoder.run(), -10.0);
    }
}

TEST(Encoder, lognRunInBothDirectionAndCountTotalAngleInNegativeRange){
    uint8_t encoderRegister = 99;
    PlatformDrivers::EncoderParser<uint8_t> encoder(encoderRegister, 200, 36);
    for(unsigned int step = 500; step > 0; step--){
        encoderRegister = step % 201;
        EXPECT_DOUBLE_EQ(encoder.run(), -10.0);
    }
    for(unsigned int step = 2; step < 250; step++){
        encoderRegister = step % 201;
        EXPECT_DOUBLE_EQ(encoder.run(), 10.0);
    }
}

}