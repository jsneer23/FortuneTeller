#pragma once

#include "servo2040.hpp"

struct Pose {

    double first_left_right = 0;
    double first_up_down = 0;
    double second_left_right = 0;
    double second_up_down = 0;

    Pose operator+(const Pose& other) const;
    Pose operator-(const Pose& other) const;
    Pose operator*(double scalar) const;
    Pose operator/(double scalar) const;
    Pose& operator+=(const Pose& other);

};

class Servos {

    private:
        servo::ServoCluster servos;
        servo::ServoCluster init_cluster();

    public:

        Servos(const Pose& start_position);

        void set_position(const Pose& next_position);
        void to_mid();

        ~Servos();

};