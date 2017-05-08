#ifndef PID_H
#define PID_H

#include <iostream>

class PID {
public:
  /*
  * Errors
  */
  double p_error;
  double i_error;
  double d_error;
  double last_cte;
  double sum_cte;

  /*
  * Coefficients
  */ 
  double Kp;
  double Ki;
  double Kd;


    //The parameter to currently mess with
    bool initialized;
    int num_measurements;

  /*
  * Constructor
  */
  PID() {}

  /*
  * Destructor.
  */
  virtual ~PID();

    void Init(double Kp, double Ki, double Kd);
    void Init(double params[]);
  /*
  * Update the PID error variables given cross track error.
  */
  double UpdateError(double cte);

};

#endif /* PID_H */
