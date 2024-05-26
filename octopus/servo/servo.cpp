#include <cstdio>
#include "pico/stdlib.h"

#include "servo2040.hpp"
#include "button.hpp"
#include "analog.hpp"
#include "analogmux.hpp"

using namespace servo;
using namespace plasma;

// How many times to update Servos per second
const uint UPDATES = 50;

// The time to travel between each random value
const uint TIME_FOR_EACH_MOVE = 2;
const uint UPDATES_PER_MOVE = TIME_FOR_EACH_MOVE * UPDATES;

// How far from zero to move the servo
constexpr float SERVO_EXTENT = 130.0f;

// Whether or not to use a cosine path between values
const bool USE_COSINE = true;

// The brightness of the LEDs
constexpr float BRIGHTNESS = 0.4f;

// The maximum current, in amps, to show on the meter
constexpr float MAX_CURRENT = 2.1f;

// The number of current measurements to take per reading
const uint SAMPLES = 50;

// The time between each current measurement
const uint TIME_BETWEEN_MS = 1;

// Set up the shared analog inputs
Analog cur_adc = Analog(servo2040::SHARED_ADC, servo2040::CURRENT_GAIN,
                        servo2040::SHUNT_RESISTOR, servo2040::CURRENT_OFFSET);

// Set up the analog multiplexer, including the pin for controlling pull-up/pull-down
AnalogMux mux = AnalogMux(servo2040::ADC_ADDR_0, servo2040::ADC_ADDR_1, servo2040::ADC_ADDR_2,
                          PIN_UNUSED, servo2040::SHARED_ADC);

// Create the LED bar, using PIO 1 and State Machine 0
WS2812 led_bar(servo2040::NUM_LEDS, pio1, 0, servo2040::LED_DATA);

// Create the user button
Button user_sw = Button(servo2040::USER_SW);


int main() {
    stdio_init_all();
    
    // Create a servo object calibrated to goBilda servos
    Calibration go_bilda = Calibration();
    go_bilda.apply_two_pairs(500, 2500, -150, 150);
    Servo s = Servo(servo2040::SERVO_1, go_bilda);

    // Initialise the servo and led bar
    s.init();
    led_bar.start();

    // Get the initial value and create a random end value between the extents
    float start_value = s.mid_value();
    float end_value = (((float)rand() / (float)RAND_MAX) * (SERVO_EXTENT * 2.0f)) - SERVO_EXTENT;

    uint update = 0;

    // Continually move the servo until the user button is pressed
    while(!user_sw.raw()) {

        // Calculate how far along this movement to be
        float percent_along = (float)update / (float)UPDATES_PER_MOVE;

        if(USE_COSINE) {
            // Move the servo between values using cosine
            s.to_percent(cos(percent_along * (float)M_PI), 1.0, -1.0, start_value, end_value);
        }
        else {
            // Move the servo linearly between values
            s.to_percent(percent_along, 0.0, 1.0, start_value, end_value);
        }

        // Print out the value the servo is now at
        printf("Value = %f\n", s.value());

        // Move along in time
        update++;

        // Have we reached the end of this movement?
        if(update >= UPDATES_PER_MOVE) {
            // Reset the counter
            update = 0;

            // Set the start as the last end and create a new random end value
            start_value = end_value;
            end_value = (((float)rand() / (float)RAND_MAX) * (SERVO_EXTENT * 2.0f)) - SERVO_EXTENT;
        }

        // Select the current sense
        mux.select(servo2040::CURRENT_SENSE_ADDR);

        // Read the current sense several times and average the result
        float current = 0.0f;
        for(auto i = 0u; i < SAMPLES; i++) {
            current += cur_adc.read_current();
            sleep_ms(TIME_BETWEEN_MS);
        }
        current /= SAMPLES;

        // Print out the current sense value
        printf("Current = %f\n", current);

        // Convert the current to a percentage of the maximum we want to show
        float percent = (current / MAX_CURRENT);

        // Update all the LEDs
        for(auto i = 0u; i < servo2040::NUM_LEDS; i++) {
            // Calculate the LED's hue, with Red for high currents and Green for low
            float hue = (1.0f - (float)i / (float)(servo2040::NUM_LEDS - 1)) * 0.333f;

            // Calculate the current level the LED represents
            float level = (i + 0.5f) / servo2040::NUM_LEDS;
            // If the percent is above the level, light the LED, otherwise turn it off
            if(percent >= level) {
                led_bar.set_hsv(i, hue, 1.0f, BRIGHTNESS);
            }
            else {
                led_bar.set_hsv(i, hue, 1.0f, 0.0f);
            }
        }

        sleep_ms(1000 / UPDATES);
    }

    // Disable the servo and turn off the LED bar
    s.disable();
    led_bar.clear();

    // Sleep a short time so the clear takes effect
    sleep_ms(100);
}