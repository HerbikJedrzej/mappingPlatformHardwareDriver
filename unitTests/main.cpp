#include <gtest/gtest.h>
#include <cstdlib>
#include <ctime>
#include <DriverIfc.hh>
#include <uartToFile.hh>
#include <Log.hh>

int main(int argc, char **argv){
  std::unique_ptr<Tools::UartLogFileManager> uart(new Tools::UartLogFileManager("UnitTests.log"));
  srand( time( NULL ) );
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
