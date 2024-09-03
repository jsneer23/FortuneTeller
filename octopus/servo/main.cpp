#include <stdio.h>

//#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "servo2040.hpp"
#include "button.hpp"
#include "analog.hpp"
#include "analogmux.hpp"

#include "servo.hpp"

using namespace servo;
using namespace plasma;

// Create the user button for termination
Button user_sw = Button(servo2040::USER_SW);

void core1_current_sensing() {

    constexpr float BRIGHTNESS = 0.1f;      // led brightness
    constexpr float MAX_CURRENT = 10.0f;       // max  current in amps shown on meter
    const uint SAMPLES = 50;                                                              // current measurements to take per reading
    const uint TIME_BETWEEN_MS = 1;         // time between each current measurement

    // Initialize the led
    WS2812 led_bar(servo2040::NUM_LEDS, pio1, 0, servo2040::LED_DATA);

    // Set up the shared analog inputs
    Analog cur_adc = Analog(servo2040::SHARED_ADC, servo2040::CURRENT_GAIN,
                        servo2040::SHUNT_RESISTOR, servo2040::CURRENT_OFFSET);

    // Set up the analog multiplexer, including the pin for controlling pull-up/pull-down
    AnalogMux mux = AnalogMux(servo2040::ADC_ADDR_0, servo2040::ADC_ADDR_1, servo2040::ADC_ADDR_2,
                          PIN_UNUSED, servo2040::SHARED_ADC);

    led_bar.start();

    while (!user_sw.raw()) {

        // Select the current sense
        mux.select(servo2040::CURRENT_SENSE_ADDR);

        // Read the current sense several times and average the result
        float current = 0.0f;
        for(auto i = 0u; i < SAMPLES; i++) {
            current += cur_adc.read_current();
            sleep_ms(TIME_BETWEEN_MS);
        }
        current /= SAMPLES;

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
    }

    led_bar.clear();
}



int main() {

    stdio_init_all();

    Pose start {    0, -45,   0, -60};
    Pose end   { -120, 20, -15, -60};

    Servos servo_cluster(start);

    // spin up current sensing thread
    // THIS MUST HAPPEN AFTER STARTING SERVO CLUSTER
    // I DON'T KNOW WHY
    multicore_reset_core1();
    multicore_launch_core1(core1_current_sensing);

    sleep_ms(1000);
    servo_cluster.set_position(end);

    int resolution = 100;
    int speed = 40;

    Pose delta = (end - start) / resolution;

        for(int i = 0; i <= resolution; i++){
            //start += delta;

            //set servo positions
            servo_cluster.set_position(start + delta * i);
            sleep_ms(1000/speed);
        }

    sleep_ms(2000);

}