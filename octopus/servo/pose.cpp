#include "pose.hpp"

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

Pose Pose::operator*(float scalar) const {

    return Pose{ first_left_right * scalar,
                    first_up_down * scalar,
                    second_left_right * scalar,
                    second_up_down * scalar};
}

Pose Pose::operator/(float scalar) const {

    return (*this) * (1.0F / scalar);
}

Pose& Pose::operator+=(const Pose& other) {

    first_left_right += other.first_left_right;
    first_up_down += other.first_up_down;
    second_left_right += other.second_left_right;
    second_up_down += other.second_up_down;

    return *this;
}





