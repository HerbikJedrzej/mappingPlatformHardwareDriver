#include <gtest/gtest.h>
#include <uartToFile.hh>
#include <EncoderParser.hh>
#include <EncoderMenager.hh>
#include <PID.hh>
#include <ModelEngine.hh>
#include <ConfigurationProvider.hh>
#include <StrategyManager.hh>
#include <plot.hh>
#include <ModelEncoderParser.hh>
#include "EngineModel.hh"

TEST(TuneRotatePID, scenario){
  auto HAL_Delay = [](uint32_t){};
  constexpr double dt = 0.01;
  unsigned int localTimer = 0;
  PlatformDrivers::Log log(TEST_UART, PlatformDrivers::LogingLevel::infoLogs, localTimer, 100);
  Drivers::ModelEngine engineRight(MODEL_R);
  Drivers::ModelEngine engineLeft(MODEL_L);
  PlatformDrivers::ModelEncoderParser encoderRight(0.0);
  PlatformDrivers::ModelEncoderParser encoderLeft(0.0);
  Drivers::GPIO gpio(
    [](const OutputList, const bool)->void{},
    [](const OutputList)->bool{return true;},
    [](const InputList)->bool{return true;},
    HAL_Delay
  );
  uint16_t adcData[4] = {0xfff, 0xfff};
  Configuration::Configuration configuration = {0x65, 0xAC, MODE_PID_ROTATE,
    {
      1.3686,
      0.0093263,
      50.209,
      1.4
    },{
      0.0,
      0.0,
      0.0,
      0.0
    },
    2, 3.3, 0xfff, new uint32_t[configuration.numberOfBatteryCells], new uint32_t[configuration.numberOfBatteryCells]};
  Drivers::Timer timer(100000000, 49999, 49, HAL_Delay);
  gpio.pin(OutputList::StmReady, false);
  PlatformDrivers::EncoderMenager encoderMenager(&encoderRight, &encoderLeft, {80.0, 125.0});
  Drivers::US_015 distanceFront(timer, &gpio, OutputList::Trig3, InputList::Echo3, InterruptInputList::EchoIrq3, HAL_Delay);
  Drivers::US_015 distanceRight(timer, &gpio, OutputList::Trig2, InputList::Echo2, InterruptInputList::EchoIrq2, HAL_Delay);
  Drivers::US_015 distanceLeft(timer, &gpio, OutputList::Trig1, InputList::Echo1, InterruptInputList::EchoIrq1, HAL_Delay);
  InterfaceSpiData spiInterfaceData = {log, configuration.spiKeyTx, configuration.spiKeyRx, configuration.mode, {}, {}, false, 0, nullptr, 0, nullptr, nullptr, 0};
  spiInterfaceData.distanceMeasureSize = 3;
  spiInterfaceData.distanceMeasure = new double[3];
  spiInterfaceData.batteryCells = 2;
  spiInterfaceData.tuneVoltage = new double[2];
  spiInterfaceData.batteryVoltage = new double[2];
  PID pidEngineMove;
  PID pidEngineRotate;
  double ahrsDummyData[3];
  AHRS ahrs(ahrsDummyData, ahrsDummyData, nullptr);
  BatteryConfiguration batteryConfiguration{
    configuration.numberOfBatteryCells,
    configuration.upperResistors_Ohm,
    configuration.lowerResistors_Ohm,
    configuration.batteryAdcMaxWord,
    configuration.maxMeasuredVoltage
  };
  BatteryObserver batteryObserver(log, adcData, batteryConfiguration);
  PlatformDrivers::DriversPlatform driversPlatform{
    &engineRight,
    &engineLeft,
    &encoderMenager,
    &distanceFront,
    &distanceRight,
    &distanceLeft,
    &gpio,
    &ahrs,
    &pidEngineMove,
    &pidEngineRotate,
    &batteryObserver
  };
  PlatformDrivers::ModeManager manager(driversPlatform, spiInterfaceData);
  pidEngineMove.P = configuration.pidForward.p;
  pidEngineMove.I = configuration.pidForward.i;
  pidEngineMove.D = configuration.pidForward.d;
  pidEngineMove.AntiWindup = configuration.pidForward.antiWindUp;
  pidEngineMove.setLimit(-50.0, 50.0);
  pidEngineRotate.P = configuration.pidRotation.p;
  pidEngineRotate.I = configuration.pidRotation.i;
  pidEngineRotate.D = configuration.pidRotation.d;
  pidEngineRotate.AntiWindup = configuration.pidRotation.antiWindUp;
  pidEngineRotate.setLimit(-50.0, 50.0);
  distanceFront.init();
  distanceRight.init();
  distanceLeft.init();
  gpio.pin(OutputList::LED_YELLOW, true);
  gpio.pin(OutputList::LED_BLUE, true);
  engineRight.set(0);
  engineLeft.set(0);
  gpio.pin(OutputList::StmReady, true);

  // spiInterfaceData.getValuePID = -50.0;

  std::vector<double> time;
  std::vector<double> measuredX;
  std::vector<double> measuredY;
  std::vector<double> measuredRotation;
  std::vector<double> percentPower;
  constexpr double seconds = 20.0;
  spiInterfaceData.expectedPosition.rotation = 0.0;
  for(localTimer = 0; localTimer < int(seconds / dt); localTimer++){
    log.info("Start loop.");

    if(localTimer ==  20)
      spiInterfaceData.getValuePID = 50.0;
    if(localTimer == 120)
      spiInterfaceData.getValuePID *= 0.0;
    if(localTimer == 320)
      spiInterfaceData.getValuePID = -50.0;
    if(localTimer == 420)
      spiInterfaceData.getValuePID = 0.0;

    {
      if(localTimer % 4 == 0)
        manager.runSpiInterfaceLoop();
      manager.runPlatformLoop();
      encoderRight.setValue(engineRight.run());
      encoderLeft.setValue(engineLeft.run());
    }

    time.push_back(double(localTimer) * dt);
    measuredX.push_back(encoderMenager.get().x);
    measuredY.push_back(encoderMenager.get().y);
    measuredRotation.push_back(encoderMenager.get().rotation);
    percentPower.push_back(spiInterfaceData.getValuePID);
  }
  plot::figure_size(1200, 780);
  plot::named_plot("X (mm)", time, measuredX);
  plot::named_plot("Y (mm)", time, measuredY);
  plot::named_plot("Power (%)", time, percentPower);
  plot::named_plot("Rotation (deg)", time, measuredRotation);
  plot::title("PID.");
  plot::legend();
  plot::save("./moduleTestsOutputData/" + std::string(test_info_->test_case_name()) + ".png");
  plot::figure_close();
}
