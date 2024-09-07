//#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "servo2040.hpp"
#include "button.hpp"
#include "analog.hpp"
#include "analogmux.hpp"

#include "pose.hpp"
#include "motion.hpp"
#include "servo.hpp"

#include <vector>

using namespace servo;
using namespace plasma;

// Create the user button for termination

void core1_current_sensing() {

    constexpr float BRIGHTNESS = 0.1F;      // led brightness
    constexpr float MAX_CURRENT = 10.0F;       // max  current in amps shown on meter
    const uint SAMPLES = 50;                                                              // current measurements to take per reading
    const uint TIME_BETWEEN_MS = 1;         // time between each current measurement

    // Initialize the led
    WS2812 led_bar(servo2040::NUM_LEDS, pio1, 0, servo2040::LED_DATA);  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)

    // Set up the shared analog inputs
    Analog cur_adc = Analog(servo2040::SHARED_ADC, servo2040::CURRENT_GAIN,
                        servo2040::SHUNT_RESISTOR, servo2040::CURRENT_OFFSET);

    // Set up the analog multiplexer, including the pin for controlling pull-up/pull-down
    AnalogMux mux = AnalogMux(servo2040::ADC_ADDR_0, servo2040::ADC_ADDR_1, servo2040::ADC_ADDR_2,
                          PIN_UNUSED, servo2040::SHARED_ADC);

    Button user_sw(servo2040::USER_SW);

    led_bar.start();

    while (!user_sw.raw()) {

        // Select the current sense
        mux.select(servo2040::CURRENT_SENSE_ADDR);

        // Read the current sense several times and average the result
        float current = 0.0F;
        for(auto i = 0U; i < SAMPLES; i++) {
            current += cur_adc.read_current();
            sleep_ms(TIME_BETWEEN_MS);
        }
        current /= SAMPLES;

        // Convert the current to a percentage of the maximum we want to show
        float percent = (current / MAX_CURRENT);

        // Update all the LEDs
        for(auto i = 0U; i < servo2040::NUM_LEDS; i++) {
            // Calculate the LED's hue, with Red for high currents and Green for low
            float hue = (1.0F - static_cast<float>(i) / static_cast<float>(servo2040::NUM_LEDS - 1)) * 0.333F;

            // Calculate the current level the LED represents
            float level = (static_cast<float>(i) + 0.5F) / servo2040::NUM_LEDS;
            // If the percent is above the level, light the LED, otherwise turn it off
            if(percent >= level) {
                led_bar.set_hsv(i, hue, 1.0F, BRIGHTNESS);
            }
            else {
                led_bar.set_hsv(i, hue, 1.0F, 0.0F);
            }
        }
    }

    led_bar.clear();
}



int main() {

    stdio_init_all();

    //Pose start {    0, -45,   0, -60};
    //Pose end   { -120, 20, -15, -60};
    Pose start {0,0,0,0};

    // pwm 0: upper left negative
    // pwm 1: lower left negative
    std::vector<Pose> initial_setpoints { {0,0,0,0}, {-130,110,-90, -110}, {70,60,-90, -110}, {70,-50,-90, -110}, {-30,20,40, -110}};


    Servos right(start);

    // spin up current sensing thread
    // THIS MUST HAPPEN AFTER STARTING SERVO CLUSTER
    // I DON'T KNOW WHY
    multicore_reset_core1();
    multicore_launch_core1(core1_current_sensing);

    sleep_ms(100);

    int resolution = 100;
    int speed = 50;

    Motion initial_motion(initial_setpoints, resolution, speed);
    //Motion tap_motion(tap_setpoints, resolution, speed+30);

    right.move(initial_motion);
 

}