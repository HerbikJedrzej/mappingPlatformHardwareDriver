#pragma once

#include <cstddef>
#include <cstdint>
#include <stdint.h>
#include <cstdlib>
#include <Log.hh>

struct BatteryConfiguration{
	const uint8_t numberOfCells = {0};
	uint32_t* upperResistors_Ohm = {nullptr};
	uint32_t* lowerResistors_Ohm = {nullptr};
	uint32_t adcMaxWord = {0xffff};
	double maxMeasuredVoltage = {3.3};
};

class BatteryObserver{
public:
	BatteryObserver(PlatformDrivers::Log& _log, uint16_t const* _measurements, const BatteryConfiguration& _batteryConfiguration);
	~BatteryObserver();
	double batteryVoltageVal() const;
	double batteryVoltageVal(const uint8_t& cell) const;
	void countUpperResistor(const uint8_t& cell, const double& referenceVoltage) const;
private:
	double absoluteBatteryVoltageVal(const uint8_t& cell) const;
	PlatformDrivers::Log& log;
	uint16_t const* measurements;
	const BatteryConfiguration& config;
};
