#include <gtest/gtest.h>
#include <SPIprotocol.hh>

TEST(SPIprotocol, size){
    SPIprotocol spi1;
    SPIprotocol spi2;
    EXPECT_EQ(1, spi1.size());
    EXPECT_EQ(1, spi2.size());
    spi1.setSize(14);
    spi2.setSize(16);
    EXPECT_EQ(14, spi1.size());
    EXPECT_EQ(16, spi2.size());
}

TEST(SPIprotocol, flags){
    SPIprotocol spi;
    spi.setSize(10);
    EXPECT_TRUE(spi.isReadyToUse());
    spi.markUsedData();
    EXPECT_FALSE(spi.isReadyToUse());
    spi.unmarkUsedData();
    EXPECT_TRUE(spi.isReadyToUse());
    spi.markUsed();
    EXPECT_FALSE(spi.isReadyToUse());
    spi.markUsedData();
    EXPECT_FALSE(spi.isReadyToUse());
    spi.unmarkUsedData();
    EXPECT_TRUE(spi.isReadyToUse());
}