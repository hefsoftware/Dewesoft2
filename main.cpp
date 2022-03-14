#include "mainwindow.h"

#include <QApplication>
//#include "Mathlib/basicfunctions.h"
#include "Mathlib/range.h"
#include <QDebug>
#include "Plotlib/tickalgorithmbasic.h"
#include "Test/tests.h"
int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

//  runTests();
  MainWindow w;
  w.show();
//  TickAlgorithmBasic alg();
//  alg.recalculate();
//  auto f=HEF::LinearFunction::newFromTwoPoints(4.3, 15.0, 7.4, 11.3);
//  auto f2=HEF::LogarithmicFunction::newDecimal();
//  auto f1=HEF::LinearFunction::newFromTwoPoints(4.3,1.0,5.3,3.0);
//  auto f2=HEF::LogarithmicFunction::newFromTwoPoints(32, 4.3, 3200, 5.3);
//  auto f3=HEF::LinearFunction::newFromGainOffset(1/3.2, 5.);
//  qDebug()<<"Comp1";
//  auto comp1=HEF::FunctionCompositionMulti::compose({f1,f2});
//  qDebug()<<"Comp2";
//  auto comp2=HEF::FunctionCompositionMulti::composeDumb({f3});
//  qDebug()<<"Comp1&2";
//  auto comp3=HEF::FunctionCompositionMulti::compose({comp1, comp2});
//  qDebug()<<"Done"<<comp3->toString();
//  qDebug()<<"Other"<<f1->fOf(f2->fOf(f3))->toString();
//  HEF::LogarithmicFunction::newFromTwoPoints(32, 4.3, 3200, 5.3).fOf()
//  auto fList=HEF::FunctionList({HEF::LinearFunction::newFromTwoPoints(4.3,1.0,5.3,3.0),f2, HEF::LinearFunction::newFromGainOffset(1/3.2, 5.)});
//  auto compDumb=HEF::FunctionCompositionMulti::composeDumb(fList);
//  auto compSmart=HEF::FunctionCompositionMulti::compose(fList);
//  auto f=comp3;
//  qDebug()<<f->toString();
//  qDebug()<<f->f(32)<<f->f(320)<<f->invert(5.3)<<f->invert(4.8);
//  qDebug()<<"";
//  f=compSmart;
//  qDebug()<<f->toString();
//  qDebug()<<f->f(32)<<f->f(320)<<f->invert(5.3)<<f->invert(4.8);
//  qDebug()<<f2->toString("x");
  return a.exec();
}
