macro(addFlag flag)
  if(NOT ADDITIONAL_FLAGS MATCHES ${flag})
    set(ADDITIONAL_FLAGS "${ADDITIONAL_FLAGS} ${flag}")
  endif()
endmacro()

addFlag("-lm")

list(APPEND LOCAL_INCLUDE_PATHS
  platform
  mainLogic
  mainLogic/ModeStrategies
  RaspberryPiInterface
  RaspberryPiInterface/strategies
)

list(APPEND LOCAL_SOURCE_PATHS
  mainLogic/ModeStrategies/NormalMode.cpp
  mainLogic/ModeStrategies/TuneMovePID.cpp
  mainLogic/ModeStrategies/TuneRotatePID.cpp
  mainLogic/Position.cpp
  mainLogic/StrategyManager.cpp
  platform/AHRS.cpp
  platform/BatteryObserver.cpp
  platform/ComplementaryFilter.cpp
  platform/ConfigurationProvider.cpp
  platform/EncoderMenager.cpp
  platform/Kalman.cpp
  platform/PID.cpp
  platform/StepingMeanFilter.cpp
  RaspberryPiInterface/SPIparser.cpp
  RaspberryPiInterface/strategies/AllDataUsed_Strategy.cpp
  RaspberryPiInterface/strategies/Battery_Strategy.cpp
  RaspberryPiInterface/strategies/DistanceMeasure_Strategy.cpp
  RaspberryPiInterface/strategies/Double_Strategy.cpp
  RaspberryPiInterface/strategies/Position_Strategy.cpp
  RaspberryPiInterface/strategies/TransmissionKey_Strategy.cpp
)

function(prepend_list var prefix)
    set(temp "")
    foreach(f ${${var}})
        list(APPEND temp "${prefix}${f}")
    endforeach()
    set(${var} "${temp}" PARENT_SCOPE)
endfunction()
