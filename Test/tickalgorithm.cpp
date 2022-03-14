#include "tickalgorithm.h"
#include <QtTest/QtTest>

#include "../Plotlib/tickalgorithmbasic.h"
#include "../Mathlib/basicfunctions.h"
#include <QDebug>
void TickAlgorithm::testLinear_data()
{
  typedef QList<double> Res;
  QTest::addColumn<HEF::FunctionPtr>("function");
  QTest::addColumn<double>("min");
  QTest::addColumn<double>("max");
  QTest::addColumn<double>("distance");
  QTest::addColumn<double>("subdistance");
  QTest::addColumn< QList<double> >("expected");

//  QTest::newRow("Test1")<<(HEF::FunctionPtr)HEF::LinearFunction::newFromTwoPoints(-100, 0, 100, 110)<<0.<<110.<<50.<<4.<<Res({-100, 0, 100});
//  QTest::newRow("Test2")<<(HEF::FunctionPtr)HEF::LinearFunction::newFromTwoPoints(-100, 0, 100, 110)<<0.<<110.<<25.<<Res({-100, -50, 0, 50, 100});
//  QTest::newRow("Test3")<<(HEF::FunctionPtr)HEF::LinearFunction::newFromTwoPoints(-100, 0, 99.9, 110)<<0.<<110.<<25.<<Res({-100, -50, 0, 50});
//  QTest::newRow("Test4")<<(HEF::FunctionPtr)HEF::LinearFunction::newFromTwoPoints(0, 0, 99.9, 100)<<0.<<100.<<50.<<Res({0, 50});
//  QTest::newRow("Test5")<<(HEF::FunctionPtr)HEF::LinearFunction::newFromTwoPoints(0, 0, 99.9, 100)<<0.<<100.<<70.<<25.<<Res({0, 90});
//  QTest::newRow("Test6")<<(HEF::FunctionPtr)HEF::LinearFunction::newFromTwoPoints(0, 0, 99.9, 100)<<0.<<100.<<70.<<41.<<Res({0, 90});
//  QTest::newRow("Test6")<<(HEF::FunctionPtr)HEF::LinearFunction::newFromTwoPoints(0, 30, 100, 130)<<0.<<100.<<70.<<41.<<Res({0, 90});
//  QTest::newRow("Test1")<<(HEF::FunctionPtr)HEF::LinearFunction::newFromTwoPoints(0, 0, 100, 100)<<0.<<200.<<70.<<4.<<Res({-100, 0, 100});

//  QTest::newRow("Test1")<<(HEF::FunctionPtr)HEF::LinearFunction::newFromTwoPoints(-224, -224, 224, 224)<<-220.<<220.<<215.<<15.<<Res({-100, 0, 100}); // Sub ticks
//  QTest::newRow("Test1")<<(HEF::FunctionPtr)HEF::LinearFunction::newFromTwoPoints(-224, -224, 224, 224)<<-220.<<220.<<215.<<40.<<Res({-220, 0, 220}); // Sub tick not working
//  QTest::newRow("Test1")<<(HEF::FunctionPtr)HEF::LinearFunction::newFromTwoPoints(-224, -448, 224, 448)<<-440.<<440.<<430.<<80.<<Res({-220, 0, 220}); // Sub tick not working
//  QTest::newRow("Test1")<<(HEF::FunctionPtr)HEF::LinearFunction::newFromTwoPoints(-224, 0, 224, 896)<<-440.<<440.<<430.<<80.<<Res({0, 220, 440}); // Sub tick not working

//  QTest::newRow("TestLog1")<<(HEF::FunctionPtr)HEF::LogarithmicFunction::newFromTwoPoints(10, 0, 1000, 100)<<0.<<100.<<45.<<qQNaN()<<Res({0, 220, 440}); // Sub tick not working
//  QTest::newRow("TestLog1")<<(HEF::FunctionPtr)HEF::LogarithmicFunction::newFromTwoPoints(100, 0, 10000, 100)<<0.<<100.<<45.<<qQNaN()<<Res({0, 220, 440}); // Sub tick not working

//  QTest::newRow("TestLog1")<<(HEF::FunctionPtr)HEF::LogarithmicFunction::newFromTwoPoints(100, 0, 1000, 100)<<-100.<<0.<<30.<<qQNaN()<<Res({0, 220, 440}); // Can divide 100..500...1000
//  QTest::newRow("TestLog1")<<(HEF::FunctionPtr)HEF::LogarithmicFunction::newFromTwoPoints(100, 0, 2000000, 100)<<-100.<<0.<<30.<<qQNaN()<<Res({0, 220, 440}); // Can divide 100..5000...10000

//  QTest::newRow("TestLog1")<<(HEF::FunctionPtr)HEF::LogarithmicFunction::newFromTwoPoints(100, 0, 140, 100)<<-100.<<0.<<30.<<qQNaN()<<Res({0, 220, 440}); // Can divide 100..200..300..400..500..600..700..800..900

//  QTest::newRow("TestLog1")<<(HEF::FunctionPtr)HEF::LogarithmicFunction::newFromTwoPoints(100, 0, 140, 100)<<-100.<<0.<<30.<<qQNaN()<<Res({0, 220, 440}); // Can divide 100..200..300..400..500..600..700..800..900
//  QTest::newRow("TestLog1")<<(HEF::FunctionPtr)HEF::LogarithmicFunction::newFromTwoPoints(100, 0, 115, 100)<<-100.<<0.<<30.<<qQNaN()<<Res({0, 220, 440}); // Can divide 100..150..200..250.. 900..950..
  QTest::newRow("TestLog1")<<(HEF::FunctionPtr)HEF::LogarithmicFunction::newFromTwoPoints(100, 0, 101.5, 100)<<-100.<<0.<<30.<<qQNaN()<<Res({0, 220, 440}); // Can divide 100..105..110..115.. 990..995..

  //  QTest::newRow("TestLog1")<<(HEF::FunctionPtr)HEF::LogarithmicFunction::newFromTwoPoints(100, 0, 1000000, 100)<<-100.<<0.<<45.<<qQNaN()<<Res({0, 220, 440}); // Sub tick not working
}

static QString listToString(const QList<double> &l)
{
  // Std::transform?
  QStringList ret;
  for(double x: l)
    ret.append(QString::number(x));
  return ret.join(", ");
}
void TickAlgorithm::testLinear()
{
  QFETCH(HEF::FunctionPtr, function);
  QFETCH(double, min);
  QFETCH(double, max);
  QFETCH(double, distance);
  QFETCH(double, subdistance);
  QFETCH(QList<double>, expected);
  auto exp=expected;
  TickAlgorithmBasic alg(function, min, max);
  alg.setMinTickDistance(distance, subdistance);
  alg.recalculate();
  auto res=alg.mainTicks().keys();
  for(int i=0;i<res.size();i++)
  {
    double x=res[i], y=alg.mainTicks()[x], realY=function->f(x);
    if(!(realY==y))
      QFAIL(QString("Returned value miscalculated f(%1) should be %3 instead it is %2").arg(x).arg(y).arg(realY).toUtf8());
    bool found=false;
    for(int i=0;i<exp.size();i++)
    {
      auto e=exp[i];
      if((e==0. && x<1e-9) || (e!=0. && qAbs((x-e)/e)<1e-5))
      {
        found=true;
        exp.removeAt(i);
        break;
      }
    }
    if(!found)
      QFAIL(QString("Returned unexpected value %1 (Wanted: %2)").arg(x).arg(listToString(expected)).toUtf8());
  }
  if(!exp.isEmpty())
    QFAIL(QString("Not returned expected value %1 (Returned: %2)").arg(exp.first()).arg(listToString(res)).toUtf8());
}
