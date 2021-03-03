#pragma once
#include <UART_Ifc.hh>
#include <stdio.h>
#include <cstdarg>
#include <OwnExceptions.hh>

namespace PlatformDrivers{

enum LogingLevel{
    noneLogs = 0,
    errorLogs,
    warningLogs,
    infoLogs
};

class Log{
    Drivers::UART_Ifc* const uart;
    const LogingLevel level;
    const unsigned int maxDataBuffor;
    char* data;
    const unsigned int& timer;
public:
	Log(Drivers::UART_Ifc* const _uart, const LogingLevel _level, const unsigned int& _timer, const unsigned int maxDataBufforSize):
        uart(_uart),
        level(_level),
        maxDataBuffor(maxDataBufforSize),
        data(new char[maxDataBuffor]),
        timer(_timer){}
    ~Log(){
        delete [] data;
    }
    template <typename... Types>
	void info(const char* txt, Types... var){
        if(level < LogingLevel::infoLogs)
            return;
        uint16_t size = sprintf(data, "I%5u: ", timer); 
        size += sprintf(&data[size], txt, var...);
        data[size++] = '\n';
        if(size > maxDataBuffor)
            THROW_out_of_range("Info log text is bigger than buffor.");
        uart->writeDMA((uint8_t*)data, size, nullptr); 
    }
    template <typename... Types> 
    void warning(const char* txt, Types... var){
        if(level < LogingLevel::warningLogs)
            return;
        uint16_t size = sprintf(data, "W%5u: ", timer); 
        size += sprintf(&data[size], txt, var...);
        data[size++] = '\n';
        if(size > maxDataBuffor)
            THROW_out_of_range("Warning log text is bigger than buffor.");
        uart->writeDMA((uint8_t*)data, size, nullptr); 
    }
    template <typename... Types> 
    void error(const char* txt, Types... var){
        if(level < LogingLevel::errorLogs)
            return;
        uint16_t size = sprintf(data, "E%5u: ", timer); 
        size += sprintf(&data[size], txt, var...);
        data[size++] = '\n';
        if(size > maxDataBuffor)
            THROW_out_of_range("Error log text is bigger than buffor.");
        uart->writeDMA((uint8_t*)data, size, nullptr); 
    }
	void info(const char* txt){
        if(level < LogingLevel::infoLogs)
            return;
        uint16_t size = sprintf(data, "I%5u: %s", timer, txt); 
        data[size++] = '\n';
        if(size > maxDataBuffor)
            THROW_out_of_range("Info log text is bigger than buffor.");
        uart->writeDMA((uint8_t*)data, size, nullptr); 
    }
    template <typename... Types> 
    void warning(const char* txt){
        if(level < LogingLevel::warningLogs)
            return;
        uint16_t size = sprintf(data, "W%5u: %s", timer, txt); 
        data[size++] = '\n';
        if(size > maxDataBuffor)
            THROW_out_of_range("Warning log text is bigger than buffor.");
        uart->writeDMA((uint8_t*)data, size, nullptr); 
    }
    template <typename... Types> 
    void error(const char* txt){
        if(level < LogingLevel::errorLogs)
            return;
        uint16_t size = sprintf(data, "E%5u: %s", timer, txt); 
        data[size++] = '\n';
        if(size > maxDataBuffor)
            THROW_out_of_range("Error log text is bigger than buffor.");
        uart->writeDMA((uint8_t*)data, size, nullptr); 
    }
};

}
