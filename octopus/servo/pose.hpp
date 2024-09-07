#pragma once

struct Pose {

    float first_left_right = 0.0F;
    float first_up_down = 0.0F;
    float second_left_right = 0.0F;
    float second_up_down = 0.0F;

    Pose operator+(const Pose& other) const;
    Pose operator-(const Pose& other) const;
    Pose operator*(float scalar) const;
    Pose operator/(float scalar) const;
    Pose& operator+=(const Pose& other);

};
