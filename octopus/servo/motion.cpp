#include "motion.hpp"
#include "pose.hpp"
#include <vector>


Motion::Motion(const std::vector<Pose>& base_path, int resolution, int speed, bool spline) : speed(speed) {

     if (base_path.size() == 0) {
        return;
    }

    if (spline) {
        // implement spline path
    }
    else {
        for(int i = 1; i < base_path.size(); i++) {

            const Pose& end = base_path[i];
            const Pose& start = base_path[i-1];

            Pose curr = start;
            Pose delta = (end - start) / static_cast<float>(resolution);

            for(int j = 0; j <= resolution; j++) {
                curr += delta;
                path.push_back(curr);
            }

        }
    }
}

int Motion::size() const{
    return path.size();
}

const Pose& Motion::at(int i) const {
    return path[i];
}

int Motion::wait_ms() const {
    return 1000/speed;
}

