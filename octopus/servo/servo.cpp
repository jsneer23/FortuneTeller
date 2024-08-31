#include <stdio.h>
#include <math.h>
#include <vector>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "servo2040.hpp"
#include "button.hpp"
#include "analog.hpp"
#include "analogmux.hpp"

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

void start_and_end(ServoCluster& servos) {

    // start and end position
    servos.value(0, 0, false);
    servos.value(1, -45, false);
    servos.value(2, 0, false);
    servos.value(3, -60, false);
    servos.load();

}

std::vector<std::vector<double>> generate_path(const std::vector<int>& start, const std::vector<int>& end, int num_steps){

    std::vector<double> step_size;
    std::vector<double> curr;
    std::vector<std::vector<double>> result;

    for(int i = 0; i < 4; i++){
        double temp = (double(end[i]) - double(start[i])) / double(num_steps);
        step_size.push_back(temp);
        curr.push_back(double(start[i]));
    }

    for(int i = 0; i <= num_steps; i++){
        std::vector<double> copy;

        for(auto& item : curr) {
            copy.push_back(item);
        }

        result.push_back(copy);

        for(int j = 0; j < 4; j++) {
            curr[j] += step_size[j];
        }
    }

    return result;

}


int main() {

    const uint UPDATES = 50;                            // number of servo updates per second
    const uint START_PIN = servo2040::SERVO_1;          // first servo in cluster
    const uint END_PIN = servo2040::SERVO_4;            // last servo in cluster
    const uint NUM_SERVOS = (END_PIN - START_PIN) + 1;  // number of servos in cluster
    
    stdio_init_all();

    // Initialize the servos
    Calibration go_bilda = Calibration();               // create calibration object
    go_bilda.apply_two_pairs(500, 2500, -150, 150);     // set info for goBilda servos
    ServoCluster servos = ServoCluster(pio0, 0, START_PIN, NUM_SERVOS, go_bilda);   //init

    // generate thread for current sensing
    multicore_reset_core1();
    multicore_launch_core1(core1_current_sensing);

    // Enable all servos (this puts them at the middle)
    servos.init();

    start_and_end(servos);
    sleep_ms(2000);

    std::vector<int> start {    0, -45,   0, -60};
    std::vector<int> end   { -120, 20, -15, -60};
    std::vector<std::vector<double>> poses = generate_path(start, end, 100);

    int speed = 60;

    for(auto& pose : poses) {
        for(int i = 0; i < 4; i++){
            servos.value(i, pose[i], false);
        }
        servos.load();
        sleep_ms(1000/speed);

    }

    sleep_ms(2000);

    start_and_end(servos);
    sleep_ms(2000);

    servos.all_to_mid();
    sleep_ms(2000);

    // Disable the servo
    servos.disable_all();

    // Sleep a short time so the clear takes effect
    sleep_ms(100);
}