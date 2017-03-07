// Auto-generated. Do not edit.



    //% color=50 weight=80
declare namespace RotaryEncoder {

    /**
     * rotated.
     */
    //% blockId=rotary_ky_rotated_left_event
    //% block="on rotated |%dir" shim=RotaryEncoder::onRotateEvent
    function onRotateEvent(dir: RotationDirection, body: () => void): void;

    /**
     * button pressed.
     */
    //% blockId=rotary_ky_pressed_event
    //% block="on button pressed" shim=RotaryEncoder::onPressEvent
    function onPressEvent(body: () => void): void;

    /**
     * initialises local variablesssss
     */
    //% blockId=rotary_ky_init
    //% block="connect clk %clk|dt %dt|sw %sw"
    //% icon="\uf1ec" shim=RotaryEncoder::init
    function init(clk: DigitalPin, dt: DigitalPin, sw: DigitalPin): void;
}

// Auto-generated. Do not edit. Really.
