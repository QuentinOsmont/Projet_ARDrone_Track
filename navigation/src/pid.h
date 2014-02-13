#ifndef _PID_H_
# define _PID_H_

typedef struct  sPIDsystem
{
  double kP;
  double kI;
  double kD;
  double lastError;
  double errSum;

}               PIDSystem;

PIDSystem PID_Init(double kP, double kI, double kD);
double PID_Compute(PIDSystem *system, double error);

#endif
