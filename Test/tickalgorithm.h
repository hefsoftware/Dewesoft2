#ifndef TICKALGORITHM_H
#define TICKALGORITHM_H
#include "../Mathlib/function.h"
class TickAlgorithm: public QObject
{
  Q_OBJECT
  struct Test
  {
    Test(HEF::FunctionPtr f, double min, double max, double tickDistance): f(f), min(min), max(max), tickDistance(tickDistance) { }
    HEF::FunctionPtr f;
    double min;
    double max;
    double tickDistance;
  };
private slots:
  void testLinear_data();
  void testLinear();
};

#endif // TICKALGORITHM_H
