#include "BatteryObserver.hh"
#include <OwnExceptions.hh>

BatteryObserver::BatteryObserver(PlatformDrivers::Log& _log, uint16_t const* _measurements, const BatteryConfiguration& _batteryConfiguration):
	log(_log),
	measurements(_measurements),
	config(_batteryConfiguration){
		for(uint8_t i = 0; i < config.numberOfCells; i++)
			if(config.lowerResistors_Ohm[i] == 0)
				log.warning("Lower resistor R%d cannot be 0.", i + 1);
	}

BatteryObserver::~BatteryObserver(){}

double BatteryObserver::absoluteBatteryVoltageVal(const uint8_t& cell) const{
	const double r1 = config.upperResistors_Ohm[cell];
	const double r2 = config.lowerResistors_Ohm[cell];
	return
		double(measurements[cell]) / double(config.adcMaxWord) * config.maxMeasuredVoltage
			* (r1 + r2) / r2;
}

double BatteryObserver::batteryVoltageVal(const uint8_t& cell) const{
	if(cell >= config.numberOfCells)
		THROW_out_of_range("Trying to get voltage of cell out out of avaibles.");
	if(cell == 0)
		return absoluteBatteryVoltageVal(cell);
	return absoluteBatteryVoltageVal(cell) - absoluteBatteryVoltageVal(cell - 1);
}

double BatteryObserver::batteryVoltageVal() const{
	return absoluteBatteryVoltageVal(config.numberOfCells - 1);
}

void BatteryObserver::countUpperResistor(const uint8_t& cell, const double& referenceVoltage) const{
	if(cell >= config.numberOfCells)
		THROW_out_of_range("Trying to tune cell resistor out of avaibles.");
	const double adc = measurements[cell];
	const double r2 = config.lowerResistors_Ohm[cell];
	log.warning("Put this value for upper battery resistor %d in json:", cell + 1);
	log.warning("R: %d [Ohm]", 
		(uint32_t)(r2 * (referenceVoltage * double(config.adcMaxWord) / (adc * config.maxMeasuredVoltage) - 1.0))
	);
}
