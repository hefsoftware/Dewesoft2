#include "datasource2doptimized.h"
#include "../Mathlib/helper.h"
#include "../Mathlib/sum.h"
#include <QDebug>
using namespace DataFile;

Source2DOptimized::Source2DOptimized()
{

}

Error Source2DOptimized::getStatFast(qint64 start, qint64 size, double *min, double *max, double *integral, double *squaredIntegral, double *deltaX)
{
//  return DataSource2D::getStatFast(start, size, min, max, integral, squaredIntegral, deltaX);
  // Check values with slow method
//  double stdMin=min?*min:qQNaN(), stdMax=max?*max:qQNaN(), stdIntegral=integral?*integral:qQNaN(), stdSquaredIntegral=squaredIntegral?*squaredIntegral:qQNaN(), stdDeltaX=deltaX?*deltaX:qQNaN();
//  DataSource2D::getStatFast(start, size, &stdMin, &stdMax, &stdIntegral, &stdSquaredIntegral, &stdDeltaX);
  int cacheIndexOk=-1;
  qint64 end=start+size;
  qint64 startOpt=start, endOpt=end;
  Error ret=Error::Success;

  for(int cacheIndex=0; cacheIndex<m_cacheSize.size(); cacheIndex++)
  {
    auto div=m_cacheSize[cacheIndex];
    qint64 curStart=HEF::roundUp(start, div), curEnd=HEF::roundDown(end, div);
    if(curEnd>curStart)
    {
      cacheIndexOk=cacheIndex;
      startOpt=curStart;
      endOpt=curEnd;
    }
    else
      break;
  }
  if(cacheIndexOk<0)
    ret=DataSource2D::getStatFast(start, size, min, max, integral, squaredIntegral);
  else
  {
    auto div=m_cacheSize[cacheIndexOk];
    ret=getCachedStat(cacheIndexOk, startOpt/div, (endOpt-startOpt)/div, min, max, integral, squaredIntegral);
//    qDebug()<<"Root"<<m_cacheSize[cacheIndexOk]<<startOpt<<endOpt-startOpt<<*max;
    if(ret==Error::Success)
    {
      for(int cacheIndex=cacheIndexOk-1; cacheIndex>=0; cacheIndex--)
      {
        div=m_cacheSize[cacheIndex];
        auto curStart=HEF::roundUp(start,div), curEnd=HEF::roundDown(end, div);
        if(curStart<startOpt)
        {
          ret=getCachedStat(cacheIndex, curStart/div, (startOpt-curStart)/div, min, max, integral, squaredIntegral);
          if(ret!=Error::Success)
            break;
//          qDebug()<<"Start"<<m_cacheSize[cacheIndex]<<curStart<<startOpt-curStart<<*max;
          startOpt=curStart;
        }
        if(curEnd>endOpt)
        {
          ret=getCachedStat(cacheIndex, endOpt/div, (curEnd-endOpt)/div, min, max, integral, squaredIntegral);
          if(ret!=Error::Success)
            break;
//          qDebug()<<"End"<<m_cacheSize[cacheIndex]<<curEnd<<curEnd-endOpt<<*max;
          endOpt=curEnd;
        }
      }
      if(startOpt>start && ret==Error::Success)
        ret=DataSource2D::getStatFast(start, startOpt-start, min, max, integral, squaredIntegral);
      if(endOpt<end && ret==Error::Success)
        ret=DataSource2D::getStatFast(endOpt, end-endOpt, min, max, integral, squaredIntegral);
    }
  }
  if(deltaX && ret==Error::Success && size>1)
  {
    double x0,x1;
    if((ret=x(start, x0))==Error::Success && (ret=x(start+size-1, x1))==Error::Success)
      *deltaX+=x1-x0;
  }

//  if(min && qAbs(stdMin-*min)>0.001)
//    qDebug()<<"Min mismatch"<<start<<size<<*min<<stdMin;
//  if(max && qAbs(stdMax-*max)>0.001)
//    qDebug()<<"Max mismatch"<<start<<size<<*max<<stdMax;
//  if(integral && qAbs(stdIntegral-*integral)>0.001)
//    qDebug()<<"Integral mismatch"<<start<<size<<*integral<<stdIntegral;
//  if(deltaX && qAbs(stdDeltaX-*deltaX)>0.001)
//    qDebug()<<"Delta Xmismatch"<<start<<size<<*deltaX<<stdDeltaX;
  return ret;
}

QVector<qint64> Source2DOptimized::suggestedCacheSize(qint64 size)
{
  QVector<qint64> ret;
  if(size>=32)
    ret.append(32);
  if(size>=128)
    ret.append(128);
  for(qint64 s=512; s<size; s*=2)
    ret.append(s);
//  for(qint64 s=2; s<size; s*=2)
//    ret.append(s);

  return ret;
}

void Source2DOptimized::CacheData::update(double *min, double *max, double *integral, double *squaredIntegral)
{
  if(min)
    *min=fmin(*min, this->min);
  if(max)
    *max=fmax(*max, this->max);
  if(integral)
    *integral+=this->integral;
  if(squaredIntegral)
    *integral+=this->squaredIntegral;
}

void Source2DOptimized::CacheData::set(const HEF::StatAccumulator &data)
{
  min=data.min();
  max=data.max();
  integral=data.sum();
  squaredIntegral=data.squaredSum();
}
