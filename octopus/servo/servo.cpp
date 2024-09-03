#include "servo.hpp"

#include "servo2040.hpp"

Pose Pose::operator+(const Pose& other) const {

        return Pose{ first_left_right + other.first_left_right,
                     first_up_down + other.first_up_down,
                     second_left_right + other.second_left_right,
                     second_up_down + other.second_up_down};
    } 

Pose Pose::operator-(const Pose& other) const {

    return Pose{ first_left_right - other.first_left_right,
                    first_up_down - other.first_up_down,
                    second_left_right - other.second_left_right,
                    second_up_down - other.second_up_down};
}

Pose Pose::operator*(double scalar) const {

    return Pose{ first_left_right * scalar,
                    first_up_down * scalar,
                    second_left_right * scalar,
                    second_up_down * scalar};
}

Pose Pose::operator/(double scalar) const {

    return (*this) * (1.0 / scalar);
}

Pose& Pose::operator+=(const Pose& other) {

    first_left_right += other.first_left_right;
    first_up_down += other.first_up_down;
    second_left_right += other.second_left_right;
    second_up_down += other.second_up_down;

    return *this;
}

servo::ServoCluster Servos::init_cluster(){

    const uint START_PIN = servo::servo2040::SERVO_1;          // first servo in cluster
    const uint END_PIN = servo::servo2040::SERVO_4;            // last servo in cluster
    const uint NUM_SERVOS = (END_PIN - START_PIN) + 1;  // number of servos in cluster

    servo::Calibration calibration;
    calibration.apply_two_pairs(500, 2500, -150, 150);
    
    return servo::ServoCluster(pio0, 0, START_PIN, NUM_SERVOS, calibration);
}

Servos::Servos(const Pose& start_position) : servos(init_cluster()) {

    servos.init();
    //to_mid();
    servos.enable_all();
    set_position(start_position);
    //sleep_ms(1000);
}

void Servos::set_position(const Pose& next_position) {

    //set servo positions
    servos.value(0, next_position.first_left_right, false);
    servos.value(1, next_position.first_up_down, false);
    servos.value(2, next_position.second_left_right, false);
    servos.value(3, next_position.second_up_down, false);

    servos.load();
}

void Servos::to_mid() {
    servos.all_to_mid();
}

Servos::~Servos() {

    to_mid();

    servos.disable_all();
    sleep_ms(100);
}