#pragma once

#include "pose.hpp"
#include <vector>

class Motion {
    private:
        std::vector<Pose> path;
        int speed;

    public:
        Motion(const std::vector<Pose>& path, int resolution, int speed, bool spline = false);

        [[nodiscard]] int size() const;
        [[nodiscard]] const Pose& at(int i) const;
        [[nodiscard]] int wait_ms() const;

};