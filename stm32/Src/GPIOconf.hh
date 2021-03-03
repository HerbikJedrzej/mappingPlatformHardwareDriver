#ifndef GPIOCONF_H
#define GPIOCONF_H

enum OutputList{
    none = -1,
    Trig1 = 43,
    Trig2 = 37,
    Trig3 = 40,
    LED_GREEN = 12,
    LED_BLUE = 11,
    LED_YELLOW = 36,
    LED_RED = 30,
    EngineLeftForward = 46,
    EngineLeftBackward = 47,
    EngineRightForward = 2,
    EngineRightBackward = 3,
    StmReady = 17,
};

enum InputList{
  MemorySelect = 28,
  Echo1 = 44,
  Echo2 = 16,
  Echo3 = 41,
};

enum InterruptInputList{
  EchoIrq1 = 0,
  EchoIrq2,
  EchoIrq3,
  MPU_Irq,
  SS,
  endOfInterruptInputList
};

enum ADClist{
  cell1 = 0,
  cell2,
  end
};

#endif
