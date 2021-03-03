#pragma once
#include "SPIprotocol_Ifc.hh"
#include <OwnExceptions.hh>

class SPIprotocol : public SPIprotocol_Ifc{
	uint16_t dataSize;
	uint8_t* dataTx;
	uint8_t* dataRx;
	bool bussy;
	bool used;
public:
    SPIprotocol():
    	dataSize(1),
	    dataTx(new uint8_t[dataSize]),
	    dataRx(new uint8_t[dataSize]),
	    bussy(false),
	    used(false){
        }
    virtual ~SPIprotocol(){
        delete [] dataTx;
        delete [] dataRx;
    }
    void setSize(const uint16_t& _size) override{
        delete [] dataTx;
        delete [] dataRx;
        dataSize = _size;
        dataTx = new uint8_t[dataSize]; 
        dataRx = new uint8_t[dataSize]; 
    }
    uint16_t size() override{
        return dataSize;
    }
    uint8_t* getTxData() override{
        return dataTx;
    }
    uint8_t* getRxData() override{
        return dataRx;
    }
    void markUsedData(){
        bussy = true;
        used = false;
    }
    void unmarkUsedData(){
        bussy = false;
    }
    bool isReadyToUse() override{
        return !(bussy || used);
    }
    void markUsed() override{
        used = true;
    }
};
