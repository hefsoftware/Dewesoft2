#include "datasource2d.h"
#include "../Mathlib/range.h"
#include "../Mathlib/sum.h"
#include <QDebug>
using namespace DataFile;
DataSource2D::DataSource2D(QObject *parent) : QObject(parent), m_buffer(nullptr), m_bufferSize(0), m_cacheType(InterpolationCacheType::NoCache), m_cacheIndex(-1)
{
  m_interpolate=Interpolate::Hermite;
}



Error DataSource2D::data(qint64 startSample, qint64 numSamples, double *x, double *y)
{
  Error ret;
  if(numSamples<=0 || (!x && !y))
    ret=((numSamples<0) ? Error::Params : Error::Success);
  else if(!y)
    ret=dataX(startSample, numSamples, x);
  else if(!x)
    ret=dataY(startSample, numSamples, y);
  else
  {
    auto endSample=startSample+numSamples;
    qint64 cnt;
    if((ret=size(cnt))==Error::Success)
    {
      if(endSample<=0 || startSample>=cnt)
      {
        // Outside the limits. Just fills in with Nans
        for(qint64 i=0;i<numSamples;i++)
          x[i]=y[i]=qQNaN();
        ret=Error::Success;
      }
      else
      {
        qint64 skipStart=0, skipEnd=0;
        if(startSample<0)
        {
          skipStart=-startSample;
          startSample=0;
        }
        if(endSample>cnt)
        {
          skipEnd=endSample-cnt;
          endSample=cnt;
        }
        numSamples=endSample-startSample;
        ret=dataCached(startSample, numSamples, &x[skipStart], &y[skipStart]);
        for(qint64 i=0;i<skipStart;i++)
          x[i]=y[i]=qQNaN();
        for(qint64 i=0;i<skipEnd;i++)
          x[skipStart+numSamples+i]=y[skipStart+numSamples+i]=qQNaN();
      }
    }
  }
  return ret;
}

Error DataSource2D::dataX(qint64 startSample, qint64 numSamples, double *x)
{
  Error ret;
  if(numSamples<=0 || !x)
    ret=((numSamples<0) ? Error::Params : Error::Success);
  else
  {
    auto endSample=startSample+numSamples;
    qint64 cnt;
    if((ret=size(cnt))==Error::Success)
    {
      if(endSample<=0 || startSample>=cnt)
      {
        // Outside the limits. Just fills in with Nans
        for(qint64 i=0;i<numSamples;i++)
          x[i]=qQNaN();
        ret=Error::Success;
      }
      else
      {
        qint64 skipStart=0, skipEnd=0;
        if(startSample<0)
        {
          skipStart=-startSample;
          startSample=0;
        }
        if(endSample>cnt)
        {
          skipEnd=endSample-cnt;
          endSample=cnt;
        }
        numSamples=endSample-startSample;
        ret=dataCachedX(startSample, numSamples, &x[skipStart]);
        for(qint64 i=0;i<skipStart;i++)
          x[i]=qQNaN();
        for(qint64 i=0;i<skipEnd;i++)
          x[skipStart+numSamples+i]=qQNaN();
      }
    }
  }
  return ret;
}

Error DataSource2D::dataY(qint64 startSample, qint64 numSamples, double *y)
{
  Error ret;
  if(numSamples<=0 || !y)
    ret=((numSamples<0) ? Error::Params : Error::Success);
  else
  {
    qint64 cnt;
    if((ret=size(cnt))==Error::Success)
    {
      auto endSample=startSample+numSamples;
      if(endSample<=0 || startSample>=cnt)
      {
        // Outside the limits. Just fills in with Nans
        for(qint64 i=0;i<numSamples;i++)
          y[i]=qQNaN();
        ret=Error::Success;
      }
      else
      {
        qint64 skipStart=0, skipEnd=0;
        if(startSample<0)
        {
          skipStart=-startSample;
          startSample=0;
        }
        if(endSample>cnt)
        {
          skipEnd=endSample-cnt;
          endSample=cnt;
        }
        numSamples=endSample-startSample;
        ret=dataCachedY(startSample, numSamples, &y[skipStart]);
        for(qint64 i=0;i<skipStart;i++)
          y[i]=qQNaN();
        for(qint64 i=0;i<skipEnd;i++)
          y[skipStart+numSamples+i]=qQNaN();
      }
    }
  }
  return ret;
}

Error DataSource2D::dataCached(qint64 startSample, qint64 numSamples, double *x, double *y)
{
  return dataUncached(startSample, numSamples, x, y);
}

Error DataSource2D::dataCachedX(qint64 startSample, qint64 numSamples, double *x)
{
  return dataUncachedX(startSample, numSamples, x);
}

Error DataSource2D::dataCachedY(qint64 startSample, qint64 numSamples, double *y)
{
  return dataUncachedY(startSample, numSamples, y);
}


//Error DataSource2D::avg(double xMin, double xMax, double &result)
//{

////  indexGE(xMin, indexMin)
//}

Error DataSource2D::dataUncachedX(qint64 startSample, qint64 numSamples, double *x)
{
  allocateBuffer(numSamples);
  return dataUncached(startSample, numSamples, x, m_buffer);
}

Error DataSource2D::dataUncachedY(qint64 startSample, qint64 numSamples, double *y)
{
  allocateBuffer(numSamples);
  return dataUncached(startSample, numSamples, m_buffer, y);
}

Error DataSource2D::yStepFast(qint64 index, double xValue, double &yResult, double *integralBefore, double *integralAfter, double *squaredIntegralBefore, double *squaredIntegralAfter, double *xBefore, double *xAfter)
{
  Error ret=Error::Success;
  if((m_cacheType==InterpolationCacheType::Linear && m_cacheIndex==index) || (ret=precalculateLinearInterpolation(index))==Error::Success)
    yResult=internalYStep(xValue, integralBefore, integralAfter, squaredIntegralBefore, squaredIntegralAfter, xBefore, xAfter);
  return ret;
}

Error DataSource2D::yStepFast(qint64 index, double xValue, double &yResult)
{
  Error ret=Error::Success;
  if((m_cacheType==InterpolationCacheType::Linear && m_cacheIndex==index) || (ret=precalculateLinearInterpolation(index))==Error::Success)
    yResult=internalYStep(xValue);
  return ret;
}

Error DataSource2D::yStepMiddleFast(qint64 index, double xValue, double &yResult, double *integralBefore, double *integralAfter, double *squaredIntegralBefore, double *squaredIntegralAfter, double *xBefore, double *xAfter)
{
  Error ret=Error::Success;
  if((m_cacheType==InterpolationCacheType::Linear && m_cacheIndex==index) || (ret=precalculateLinearInterpolation(index))==Error::Success)
    yResult=internalYMiddle(xValue, integralBefore, integralAfter, squaredIntegralBefore, squaredIntegralAfter, xBefore, xAfter);
  return ret;
}

Error DataSource2D::yStepMiddleFast(qint64 index, double xValue, double &yResult)
{
  Error ret=Error::Success;
  if((m_cacheType==InterpolationCacheType::Linear && m_cacheIndex==index) || (ret=precalculateLinearInterpolation(index))==Error::Success)
    yResult=internalYMiddle(xValue);
  return ret;
}

Error DataSource2D::yLinearFast(qint64 index, double xValue, double &yResult)
{
  Error ret=Error::Success;
  if((m_cacheType==InterpolationCacheType::Linear && m_cacheIndex==index) || (ret=precalculateLinearInterpolation(index))==Error::Success)
    yResult=internalYLinear(xValue);
  return ret;
}

Error DataSource2D::yLinearFast(qint64 index, double xValue, double &yResult, double *integralBefore, double *integralAfter, double *squaredIntegralBefore, double *squaredIntegralAfter, double *xBefore, double *xAfter)
{
  Error ret=Error::Success;
  if((m_cacheType==InterpolationCacheType::Linear && m_cacheIndex==index) || (ret=precalculateLinearInterpolation(index))==Error::Success)
    yResult=internalYLinear(xValue, integralBefore, integralAfter, squaredIntegralBefore, squaredIntegralAfter, xBefore, xAfter);
  return ret;
}

Error DataSource2D::yHermiteFast(qint64 index, double xValue, double &yResult)
{
  Error ret=Error::Success;
  if((m_cacheType==InterpolationCacheType::Hermite && m_cacheIndex==index) || (ret=precalculateHermiteInterpolation(index))==Error::Success)
    yResult=internalYHermite(xValue);
  return ret;
}

Error DataSource2D::yInterpolateFast(qint64 index, double xValue, double &yResult)
{
  switch(m_interpolate)
  {
    case Interpolate::Step:
      return yStepFast(index, xValue, yResult);
    case Interpolate::Middle:
      return yStepMiddleFast(index, xValue, yResult);
    case Interpolate::Linear:
      return yLinearFast(index, xValue, yResult);
    case Interpolate::Hermite:
      return yHermiteFast(index, xValue, yResult);
  }
  return Error::Unimplemented;
}

Error DataSource2D::yHermiteFast(qint64 index, double xValue, double &yResult, double *integralBefore, double *integralAfter, double *squaredIntegralBefore, double *squaredIntegralAfter, double *xBefore, double *xAfter)
{
  Error ret=Error::Success;
  if((m_cacheType==InterpolationCacheType::Hermite && m_cacheIndex==index) || (ret=precalculateHermiteInterpolation(index))==Error::Success)
    yResult=internalYHermite(xValue, integralBefore, integralAfter, squaredIntegralBefore, squaredIntegralAfter, xBefore, xAfter);
  return ret;
}

Error DataSource2D::yInterpolateFast(qint64 index, double xValue, double &yResult, double *integralBefore, double *integralAfter, double *squaredIntegralBefore, double *squaredIntegralAfter, double *xBefore, double *xAfter)
{
  switch(m_interpolate)
  {
    case Interpolate::Step:
      return yStepFast(index, xValue, yResult, integralBefore, integralAfter, squaredIntegralBefore, squaredIntegralAfter, xBefore, xAfter);
    case Interpolate::Middle:
      return yStepMiddleFast(index, xValue, yResult, integralBefore, integralAfter, squaredIntegralBefore, squaredIntegralAfter, xBefore, xAfter);
    case Interpolate::Linear:
      return yLinearFast(index, xValue, yResult, integralBefore, integralAfter, squaredIntegralBefore, squaredIntegralAfter, xBefore, xAfter);
    case Interpolate::Hermite:
      return yHermiteFast(index, xValue, yResult, integralBefore, integralAfter, squaredIntegralBefore, squaredIntegralAfter, xBefore, xAfter);
  }
  return Error::Unimplemented;
}

//Error DataSource2D::yStep(double xValue, double &yResult, double *integralBefore, double *integralAfter)
//{
//  Error ret;
//  qint64 index=-1;
//  if((ret=interpolationPrepare(xValue, yResult, index))==Error::NeedExtraProcess) // Calculate index and handle basic cases (before begin and after last element)
//    if((m_cacheType==InterpolationCacheType::Linear && m_cacheIndex==index) || (ret=precalculateLinearInterpolation(index))==Error::Success)
//    {
//      ret=Error::Success;
//      yResult=m_y0;
//      if(integralBefore)
//        *integralBefore=m_y0*(xValue-m_x0);
//      if(integralAfter)
//        *integralAfter=m_y0*(m_x1-xValue);
//    }
//  return ret;
//}

//Error DataSource2D::yStepMiddle(double xValue, double &yResult, double *integralBefore, double *integralAfter)
//{
//  Error ret;
//  qint64 index=-1;
//  if((ret=interpolationPrepare(xValue, yResult, index))==Error::NeedExtraProcess) // Calculate index and handle basic cases (before begin and after last element)
//    if((m_cacheType==InterpolationCacheType::Linear && m_cacheIndex==index) || (ret=precalculateLinearInterpolation(index))==Error::Success)
//    {
//      double midX=(m_x0+m_x1)*.5;
//      double halfSpan=(m_x1-m_x0)*.5;
//      ret=Error::Success;
//      yResult=xValue<=midX?m_y0:m_y1;
//      if(integralBefore)
//        *integralBefore=(xValue<=midX)?m_y0*(xValue-m_x0):(halfSpan*m_y0 + m_y1*(xValue-midX));
//      if(integralAfter)
//        *integralAfter=(xValue>=midX)?m_y1*(m_x1-xValue):(halfSpan*m_y1 + m_y0*(midX-xValue));
//    }
//  return ret;
//}

Error DataSource2D::interpolationPrepare(double xValue, double &yResult, qint64 &index)
{
  Error ret=Error::Success;
  yResult=qQNaN();
  if(m_cacheIndex>=0 && xValue>=m_x0 && xValue<=m_x1)
    index=m_cacheIndex;
  else
    ret=indexLE(xValue, index);
  qint64 s;
  if(ret==Error::Success && (ret=size(s))==Error::Success)
  {
    if(index==s-1)
    {
      double xLast, yLast;
      if((ret=dataCached(index, xLast, yLast))==Error::Success)
        ret=(xLast==xValue)?y(index, yResult):Error::Success;
    }
    else if(index>=0)
      ret=Error::NeedExtraProcess;
    else
      ret=Error::Success;
  }
  return ret;
}
//Error DataSource2D::yLinear(double xValue, double &yResult, double *integralBefore, double *integralAfter)
//{
//  Error ret;
//  qint64 index=-1;
//  if((ret=interpolationPrepare(xValue, yResult, index))==Error::NeedExtraProcess) // Calculate index and handle basic cases (before begin and after last element)
//    if((m_cacheType==InterpolationCacheType::Linear && m_cacheIndex==index) || (ret=precalculateLinearInterpolation(index))==Error::Success)
//    {
//      ret=Error::Success;
//      yResult=(xValue-m_x0)*m_c1+m_y0;
//      if(integralBefore)
//        *integralBefore=HEF::integralSawtooth(m_x0, m_y0, xValue,yResult);
//      if(integralAfter)
//        *integralAfter=HEF::integralSawtooth(xValue,yResult, m_x1, m_y1);
//    }
//  return ret;
//}

//Error DataSource2D::yStep(double xValue, double &yResult, double *integralBefore, double *integralAfter)

double DataSource2D::internalYMiddle(double x, double *integralBefore, double *integralAfter, double *squaredIntegralBefore, double *squaredIntegralAfter, double *xBefore, double *xAfter)
{
  double ret, integ, sqInteg, diff;
  double mid=(m_x1+m_x0)*.5;
  double integTot=(m_y0+m_y1)*(m_x1-m_x0)*.5, sqIntegTot=(m_y0*m_y0+m_y1*m_y1)*(m_x1-m_x0)*.5;
  if(x<m_x0)
  {
    diff=0.;
    ret=m_y0;
    integ=sqInteg=0.;
  }
  else if(x>m_x1)
  {
    diff=m_x1-m_x0;
    ret=m_y1;
    integ=integTot;
    sqInteg=sqIntegTot;
  }
  else if(x<mid)
  {
    diff=x-m_x0;
    ret=m_y0;
    integ=m_y0*(x-m_x0);
    sqInteg=m_y0*m_y0*(x-m_x0);
  }
  else
  {
    diff=x-m_x0;
    ret=m_y1;
    integ=integTot-(m_y1*(m_x1-x));
    sqInteg=sqIntegTot-(m_y1*m_y1*(m_x1-x));
  }
  if(integralBefore)
    *integralBefore=integ;
  if(squaredIntegralBefore)
    *squaredIntegralBefore=sqInteg;
  if(integralAfter)
    *integralAfter=integTot-integ;
  if(squaredIntegralAfter)
    *squaredIntegralAfter=sqIntegTot-sqInteg;
  if(xBefore)
    *xBefore=diff;
  if(xAfter)
    *xAfter=(m_x1-m_x0)-diff;
  return ret;
}

double DataSource2D::internalYStep(double x, double *integralBefore, double *integralAfter, double *squaredIntegralBefore, double *squaredIntegralAfter, double *xBefore, double *xAfter)
{
  double ret, integ, sqInteg, diff;
  double integTot=m_y0*(m_x1-m_x0), sqIntegTot=m_y0*m_y0*(m_x1-m_x0);
  if(x<m_x0)
  {
    diff=0;
    ret=m_y0;
    integ=sqInteg=0.;
  }
  else if(x>m_x1)
  {
    diff=m_x1-m_x0;
    ret=m_y1;
    integ=integTot;
    sqInteg=sqIntegTot;
  }
  else
  {
    diff=x-m_x0;
    ret=m_y0;
    integ=m_y0*(x-m_x0);
    sqInteg=m_y0*m_y0*(x-m_x0);
  }
  if(integralBefore)
    *integralBefore=integ;
  if(squaredIntegralBefore)
    *squaredIntegralBefore=sqInteg;
  if(integralAfter)
    *integralAfter=integTot-integ;
  if(squaredIntegralAfter)
    *squaredIntegralAfter=sqIntegTot-sqInteg;
  if(xBefore)
    *xBefore=diff;
  if(xAfter)
    *xAfter=(m_x1-m_x0)-diff;
  return ret;
}

double DataSource2D::internalYLinear(double x, double *integralBefore, double *integralAfter, double *squaredIntegralBefore, double *squaredIntegralAfter, double *xBefore, double *xAfter)
{
  double ret, integ, sqInteg, diff;
  if(x<m_x0)
  {
    diff=0.;
    ret=m_y0;
    integ=sqInteg=0.;
  }
  else if(x>m_x1)
  {
    diff=m_x1-m_x0;
    ret=m_y1;
    integ=HEF::integralSawtooth(m_x0, m_y0, m_x1, m_y1);
    sqInteg=HEF::integralSquaredSawtooth(m_x0, m_y0, m_x1, m_y1);
  }
  else
  {
    diff=x-m_x0;
    ret=diff*m_c1+m_y0;
    integ=HEF::integralSawtooth(m_x0, m_y0, x, ret);
    sqInteg=HEF::integralSquaredSawtooth(m_x0, m_y0, x, ret);
  }
  if(integralBefore)
    *integralBefore=integ;
  if(squaredIntegralBefore)
    *squaredIntegralBefore=sqInteg;
  if(integralAfter)
    *integralAfter=HEF::integralSawtooth(m_x0, m_y0, m_x1, m_y1)-integ;
  if(squaredIntegralAfter)
    *squaredIntegralAfter=HEF::integralSquaredSawtooth(m_x0, m_y0, m_x1, m_y1)-sqInteg;
  if(xBefore)
    *xBefore=diff;
  if(xAfter)
    *xAfter=(m_x1-m_x0)-diff;
  return ret;
}

double DataSource2D::internalYHermite(double x)
{
  // https://en.wikipedia.org/wiki/Monotone_cubic_interpolation
  double ret=qQNaN();
  if(x<m_x0) ret=m_y0;
  else if(x>m_x1) ret=m_y1;
  else
  {
    double diff = x - m_x0, diffSq = diff*diff;
    ret=m_y0 + m_c1*diff + m_c2*diffSq + m_c3*diff*diffSq;
  }
  return ret;
}

double DataSource2D::internalYHermite(double x, double *integralBefore, double *integralAfter, double *squaredIntegralBefore, double *squaredIntegralAfter, double *xBefore, double *xAfter)
{
  double ret, integ, sqInteg, diff;
  double integTot=(integralBefore||integralAfter)?HEF::integralHermite(m_x1-m_x0, m_y0, m_c1, m_c2, m_c3):qQNaN();
  double sqIntegTot=(squaredIntegralBefore||squaredIntegralAfter)?HEF::integralSquaredHermite(m_x1-m_x0, m_y0, m_c1, m_c2, m_c3):qQNaN();
  if(x<m_x0)
  {
    diff=0.;
    ret=m_y0;
    integ=sqInteg=0.;
  }
  else if(x>m_x1)
  {
    diff=(m_x1-m_x0);
    ret=m_y1;
    integ=integTot;
    sqInteg=sqIntegTot;
  }
  else
  {
    diff=x-m_x0;
    double diffSq = diff*diff;
    ret=m_y0 + m_c1*diff + m_c2*diffSq + m_c3*diff*diffSq;
    integ=(integralBefore||integralAfter)?HEF::integralHermite(diff, m_y0, m_c1, m_c2, m_c3):qQNaN();
    sqInteg=(squaredIntegralBefore||squaredIntegralAfter)?HEF::integralSquaredHermite(diff, m_y0, m_c1, m_c2, m_c3):qQNaN();
  }
  if(integralBefore)
    *integralBefore=integ;
  if(squaredIntegralBefore)
    *squaredIntegralBefore=sqInteg;
  if(integralAfter)
    *integralAfter=integTot-integ;
  if(squaredIntegralAfter)
    *squaredIntegralAfter=sqIntegTot-sqInteg;
  if(xBefore)
    *xBefore=diff;
  if(xAfter)
    *xAfter=(m_x1-m_x0)-diff;
  return ret;

}

double DataSource2D::internalYMiddle(double x)
{
  double ret=qQNaN();
  if(x<(m_x0+m_x1)*.5) ret=m_y0;
  else ret=m_y1;
  return ret;
}


double DataSource2D::internalYStep(double x)
{
  double ret=qQNaN();
  if(x<m_x1) ret=m_y0;
  else ret=m_y1;
  return ret;
}

double DataSource2D::internalYLinear(double x)
{
  double ret=qQNaN();
  if(x<m_x0) ret=m_y0;
  else if(x>m_x1) ret=m_y1;
  else ret=(x-m_x0)*m_c1+m_y0;
  return ret;
}



//Error DataSource2D::yHermite(double xValue, double &yResult, double *integralBefore, double *integralAfter)
//{
//  // https://en.wikipedia.org/wiki/Monotone_cubic_interpolation
//  Error ret;
//  qint64 index=-1;
//  if((ret=interpolationPrepare(xValue, yResult, index))==Error::NeedExtraProcess) // Calculate index and handle basic cases (before begin and after last element)
//    if((m_cacheType==InterpolationCacheType::Hermite && m_cacheIndex==index) || (ret=precalculateHermiteInterpolation(index))==Error::Success)
//    {
//      ret=Error::Success;
//      double diff = xValue - m_x0, diffSq = diff*diff;
//      yResult=m_y0 + m_c1*diff + m_c2*diffSq + m_c3*diff*diffSq;
//      if(integralBefore)
//        *integralBefore=HEF::integralSawtooth(m_x0, m_y0, xValue,yResult);
//      if(integralAfter)
//        *integralAfter=HEF::integralSawtooth(xValue,yResult, m_x1, m_y1);
//    }
//  return ret;
//}

Error DataSource2D::getStatFast(qint64 start, qint64 size, double *min, double *max, double *integral, double *squaredIntegral, double *xDelta)
{
  Error ret=Error::Generic;
  HEF::KahanSum integ, sqInteg;
//  qDebug()<<"Get stat fast"<<start<<size;
  if(size<200)
  {
    double x[size];
    double y[size];
    if((ret=dataCached(start,size,x,y))==Error::Success)
    {
      if(xDelta)
        *xDelta+=x[size-1]-x[0];
      for(int i=0;i<size;i++)
      {
        double v=y[i];
        if(min)
          *min=fmin(*min, v);
        if(max)
          *max=fmax(*max, v);
        if(integral && i>0)
          integ.add(HEF::integralSawtooth(x[i-1], y[i-1],x[i],y[i]));
        if(squaredIntegral && i>0)
          sqInteg.add(HEF::integralSquaredSawtooth(x[i-1], y[i-1],x[i],y[i]));
      }
    }
  }
  else
  {
    int bufSize=qMin(size, (qint64)4096);
    bool notFirst=false;
//    QScopedArrayPointer<double> x(new double[bufSize]),y(new double[bufSize]);
    double x[bufSize];
    double y[bufSize];
    double xFirst=0.;
    while(true)
    {
      int nRead=qMin((qint64)bufSize-notFirst,size), nInBuf=nRead+notFirst;
      if((ret=dataCached(start, nRead, x+notFirst,y+notFirst))!=Error::Success)
        break;
      if(!notFirst)
        xFirst=x[0];
      for(qint64 i=notFirst;i<nInBuf;i++)
      {
        double v=y[i];
        if(min)
          *min=fmin(*min, v);
        if(max)
          *max=fmax(*max, v);
        if(integral && i>0)
          integ.add(HEF::integralSawtooth(x[i-1], y[i-1],x[i],y[i]));
        if(squaredIntegral && i>0)
          sqInteg.add(HEF::integralSquaredSawtooth(x[i-1], y[i-1],x[i],y[i]));
      }
      size-=nRead;
      if(size<=0)
      {
        if(xDelta)*xDelta+=x[nInBuf-1]-xFirst;
        break;
      }
      start+=nRead;
      x[0]=x[nInBuf-1];
      y[0]=y[nInBuf-1];
      notFirst=true;
    }
  }
  if(integral)
    *integral+=integ.sum();
  if(squaredIntegral)
    *squaredIntegral+=sqInteg.sum();
  return ret;
}

HEF::Range DataSource2D::domain()
{
  return HEF::Range(-qInf(),qInf());
}
//Error DataSource2D::getStat(const QList<HEF::Range> &ranges, double *avg, double *min, double *max, double *rms, double *integral, double *integralSquared)
Error DataSource2D::getStat(const QList<HEF::Range> &ranges, QVector<double> *min, QVector<double> *max, QVector<double> *avg, QVector<double> *rms, QVector<double> *delta, QVector<double> *integral, QVector<double> *integralSquared)
{
  if(min)
    min->fill(qQNaN(), ranges.size());
  if(max)
    max->fill(qQNaN(), ranges.size());
  if(avg)
    avg->fill(qQNaN(), ranges.size());
  if(rms)
    rms->fill(qQNaN(), ranges.size());
  if(delta)
    delta->fill(qQNaN(), ranges.size());
  if(integral)
    integral->fill(qQNaN(), ranges.size());
  if(integralSquared)
    integralSquared->fill(qQNaN(), ranges.size());
  return getStat(ranges, min?min->data():nullptr, max?max->data():nullptr, avg?avg->data():nullptr, rms?rms->data():nullptr, delta?delta->data():nullptr, integral?integral->data():nullptr, integralSquared?integralSquared->data():nullptr);
}

Error DataSource2D::getStat(const QList<HEF::Range> &ranges, double *min, double *max, double *avg, double *rms, double *delta, double *integral, double *integralSquared)
{
  qint64 s;
  Error ret=size(s);
  auto d=domain();
  if(ret==Error::Success)
  {
//    qDebug()<<"Get stat";
    qint64 cacheIndex0, cacheIndex1;
    // X,Y, Integral before and integral after cache
    double cacheX0=qQNaN(), cacheX1=qQNaN(), cacheY0=qQNaN(), cacheY1=qQNaN(), cacheB0=qQNaN(), cacheB1=qQNaN(), cacheA0=qQNaN(), cacheA1=qQNaN();

    for(int i=0;i<ranges.size() && ret==Error::Success;i++)
    {
      auto cr=ranges[i]&d;
      qreal curMin=qQNaN(), curMax=qQNaN(), curIntegral=0., curSquaredIntegral=0., curDelta=0.;
//      qDebug()<<"This"<<i<<cr.toString();
      for(int j=0;j<cr.numBasicRanges();j++)
      {
        double x0=cr.basicRange(j).min(), x1=cr.basicRange(j).max(), y0=qQNaN(), y1=qQNaN(), b0=0,a0=0, b1=0 ,a1=0;
        qint64 i0=-1, i1=-1;
        if(x0==cacheX0)
        { i0=cacheIndex0; y0=cacheY0; b0=cacheB0; a0=cacheA0; }
        else if(x0==cacheX1)
        { i0=cacheIndex1; y0=cacheY1; b0=cacheB1; a0=cacheA1; }
        else
        {
          ret=indexGEFast(x0, i0, 0, s);
          if(ret==Error::Success)
            Q_ASSERT_X(false, "Here","yHermite unimplemented");
//            ret=yHermite(x0,y0, &b0, &a0);
        }
        if(x1==cacheX0)
          { i1=cacheIndex0; y1=cacheY0; b1=cacheB0; a1=cacheA0; }
        else if(x1==cacheX1)
          { i1=cacheIndex1; y1=cacheY1; b1=cacheB1; a1=cacheA1;  }
        else
        {
          if(ret==Error::Success)
            ret=indexGEFast(x1, i1, 0, s);
          if(ret==Error::Success)
            Q_ASSERT_X(false, "Here","yHermite unimplemented");
//            ret=yHermite(x1,y1, &b1, &a1);
        }
        if(ret==Error::Success)
        {
          curMin=std::fmin(y0,curMin);
          curMax=std::fmax(y0,curMax);
          curMin=std::fmin(y1,curMin);
          curMax=std::fmax(y1,curMax);
//          qDebug()<<"Ok"<<curMin<<curMax<<"Now checking the big interval"<<i0<<i1<<"Integrals"<<b0<<a0<<"and"<<b1<<a1;
          if(i1>i0)
          {
            double xr0=qQNaN(),xr1=qQNaN();
            if(ret==Error::Success)
              ret=x(i0,xr0);
            if(i1==i0+1)
              xr1=xr0;
            else if(ret==Error::Success)
              ret=x(i1-1,xr1);
            curDelta+=xr1-xr0;
            if(x0!=xr0 && i0>0)
            {
              double d=xr0-x0;
              curDelta+=d;
              curSquaredIntegral+=HEF::square(a0/d)*d;
              curIntegral+=a0;
            }
            if(x1!=xr1 && i1<s)
            {
              double d=x1-xr1;
              curDelta+=d;
              curSquaredIntegral+=HEF::square(b1/d)*d;
              curIntegral+=b1;
            }
            // We have at least a whole interval included
            if(ret==Error::Success)
              ret=getStatFast(i0, i1-i0, min?&curMin:nullptr, max?&curMax:nullptr, (integral||avg)?&curIntegral:nullptr, (integralSquared||rms)?&curSquaredIntegral:nullptr);
          }
          else if(i0>0 && i1<s) // i0==i1
          {
//            qDebug()<<"Inter point value"b1<<b0;
            curDelta+=x1-x0;
            curIntegral+=HEF::integralSawtooth(x0,y0,x1,y1);//b1-b0;
            curSquaredIntegral+=HEF::integralSquaredSawtooth(x0,y0,x1,y1);
//            qDebug()<<"Monorange"<<i1<<i0;
          }
        }
        cacheIndex0=i0; cacheX0=x0; cacheY0=y0; cacheA0=a0; cacheB0=b0;
        cacheIndex1=i1; cacheX1=x1; cacheY1=y1; cacheA1=a1; cacheB1=b1;
      }
      if(min)
        min[i]=curMin;
      if(max)
        max[i]=curMax;
      if(delta)
        delta[i]=curDelta;
      if(integral)
        integral[i]=curIntegral;
      if(integralSquared)
        integralSquared[i]=curSquaredIntegral;
      if(avg)
        avg[i]=curDelta>0.?curIntegral/curDelta:qQNaN();
      if(rms)
        rms[i]=curDelta>0.?sqrt(curSquaredIntegral/curDelta):qQNaN();
    }
  }
  return ret;
}

Error DataSource2D::index(double x, qint64 &index, double &percent)
{
  qint64 s;
  Error ret=size(s);
  percent=0.;
  index=-10;
  if(ret==Error::Success)
  {
    if(s==0)
    {
      index=0;
      percent=-1;
    }
    else
    {
      ret=indexGEFast(x, index, 0, s);
      if(ret==Error::Success)
      {
        if(index==0)
        {
          double x0;
          ret=dataCachedX(0,x0);
          if(ret==Error::Success)
          {
            index=0;
            percent=(x==x0)?0:-1.;
          }
        }
        else if(index>=s)
        {
          index=s-1;
          percent=2.;
        }
        else if(index<s)
        {
          double xr[2];
          index-=1;
          ret=dataCachedX(index, 2, xr);
          if(ret==Error::Success && xr[0]<xr[1])
            percent=(x-xr[0])/(xr[1]-xr[0]);
        }
      }
    }
  };
  return ret;
}

Error DataSource2D::indexLE(double x, qint64 &index)
{
//  if(m_cacheUsed && time>=m_cacheX[0] && time<=m_cacheX[m_cacheUsed-1])
//    return getSampleBeforeTime(time,m_cacheStart,m_cacheEnd);
//  else
  qint64 s;
  Error ret=size(s);
  return ret==Error::Success?indexLEFast(x, index, 0, s) : ret;
}

Error DataSource2D::indexGE(double x, qint64 &index)
{
  //  if(m_cacheUsed && time>=m_cacheX[0] && time<=m_cacheX[m_cacheUsed-1])
  //    return getSampleAfterTime(time,m_cacheStart,m_cacheEnd);
  //  else
  //    return getSampleAfterTime(time,0,m_size);
  qint64 s;
  Error ret=size(s);
  return ret==Error::Success?indexGEFast(x, index, 0, s) : ret;
}

Error DataSource2D::syncronous(qint64 index, qint64 count, double &frequency)
{
  frequency=qQNaN();
  qint64 s;
  Error ret=size(s);
  if(ret==Error::Success)
    ret=(index>=0 && index+count<=s)?syncronousFast(index, count, frequency):Error::Params;
  return ret;
}

Error DataSource2D::indexLEFast(double xValue, qint64 &index, qint64 low, qint64 high)
{
  Error ret;
  double xIndex;
  if((ret=indexGEFast(xValue, index, low, high))==Error::Success && (ret=x(index, xIndex))==Error::Success && xIndex!=xValue)
    index--;
  return ret;
}

Error DataSource2D::indexGEFast(double xValue, qint64 &index, qint64 low, qint64 high)
{
  Error ret=Error::Success;
  double vx;
  while(low!=high)
  {
    qint64 mid=(low+high)/2;
    ret=x(mid, vx);
    if(ret!=Error::Success)
      break;
    if(vx<xValue)
      low=mid+1;
    else
      high=mid;
  }
  index=low;
  return ret;
}

Error DataSource2D::allocateBuffer(qint64 size)
{
  Error ret=Error::Success;
  if(m_bufferSize<size)
  {
    double *newBuffer=(double *)realloc(m_buffer, size*sizeof(double));
    if(newBuffer)
    {
      m_buffer=newBuffer;
      m_bufferSize=size;
    }
    else
      ret=Error::Memory;
  }
  return ret;
}

Error DataSource2D::precalculateLinearInterpolation(qint64 index)
{
  Error ret;
  double x[2], y[2];
  if((ret=data(index, 2, x, y))==Error::Success)
  {
    m_x0=x[0]; m_y0=y[0];
    m_x1=x[1]; m_y1=y[1];
    m_c1=(y[1]-y[0])/(x[1]-x[0]);
    m_cacheType=InterpolationCacheType::Linear;
    m_cacheIndex=index;
  }
  return ret;
}

Error DataSource2D::precalculateHermiteInterpolation(qint64 index)
{
  Error ret;
  qint64 s;

  double c1_1,c1_2;
  double x[4], y[4];
  if((ret=size(s))==Error::Success && (ret=data(index-1, 4, x, y))==Error::Success)
  {
    double dx1=x[2]-x[1], dy1=y[2]-y[1], ds1=dy1/dx1;
    if(index==0)
      c1_1=ds1;
    else
    {
      auto dx0=x[1]-x[0], dy0=y[1]-y[0], ds0=dy0/dx0;
      if (ds0*ds1 <= 0)
        c1_1=0.;
      else {
        auto common = dx0 + dx1;
        c1_1=(3*common/((common + dx1)/ds0 + (common + dx0)/ds1));
      }
    }
    if(index==s-2)
      c1_2=ds1;
    else
    {
      auto dx2=x[3]-x[2], dy2=y[3]-y[2], ds2=dy2/dx2;
      if (ds1*ds2 <= 0)
        c1_2=0.;
      else {
        double common = dx1 + dx2;
        c1_2=(3*common/((common + dx2)/ds1 + (common + dx1)/ds2));
      }
    }
    auto invDx = 1/dx1, common = c1_1 + c1_2 - 2* ds1;
    double c2=((ds1 - c1_1 - common)*invDx), c3=(common*invDx*invDx);
    m_x0=x[1]; m_y0=y[1];
    m_x1=x[2]; m_y1=y[2];
    m_c1=c1_1; m_c2=c2; m_c3=c3;
    m_cacheType=InterpolationCacheType::Hermite;
    m_cacheIndex=index;
  }
  return ret;
}
//         QScopedPointer<double> y(new double[numSamples]);
//ret=data(&x[skipStart], &y[skipStart], startSample, numSamples);
//for(qint64 i=0;i<skipStart;i++)
//  y[i]=qQNaN();
//for(qint64 i=0;i<skipEnd;i++)
//  y[skipStart+numSamples+i]=qQNaN();

//double DataSource2D::x(qint64 index)
//{
//  double x=qQNaN(),y=qQNaN();
//  return (data(&x, &y, index, 1)?x:qQNaN());
//}

Error DataSourceTest::size(qint64 &result)
{
  result=3;
  return Error::Success;
}

Error DataSourceTest::dataUncached(qint64 startSample, qint64 numSamples, double *x, double *y)
{
  for(int i=0;i<numSamples;i++)
  {
    int j=startSample+i;
    switch(j)
    {
    case 0:
      x[i]=1; y[i]=0; break;
    case 1:
      x[i]=4; y[i]=2; break;
    case 2:
      x[i]=7; y[i]=4; break;
//    case 3:
//      x[i]=3.5; y[i]=1.4; break;
//    case 4:
//      x[i]=4.5; y[i]=1.8; break;
    }
  }
  return Error::Success;
}


DataSourceTest2::DataSourceTest2(const QList<double> &x, const QList<double> &y)
{
  Q_ASSERT(x.size()==y.size());
  m_x=x;
  m_y=y;
}

Error DataSourceTest2::size(qint64 &result)
{
  result=m_x.size();
  return Error::Success;
}

Error DataSourceTest2::dataUncached(qint64 startSample, qint64 numSamples, double *x, double *y)
{
  for(qint64 i=0;i<numSamples;i++)
  {
    if(x)
      x[i]=m_x[i+startSample];
    if(y)
      y[i]=m_y[i+startSample];
  }
  return Error::Success;
}
