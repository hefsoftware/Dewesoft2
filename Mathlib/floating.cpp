#include "floating.h"
#include <QList>
#include <QDebug>
#include <cmath>
#include <limits>

using namespace HEF;

// Defined as macros for performance
#define ENCODE64_FAST(value) (*(quint64 *)&(value))
#define DECODE64_FAST(value) (*(double *)&(value))

static const QList<double> testValues({qQNaN(), -qInf(), -1.31e13, -10/3, -1, -2.225073858507201e-308, -0., 0., 2.225073858507201e-308, 1., 10/3, 1.31e13, qInf()});

double HEF::decodeFloating64Portable(quint64 value)
{
  bool sign=value>>63;
  int exp=(value>>52)&0x7FF;
  value=value&0xFFFFFFFFFFFFFl;
  double ret=qQNaN();
  switch(exp)
  {
  case 0:
    ret=value*pow(2,-1022-52); break;
  default:
    ret=((((quint64)1)<<52)|value)*pow(2,exp-53-1022); break;
  case 2047:
    ret=(value?qQNaN():qInf()); break;
  }
  if(sign)
    ret=-ret;
  return ret;
}

quint64 HEF::encodeFloating64Portable(double value)
{
  bool sign=true;
  int exp=0;
  quint64 mantissa=0;
  sign=copysign(1,value)<=-0.; // Handles +/-0. and NaNs
  value=std::fabs(value);
  if(qIsFinite(value))
  {
    value=std::frexp (value, &exp);
    if(value==0.)
      exp=0.;
    else
    {
      exp+=1022;
      if(exp>0)
        mantissa=0xFFFFFFFFFFFFFl&(quint64)(value*pow(2,53));
      else if(exp>=-51)
      {
        // Subnormal number
        mantissa=(quint64)(value*pow(2,exp+52));
        exp=0;
      }
    }
  }
  else if(qIsInf(value))
  {
    exp=2047;
  }
  else if(qIsNaN(value))
  {
    exp=2047;
    mantissa=0x8000000000000l;
  }
  return (((quint64)sign)<<63)|(((quint64)exp)<<52)|mantissa;
}

//Test performances
//QElapsedTimer t;
//t.start();
//for(qint64 i=0;i<100000000;i++)
//  HEF::decodeFloating64Portable(HEF::encodeFloating64Portable(0.15));
//qDebug()<<"Encoded portable"<<t.restart();
//for(qint64 i=0;i<100000000;i++)
//  HEF::decodeFloating64Fast(HEF::encodeFloating64Fast(0.15));
//qDebug()<<"Encoded fast"<<t.restart();
//for(qint64 i=0;i<100000000;i++)
//  HEF::decodeFloating64(HEF::encodeFloating64(0.15));
//qDebug()<<"Encoded"<<t.restart();
//testFloatingPoint();
//exit(0);
void testFloatingPoint()
{
  for(int i=0;i<testValues.size();i++)
  {
    double value=testValues[i];
    quint64 simpleCast=*((quint64*)&value);
    quint64 encoded=encodeFloating64Portable(value);
    if(encoded!=simpleCast)
      qDebug()<<i<<value<<QString::number(simpleCast,16)<<"!="<<QString::number(encoded,16);
  }

  for(int i=0;i<testValues.size();i++)
  {
    double value=testValues[i];
    double reencoded=decodeFloating64Portable(encodeFloating64Portable(value));
    if(value!=reencoded && !(qIsNaN(value) && qIsNaN(reencoded)))
      qDebug()<<i<<value<<"!="<<reencoded;//QString::number(simpleCast,16)<<QString::number(encoded,16);
  }
}
static bool hasFastEncodeDecode()
{
  bool fast=true;
  for(int i=0;i<testValues.size();i++)
  {
    if(encodeFloating64Portable(testValues[i])!=ENCODE64_FAST(testValues[i]))
    {
      fast=false;
      break;
    }
  }
  if(fast)
  {
    for(int i=0;i<testValues.size();i++)
    {
      double value=testValues[i];
      quint64 encoded=encodeFloating64Portable(value);
      double decoded=DECODE64_FAST(encoded);
      if(decoded!=value && !(qIsNaN(decoded) && qIsNaN(value)))
      {
        fast=false;
        break;
      }
    }
  }
  return fast;
}
static bool fastEncodeDecode=false, slowEncodeDecode=false;
static bool pickEncodeDecodeMethod()
{
  fastEncodeDecode=hasFastEncodeDecode();
  slowEncodeDecode=!fastEncodeDecode;
  if(slowEncodeDecode)
    qWarning("Slow encode/decode selected");
}

quint64 HEF::encodeFloating64(double value)
{
  if(fastEncodeDecode)
    return ENCODE64_FAST(value);
  else if(slowEncodeDecode)
    return encodeFloating64Portable(value);
  else
  {
    pickEncodeDecodeMethod();
    return encodeFloating64(value);
  }
}

double HEF::decodeFloating64(quint64 value)
{
  if(fastEncodeDecode)
    return DECODE64_FAST(value);
  else if(slowEncodeDecode)
    return encodeFloating64Portable(value);
  else
  {
    pickEncodeDecodeMethod();
    return decodeFloating64(value);
  }
}

quint64 HEF::encodeFloating64Fast(double value)
{
  return ENCODE64_FAST(value);
}

double HEF::decodeFloating64Fast(quint64 value)
{
  return DECODE64_FAST(value);
}
