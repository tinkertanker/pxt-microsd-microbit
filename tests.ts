// tests go here; this will not be compiled when this package is used as a library

let item = 0
basic.forever(() => {
    basic.showNumber(item)
})
RotaryEncoder.onPressEvent(() => {
    item = 0
})
RotaryEncoder.onRotateEvent(RotationDirection.Right, () => {
    item += -1
})
RotaryEncoder.onRotateEvent(RotationDirection.Left, () => {
    item += 1
})
item = 0
RotaryEncoder.init(DigitalPin.P14, DigitalPin.P15, DigitalPin.P16)
basic.showNumber(0)

while(true) {
	
}