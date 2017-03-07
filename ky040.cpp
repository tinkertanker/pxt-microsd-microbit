#include "pxt.h"
#define ROTATOR 100
#define LEFTR 1
#define RIGHTR 2
#define PRESS 3
using namespace pxt;

enum class DigitalPin{ 

}; //defined in pxt core

enum class RotationDirection{
  Left = 0,
  Right = 1
};

//% color=50 weight=80
namespace RotaryEncoder {

  class InternalButton : public MicroBitComponent{
  private:
    DigitalIn *pinReader;
    MicroBitEvent *eventFire;
    bool lastState;
    Timer *tsb;
    uint32_t lastPress = 0;
  public:
    InternalButton(PinName pin, MicroBitEvent* evt, Timer *timer){
      this -> pinReader = new DigitalIn(pin);
      this -> id = ROTATOR;
      this -> status = 0;
      this -> eventFire = evt;
      this -> lastState = false;
      this -> tsb = timer;
    }
    void systemTick(){
      if(pinReader->read()) {lastState = false; return;}
      uint32_t now = tsb->read_ms();
      if(now - lastPress < 50) return;
      lastPress = now;
      if(lastState) return;
      lastState = true;
      eventFire->fire();
    }
    ~InternalButton(){
      system_timer_remove_component(this);
    }
  };


  uint32_t lastRotateInterrupt = 0;

  //GOTTA GO FAST!
  InterruptIn *rotaryInterrupt;
  DigitalIn *dataValue;
  MicroBitEvent *leftRotateEvt, *rightRotateEvt, *pressEvt;
  InternalButton *btn;

  //software debouncing stuff
  Timer tsb;

  vector<Action> leftRotate, rightRotate, pressRotate;

  /**
  * rotated.
  */
  //% blockId=rotary_ky_rotated_left_event
  //% block="on rotated |%dir"
  void onRotateEvent(RotationDirection dir, Action body) {
    //add handlers
    if(dir == RotationDirection::Left) leftRotate.push_back(body);
    else rightRotate.push_back(body);
  }

  /**
  * button pressed.
  */
  //% blockId=rotary_ky_pressed_event
  //% block="on button pressed"
  void onPressEvent(Action body){
    pressRotate.push_back(body);
  }

  //runs stuff

  void checkAction(vector<Action> runner){
    for(int i=0;i<runner.size();i++){
      runAction0(runner[i]); //wth is runaction0
    }
  }

  //event handlers

  void onLeftRotate(MicroBitEvent){
    checkAction(leftRotate);
  }

  void onRightRotate(MicroBitEvent){
    checkAction(rightRotate);
  }

  void onPress(MicroBitEvent){
    checkAction(pressRotate);
  }

  //interrupt handlers

  void onRotated(){
    uint32_t now = tsb.read_ms();
    if(now - lastRotateInterrupt < 50) return;
    lastRotateInterrupt = now;
    if(dataValue->read()) leftRotateEvt->fire();//fire left rotate
    else rightRotateEvt->fire(); //fire right rotate
  }


  /**
  * initialises local variablesssss
  */
  //% blockId=rotary_ky_init
  //% block="connect clk %clk|dt %dt|sw %sw"
  //% icon="\uf1ec"
  void init(DigitalPin clk, DigitalPin dt, DigitalPin sw){

    rotaryInterrupt = new InterruptIn((PinName)clk); //wtfffffffffff
    dataValue = new DigitalIn((PinName)dt);

    leftRotateEvt = new MicroBitEvent(ROTATOR, LEFTR, CREATE_ONLY);
    rightRotateEvt = new MicroBitEvent(ROTATOR, RIGHTR, CREATE_ONLY);
    pressEvt = new MicroBitEvent(ROTATOR, PRESS, CREATE_ONLY);

    btn = new InternalButton((PinName)sw, pressEvt, &tsb);
    system_timer_add_component(btn);

    uBit.messageBus.listen(ROTATOR, LEFTR, onLeftRotate, MESSAGE_BUS_LISTENER_IMMEDIATE);
    uBit.messageBus.listen(ROTATOR, RIGHTR, onRightRotate, MESSAGE_BUS_LISTENER_IMMEDIATE);
    uBit.messageBus.listen(ROTATOR, PRESS, onPress, MESSAGE_BUS_LISTENER_IMMEDIATE);

    tsb.start(); //interrupt timer for debounce
    rotaryInterrupt->fall(&onRotated);
  }
}