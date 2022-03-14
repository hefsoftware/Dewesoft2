#include "tests.h"
#include "tickalgorithm.h"
#include <QtTest/QtTest>


void runTests()
{
  TickAlgorithm alg;
  QTest::qExec(&alg);
}
