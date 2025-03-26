#proximity.py
#must be named code.py on board (CircuitPython)

# This demo uses a passive buzzer, programmable neopixel (RGB LED), and a HCSR04 sonar sensor.

# As you move your hand closer, the sensor detects the distance, the microcontroller
# chooses a color for the LED and makes it more intense depending on how close you get.
# At the same time, you will hear the buzzer's frequency will change depending on how close you are.

import board, random, time, pwmio
import neopixel as np
import adafruit_hcsr04 as hcsr

buzzer = pwmio.PWMOut(board.D5, variable_frequency=True)
pixel = np.NeoPixel(board.NEOPIXEL, n=1)
sonar = hcsr.HCSR04(trigger_pin=board.SCL, echo_pin=board.SDA)

colors = [(255, 0, 0), (255, 150, 0), (0, 255, 0), (0, 255, 255), (0, 0, 255), (180, 0, 255), (255, 255, 255)]
curr_color = None
last_brightness = 0
MIN_FREQ = 10
MAX_FREQ = 5000

while True:
    try:
        dist = sonar.distance
        brightness = int(255/(dist/1.5))
    except RuntimeError:
        distance = None
        brightness = last_brightness
        
    if brightness < 10: #technically never 0, establish min threshold
        brightness = 0
        
    if brightness > 0 and curr_color is None:
        curr_color = random.choice(colors)
        
    if brightness == 0:
        curr_color = None
        
    if curr_color is not None:
        pixel[0] = tuple(int(c * brightness / 255) for c in curr_color)
        buzzer.frequency = min(MAX_FREQ, max(MIN_FREQ, 15*brightness))
        buzzer.duty_cycle = 32768
    else:
        pixel[0] = (0, 0, 0)
        buzzer.duty_cycle = 0
        
    last_brightness = brightness
    time.sleep(0.01)
