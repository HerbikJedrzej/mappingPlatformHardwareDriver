#pragma once

#include <Models.hh>

#define TEST_UART Tools::UartLogFileManager::getUartInstance(std::string(test_info_->test_case_name()) + "." + std::string(test_info_->name())).get()

#define MODEL(K,A) \
new models::RealIntegral( \
    new models::Inercion( \
        new models::Engine(17.0, A) \
    , 1.0, 0.1124) \
, K, 0.1127)

#define MODEL_R MODEL(3.12, 57.5)
#define MODEL_L MODEL(2.44, 63.5)

#define MODE_NORMAL 0
#define MODE_PID_ROTATE 1
#define MODE_PID_MOVE 2
