/*
   Represents a group of pins directly on the arduino used to control outputs.
*/
#include "Group.h"

class PinGroup : public Group {
  private:
    const bool usePWM;
    const RelayState relayMode;
  public:
    PinGroup(const int channels, const int startingChannel, const bool usePWM, const int pins[], const RelayState relayMode)
      : Group(channels, startingChannel), usePWM(usePWM), relayMode(relayMode) {}

    virtual void start(){
      
    }
    
    virtual void update() {

    }
};
