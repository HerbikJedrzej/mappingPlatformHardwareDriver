#pragma once

#include <UART_Ifc.hh>
#include <memory>

namespace Tools{

class UartLogFileManager
{
    static std::shared_ptr<Drivers::UART_Ifc> uartToFile;
public:
    UartLogFileManager(const std::string& fileName);
    ~UartLogFileManager();
    static std::shared_ptr<Drivers::UART_Ifc> getUartInstance(const std::string& caseText);
};

}