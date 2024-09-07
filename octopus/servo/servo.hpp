#pragma once

#include "motion.hpp"
#include "pose.hpp"
#include "servo2040.hpp"
//#include <vector>

class Servos {

    private:
        servo::ServoCluster servos;
        Pose start_and_end;
        static servo::ServoCluster init_cluster();
        void set_position(const Pose& next_position);

    public:

        Servos(const Pose& start_position); //constructor
        Servos(const Servos& other) = default; //copy constructor 
        Servos(Servos&& other) = default; //move constructor
        Servos& operator=(const Servos& other) = default; //copy assignment
        Servos& operator=(Servos&& other) = default; //move assignment

        void move(const Motion& motion);
        void to_mid();

        ~Servos();

};