#include "servo.hpp"
#include "motion.hpp"
#include "pose.hpp"
#include "servo2040.hpp"
#include <vector>

servo::ServoCluster Servos::init_cluster(){

    const uint START_PIN = servo::servo2040::SERVO_1;          // first servo in cluster
    const uint END_PIN = servo::servo2040::SERVO_4;            // last servo in cluster
    const uint NUM_SERVOS = (END_PIN - START_PIN) + 1;  // number of servos in cluster

    servo::Calibration calibration;
    calibration.apply_two_pairs(500, 2500, -150, 150);
    
    return {pio0, 0, START_PIN, NUM_SERVOS, calibration};
}

void Servos::set_position(const Pose& next_position) {

    //set servo positions
    servos.value(0, next_position.first_left_right, false);
    servos.value(1, next_position.first_up_down, false);
    servos.value(2, next_position.second_left_right, false);
    servos.value(3, next_position.second_up_down, false);

    servos.load();
}

Servos::Servos(const Pose& position) : servos(init_cluster()), start_and_end(position) {

    servos.init();
    servos.enable_all();
    set_position(start_and_end);
    sleep_ms(200);
}

void Servos::move(const Motion& motion){

    for(int i = 0; i <= motion.size(); i++) {
        set_position(motion.at(i));
        sleep_ms(motion.wait_ms());
    }

}

void Servos::to_mid() {
    //servos.all_to_mid();
}

Servos::~Servos() {

    //set_position(start_and_end);
    //sleep_ms(1000);
    //servos.disable_all(false);
    //sleep_ms(1000);
}