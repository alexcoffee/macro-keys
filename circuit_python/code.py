# this script needs to be uploaded to CIRCUITPY mass storage device
import time
import board
import digitalio
import usb_hid
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode

kbd = Keyboard(usb_hid.devices)


button_a = digitalio.DigitalInOut(board.GP12)
button_a.direction = digitalio.Direction.INPUT
button_a.pull = digitalio.Pull.UP

button_b = digitalio.DigitalInOut(board.GP15)
button_b.direction = digitalio.Direction.INPUT
button_b.pull = digitalio.Pull.UP


# GP13 gets shorted with GP12 when 3.5mm plug is inserted.
button2 = digitalio.DigitalInOut(board.GP13)
button2.direction = digitalio.Direction.INPUT
button2.pull = digitalio.Pull.UP

# GP14 gets shorted with GP15 when 3.5mm plug is inserted.
button3 = digitalio.DigitalInOut(board.GP14)
button3.direction = digitalio.Direction.INPUT
button3.pull = digitalio.Pull.UP


while True:
    if button_a.value == 0:
        kbd.send(Keycode.CONTROL, Keycode.A)
        time.sleep(0.2)
    if button_b.value == 0:
        kbd.send(Keycode.CONTROL, Keycode.B)
        time.sleep(0.2)