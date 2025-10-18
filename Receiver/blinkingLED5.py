# https://pypi.org/project/gpiod/
import time
import  gpiod # sudo apt install python3-libgpiod
from gpiod.line import Direction, Value

LINE = 14 # get GPIO chips and lines => gpioinfo ; 


with gpiod.request_lines(
    "/dev/gpiochip4", # Open GPIO chip => ls /dev/gpiochip* ; gpiodetect;
    consumer="blinkingLED5",
    config={
        LINE: gpiod.LineSettings(
            direction=Direction.OUTPUT,
            output_value=Value.ACTIVE
        )
    },
) as request:
    while True:
        request.set_value(LINE, Value.ACTIVE)
        time.sleep(1)
        request.set_value(LINE, Value.INACTIVE)
        time.sleep(1)