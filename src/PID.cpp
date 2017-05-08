#include "PID.h"

/*
* TODO: Complete the PID class.
*/

void PID::Init(double Kp, double Ki, double Kd) {
    this->Kp = Kp; // start by moving
    this->Ki = Ki;
    this->Kd = Kd;
    this->sum_cte = 0;
    this->initialized = false;
    this->num_measurements = 0;
}

void PID::Init(double params[]) {
    this->Kp = params[0]; // start by moving
    this->Ki = params[1];
    this->Kd = params[2];
    this->sum_cte = 0;
    this->initialized = false;
    this->num_measurements = 0;
}

PID::~PID() {

}


double PID::UpdateError(double cte) {
    if (!initialized) {
        last_cte = cte;
        initialized = true;
    }
    sum_cte += cte;
    p_error = -Kp*cte;
    i_error = Ki*sum_cte;
    d_error = Kd*(cte-last_cte);
    std::cout << "P" << p_error << std::endl;
    std::cout << "I" << i_error << std::endl;
    std::cout << "D" << d_error << std::endl;
    last_cte = cte;
    num_measurements ++;
    std::cout << "Num: " <<  num_measurements << std::endl;
    std::cout << "Mean: " <<  sum_cte / num_measurements << std::endl;
    return p_error - i_error - d_error;
}
