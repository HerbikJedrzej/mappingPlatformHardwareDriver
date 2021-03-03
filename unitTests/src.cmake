list(APPEND TESTS_SRC
  ../tools/uartToFile.cpp
  main.cpp
  mainLogic/Log_test.cpp
  mainLogic/ModeStrategies/NormalMode_test.cpp
  mainLogic/ModeStrategies/TuneMovePID_test.cpp
  mainLogic/ModeStrategies/TuneRotatePID_test.cpp
  mainLogic/Position_test.cpp
  platform/BatteryObserver_test.cpp
  platform/ConfigurationProvider_test.cpp
  platform/EncoderMenager_test.cpp
  platform/EncoderParser_test.cpp
  RaspberryPiInterface/SPIparser_test.cpp
  RaspberryPiInterface/SPIprotocol_test.cpp
  RaspberryPiInterface/strategies/AllDataUsed_Strategy_test.cpp
  RaspberryPiInterface/strategies/Battery_Strategy_test.cpp
  RaspberryPiInterface/strategies/DistanceMeasure_Strategy_test.cpp
  RaspberryPiInterface/strategies/Double_Strategy_test.cpp
  RaspberryPiInterface/strategies/Position_Strategy_test.cpp
  RaspberryPiInterface/strategies/TransmissionKey_Strategy_test.cpp
)