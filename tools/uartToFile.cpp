#include "uartToFile.hh"
#include <iostream>
#include <fstream>
#include <ctime>

namespace Tools{

std::shared_ptr<Drivers::UART_Ifc> UartLogFileManager::uartToFile(nullptr);

class UARTtoFile : public Drivers::UART_Ifc
{
    std::ofstream file;
public:
    UARTtoFile(const std::string& fileName);
    ~UARTtoFile();
    Drivers::BusStatus read(uint8_t *Data, uint16_t Size) override;
    Drivers::BusStatus write(uint8_t *Data, uint16_t Size) override;
    Drivers::BusStatus writeDMA(uint8_t *Data, uint16_t Size, DriverIfc* _callMe) override;
    Drivers::BusStatus writeDMAwithoutAlocate(uint8_t *Data, uint16_t Size, DriverIfc* _callMe) override;
    bool isBussy() override;
};

UartLogFileManager::UartLogFileManager(const std::string& fileName){
    if(uartToFile)
        throw std::invalid_argument("UARTtoFile is singleton. Could be created only once.");
    uartToFile.reset(new UARTtoFile(fileName));
}

UartLogFileManager::~UartLogFileManager(){}

std::shared_ptr<Drivers::UART_Ifc> UartLogFileManager::getUartInstance(const std::string& caseText){
    if(!uartToFile)
        throw std::invalid_argument("Uart driver to write to file is not created.");
    std::string txt("\n\n\n--------------------------------------------------# " + caseText + " #--------------------------------------------------\n");
    uartToFile->write((uint8_t*)txt.c_str(), txt.size());
    return uartToFile;
}

UARTtoFile::UARTtoFile(const std::string& fileName):
    Drivers::UART_Ifc([](uint32_t){}){
        file.open(fileName);
        time_t now = time(0);
        char* dt = ctime(&now);
        file << "File created in date: " << dt << std::endl;
    }

UARTtoFile::~UARTtoFile(){
    file.close();
}

Drivers::BusStatus UARTtoFile::read(uint8_t*, uint16_t){
    throw std::invalid_argument("Uart driver hasn't implemented read method.");
    return Drivers::BusStatus::OK;
}

Drivers::BusStatus UARTtoFile::write(uint8_t *Data, uint16_t Size){
    std::string txt((const char*)Data);
    file << txt.substr(0, Size) << std::flush;
    return Drivers::BusStatus::OK;
}

Drivers::BusStatus UARTtoFile::writeDMA(uint8_t *Data, uint16_t Size, DriverIfc*){
    return write(Data, Size);
}

Drivers::BusStatus UARTtoFile::writeDMAwithoutAlocate(uint8_t *Data, uint16_t Size, DriverIfc*){
    return write(Data, Size);
}

bool UARTtoFile::isBussy(){
    return false;
}

}

