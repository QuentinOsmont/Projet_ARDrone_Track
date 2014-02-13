#include "pid.h"


PIDSystem PID_Init(double kP, double kI, double kD)
{
  PIDSystem system;
  system.kP = kP;
  system.kI = kI;
  system.kD = kD;

  system.errSum = 0;
  system.lastError = 0;

  return system;
}

double PID_Compute(PIDSystem *system, double error)
{
  double errDif;

  double P, I, D;

  system->errSum += error;               // Errors Sum
  errDif = error - system->lastError;    // Error Variation
  system->lastError = error;             // Save Error

  P = error * system->kP;                 // P
  I = system->errSum * system->kI;        // I
  D = errDif * system->kD;                // D

  return (P + I + D);
}
