#ifndef Group_h
#define Group_h

#include "Arduino.h"

/*
  Used to represent if a relay module is active high or active low.
  This is used for some of the group types.
*/
enum RelayState {
  ACTIVE_HIGH,
  ACTIVE_LOW
};

/*class RelayState {

  public:
    enum Value
    {
      ACTIVE_HIGH,
      ACTIVE_LOW
    };

  };*/

/*
   Group is the basic building block to a controller with multiple different devices
   that need to be controlled using different boards, but still share common functionality.
*/
class Group {
  private:
    const int channels;
    const int startingChannel;
  public:
    Group(const int channels, const int startingChannel)
      : channels(channels), startingChannel(startingChannel) {}
    ~Group();

    virtual void start();
    virtual void update();

};



#endif
