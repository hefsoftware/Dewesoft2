#include "plotarea2d.h"
#include "../../Band/scale.h"
#include "../../../Mathlib/sum.h"
#include <QPainter>
#include <QDebug>
using namespace Plot;
extern QSharedPointer<DataFile::DataSource2D> testSource1;
extern QSharedPointer<DataFile::DataSource2D> testSource2;
#include <QElapsedTimer>

#include "profileapi.h"
inline qint64 highPrecisionTime()
{
  qint64 ret=-1;
  LARGE_INTEGER ticks;
  if (QueryPerformanceCounter(&ticks))
    ret=ticks.QuadPart;
  return ret;
}
inline qint64 highPrecisionFreq()
{
  qint64 ret=-1;
  LARGE_INTEGER ticks;
  if (QueryPerformanceFrequency(&ticks))
    ret=ticks.QuadPart;
  return ret;
}

// Optimized "domain &"; getStatOptim2
// Whole execution (new getStatOptim): 2.0ms
// No getStatFast: 1.3ms
// Only indexLEFast and yInterpolateFast: 1.1ms
// Only indexLEFast: 1.0ms
// Basic cycle: 0.62ms
// Only "domain &" 0.62ms
// Estimated: Domani & 0.6ms, getStatFast 0.7ms, indexLEFast: 0.4ms

// Optimized "domain &"
// Only domain &: 0.57ms
// Only domain &+getIndex: 1.0ms
// Whole execution (cached getStatFast 32,128,512,1024,... ): 2.3ms


// Whole execution (dynamic-alloca based getStatFast): 33.2ms
// Whole execution (stack based getStatFast) 27.4 uS
// Whole execution (cached getStatFast 32,128,512,1024,...) 3.1ms
// Whole execution (cached getStatFast 2,4,8,16,32,64,128,... ) 3.0ms
//

// Only domain and: 0.57ms


// Without inter-integer range data extraction (getStatFast only): 33.0ms
// Without getStatFast integer range data extraction (inter-integer ranges only): 2.5ms
// Without stats extraction: 2.3 mS

// Estimates:
// getStatFast 33.4ms
// inter-integer data extraction
//static DataFile::Error getStatOptim(DataFile::DataSource2D *data, QList<HEF::Range> ranges2, double *min, double *max, double *avg, double *rms, double *delta, double *integral, double *integralSquared)
//{
//  DataFile::Error ret=DataFile::Error::Generic;
//  qint64 s;
//  ret=data->size(s);
//  auto d=data->domain();
//  if(ret==DataFile::Error::Success)
//  {
//////    qDebug()<<"Get stat";
//    qint64 cacheIndex0, cacheIndex1;
////    // X,Y, Integral before and integral after cache
//    double cacheX0=qQNaN(), cacheX1=qQNaN(), cacheY0=qQNaN(), cacheY1=qQNaN(), cacheB0=qQNaN(), cacheB1=qQNaN(), cacheA0=qQNaN(), cacheA1=qQNaN();
//    auto ranges=ranges2;
//    ranges&=d; // Optimized join with domain
////    for(int i=0;i<ranges2.size();i++)
////    {
////      auto cur=ranges2[i]&d;
////      if(!cur.isEqual(ranges[i]))
////        qDebug()<<"Delta at"<<i<<ranges[i].toString()<<"instead of"<<cur.toString()<<"in"<<ranges[i].toString()<<"&"<<d.toString();
////    }


//    for(int i=0;i<ranges.size() && ret==DataFile::Error::Success;i++)
//    {
//      auto cr=ranges[i];
//      qreal curMin=qQNaN(), curMax=qQNaN(), curIntegral=0., curSquaredIntegral=0., curDelta=0.;
//      for(int j=0;j<cr.numBasicRanges();j++)
//      {
//        double x0=cr.basicRange(j).min(), x1=cr.basicRange(j).max(), y0=qQNaN(), y1=qQNaN(), b0=0,a0=0, b1=0 ,a1=0;
//        qint64 i0=-1, i1=-1;
//        if(x0==cacheX0)
//        { i0=cacheIndex0; y0=cacheY0; b0=cacheB0; a0=cacheA0; }
//        else if(x0==cacheX1)
//        { i0=cacheIndex1; y0=cacheY1; b0=cacheB1; a0=cacheA1; }
//        else
//        {
//          ret=data->indexGEFast(x0, i0, 0, s);
//          if(ret==DataFile::Error::Success)
//            Q_ASSERT_X(false, "Here","yHermite unimplemented");
////            ret=data->yHermite(x0,y0, &b0, &a0);
//        }
//        if(x1==cacheX0)
//          { i1=cacheIndex0; y1=cacheY0; b1=cacheB0; a1=cacheA0; }
//        else if(x1==cacheX1)
//          { i1=cacheIndex1; y1=cacheY1; b1=cacheB1; a1=cacheA1;  }
//        else
//        {
//          if(ret==DataFile::Error::Success)
//            ret=data->indexGEFast(x1, i1, 0, s);
//          if(ret==DataFile::Error::Success)
//            Q_ASSERT_X(false, "Here","yHermite unimplemented");
////            ret=data->yHermite(x1,y1, &b1, &a1);
//        }
//        if(ret==DataFile::Error::Success)
//        {
//          curMin=std::fmin(y0,curMin);
//          curMax=std::fmax(y0,curMax);
//          curMin=std::fmin(y1,curMin);
//          curMax=std::fmax(y1,curMax);
////          qDebug()<<"Ok"<<curMin<<curMax<<"Now checking the big interval"<<i0<<i1<<"Integrals"<<b0<<a0<<"and"<<b1<<a1;
//          if(i1>i0)
//          {
//            double xr0=qQNaN(),xr1=qQNaN();
//            if(ret==DataFile::Error::Success)
//              ret=data->x(i0,xr0);
//            if(i1==i0+1)
//              xr1=xr0;
//            else if(ret==DataFile::Error::Success)
//              ret=data->x(i1-1,xr1);
//            curDelta+=xr1-xr0;
//            if(x0!=xr0 && i0>0)
//            {
//              double d=xr0-x0;
//              curDelta+=d;
//              curSquaredIntegral+=HEF::square(a0/d)*d;
//              curIntegral+=a0;
//            }
//            if(x1!=xr1 && i1<s)
//            {
//              double d=x1-xr1;
//              curDelta+=d;
//              curSquaredIntegral+=HEF::square(b1/d)*d;
//              curIntegral+=b1;
//            }
//            // We have at least a whole interval included
//            if(ret==DataFile::Error::Success)
//              ret=data->getStatFast(i0, i1-i0, min?&curMin:nullptr, max?&curMax:nullptr, (integral||avg)?&curIntegral:nullptr, (integralSquared||rms)?&curSquaredIntegral:nullptr);
//          }
//          else if(i0>0 && i1<s) // i0==i1
//          {
//////            qDebug()<<"Inter point value"b1<<b0;
//            curDelta+=x1-x0;
//            curIntegral+=HEF::integralSawtooth(x0,y0,x1,y1);//b1-b0;
//            curSquaredIntegral+=HEF::integralSquaredSawtooth(x0,y0,x1,y1);
//////            qDebug()<<"Monorange"<<i1<<i0;
//          }
//        }
//        cacheIndex0=i0; cacheX0=x0; cacheY0=y0; cacheA0=a0; cacheB0=b0;
//        cacheIndex1=i1; cacheX1=x1; cacheY1=y1; cacheA1=a1; cacheB1=b1;
//      }
//      if(min)
//        min[i]=curMin;
//      if(max)
//        max[i]=curMax;
//      if(delta)
//        delta[i]=curDelta;
//      if(integral)
//        integral[i]=curIntegral;
//      if(integralSquared)
//        integralSquared[i]=curSquaredIntegral;
//      if(avg)
//        avg[i]=curDelta>0.?curIntegral/curDelta:qQNaN();
//      if(rms)
//        rms[i]=curDelta>0.?sqrt(curSquaredIntegral/curDelta):qQNaN();
//    }
//  }
//  return ret;
//}


static DataFile::Error getStatOptim2(DataFile::DataSource2D *data, QList<HEF::Range> ranges2, double *min, double *max, double *avg, double *rms, double *delta, double *integral, double *integralSquared, double *yValue, qint64 *numRanges)
{
  DataFile::Error ret=DataFile::Error::Generic;
  qint64 s;
  ret=data->size(s);
  auto d=data->domain();
  if(ret==DataFile::Error::Success)
  {
    auto ranges=ranges2;
    ranges&=d; // Optimized join with domain

    struct IndexCache
    {
      IndexCache() {x=y=integralBefore=integralAfter=integralSquaredBefore=integralSquaredAfter=qQNaN(); index=-1;}
      double x;
      double y;
      double integralBefore, integralAfter;
      double integralSquaredBefore, integralSquaredAfter;
      double xBefore, xAfter;
      qint64 index;
    } c0, c1;

    for(int i=0;i<ranges.size() && ret==DataFile::Error::Success;i++)
    {
      auto &cr=ranges[i];
      qreal curMin=qQNaN(), curMax=qQNaN(), curIntegral=0., curSquaredIntegral=0., curDelta=0., curValue=qQNaN();
      qint64 curNumItem=0;
      for(int j=0;j<cr.numBasicRanges();j++)
      {
        double x0=cr.basicRange(j).min(), x1=cr.basicRange(j).max();
        bool recache0=true, recache1=true;
        if(x1==c0.x) // Check if we have already data in cache
        {
          c1=c0;
          recache1=false;
        }
        else if(x0==c1.x)
        {
          c0=c1;
          recache0=false;
        }
        if(recache0)
        {
          c0.x=x0;
          if((ret=data->indexLEFast(x0, c0.index, 0, s))!=DataFile::Error::Success)
            break;
          if(c0.index<0)
            c0.index=0;
          if((ret=data->yInterpolateFast(c0.index, x0, c0.y, (avg||integral)?&c0.integralBefore:nullptr, (avg||integral)?&c0.integralAfter:nullptr, (rms||integralSquared)?&c0.integralSquaredBefore:nullptr, (rms||integralSquared)?&c0.integralSquaredAfter:nullptr, &c0.xBefore, &c0.xAfter))!=DataFile::Error::Success)
            break;
        }
        if(recache1)
        {
          c1.x=x1;
          if((ret=data->indexLEFast(x1, c1.index, 0, s))!=DataFile::Error::Success)
            break;
          if((ret=data->yInterpolateFast(c1.index, x1, c1.y, (avg||integral)?&c1.integralBefore:nullptr, (avg||integral)?&c1.integralAfter:nullptr, (rms||integralSquared)?&c1.integralSquaredBefore:nullptr, (rms||integralSquared)?&c1.integralSquaredAfter:nullptr, &c1.xBefore, &c1.xAfter))!=DataFile::Error::Success)
            break;
        }
        curMin=std::fmin(c0.y,curMin);
        curMax=std::fmax(c0.y,curMax);
        curMin=std::fmin(c1.y,curMin);
        curMax=std::fmax(c1.y,curMax);
        if(c1.index==c0.index)
        {
          if(qIsFinite(c0.xBefore) && qIsFinite(c1.xBefore))
          {
            curIntegral+=c1.integralBefore-c0.integralBefore;
            curSquaredIntegral+=c1.integralSquaredBefore-c0.integralSquaredBefore;
            curDelta+=c1.xBefore-c0.xBefore;
          }
          else if(qIsFinite(c0.xBefore) && qIsFinite(c1.xBefore))
          {
            curIntegral+=c0.integralAfter-c1.integralAfter;
            curSquaredIntegral+=c0.integralSquaredAfter-c1.integralSquaredAfter;
            curDelta+=c0.xBefore-c1.xAfter;
          }
          curNumItem+=1;
        }
        else if(c1.index==c0.index+1)
        {
          curIntegral+=c0.integralAfter+c1.integralBefore;
          curSquaredIntegral+=c0.integralSquaredAfter+c1.integralSquaredBefore;
          curDelta+=c0.xAfter+c1.xBefore;
//          ret=data->getStatFast(c0.index+1, c1.index-c0.index, min?&curMin:nullptr, max?&curMax:nullptr, (integral||avg)?&curIntegral:nullptr, (integralSquared||rms)?&curSquaredIntegral:nullptr, &curDelta);
          curNumItem+=2;
          if((ret=data->y(c1.index,curValue))!=DataFile::Error::Success)
            break;
          curMin=std::fmin(curValue,curMin);
          curMax=std::fmax(curValue,curMax);
        }
        else if(c1.index>c0.index)
        {
          curIntegral+=c0.integralAfter+c1.integralBefore;
          curSquaredIntegral+=c0.integralSquaredAfter+c1.integralSquaredBefore;
          curDelta+=c0.xAfter+c1.xBefore;
          ret=data->getStatFast(c0.index+1, c1.index-c0.index, min?&curMin:nullptr, max?&curMax:nullptr, (integral||avg)?&curIntegral:nullptr, (integralSquared||rms)?&curSquaredIntegral:nullptr, &curDelta);
          curNumItem+=c1.index-c0.index+2;
        }
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
      if(numRanges)
        numRanges[i]=curNumItem;
      if(yValue)
        yValue[i]=(curNumItem==2)?curValue:qQNaN();
    }
  }
  return ret;
}

////Error DataSource2D::getStat(const QList<HEF::Range> &ranges, double *avg, double *min, double *max, double *rms, double *integral, double *integralSquared)
//static DataFile::Error getStatOptim(DataFile::DataSource2D *data, const QList<HEF::Range> &ranges, QVector<double> *min=nullptr, QVector<double> *max=nullptr, QVector<double> *avg=nullptr, QVector<double> *rms=nullptr, QVector<double> *delta=nullptr, QVector<double> *integral=nullptr, QVector<double> *integralSquared=nullptr)
//{
//  if(min)
//    min->fill(qQNaN(), ranges.size());
//  if(max)
//    max->fill(qQNaN(), ranges.size());
//  if(avg)
//    avg->fill(qQNaN(), ranges.size());
//  if(rms)
//    rms->fill(qQNaN(), ranges.size());
//  if(delta)
//    delta->fill(qQNaN(), ranges.size());
//  if(integral)
//    integral->fill(qQNaN(), ranges.size());
//  if(integralSquared)
//    integralSquared->fill(qQNaN(), ranges.size());
//  if(yValue)
//    yValue->fill(qQNaN(), ranges.size());
//  if(numRanges)
//    numRanges->fill(-1, ranges.size());
//  return getStatOptim2(data, ranges, min?min->data():nullptr, max?max->data():nullptr, avg?avg->data():nullptr, rms?rms->data():nullptr, delta?delta->data():nullptr, integral?integral->data():nullptr, integralSquared?integralSquared->data():nullptr, yValue?yValue->data():nullptr, numRanges?numRanges->data():nullptr);
//}

static DataFile::Error getStatOptim2(DataFile::DataSource2D *data, const QList<HEF::Range> &ranges, QVector<double> *min=nullptr, QVector<double> *max=nullptr, QVector<double> *avg=nullptr, QVector<double> *rms=nullptr, QVector<double> *delta=nullptr, QVector<double> *integral=nullptr, QVector<double> *integralSquared=nullptr, QVector<double> *yValue=nullptr, QVector<qint64> *numRanges=nullptr)
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
  if(yValue)
    yValue->fill(qQNaN(), ranges.size());
  if(numRanges)
    numRanges->fill(-1, ranges.size());
  return getStatOptim2(data, ranges, min?min->data():nullptr, max?max->data():nullptr, avg?avg->data():nullptr, rms?rms->data():nullptr, delta?delta->data():nullptr, integral?integral->data():nullptr, integralSquared?integralSquared->data():nullptr, yValue?yValue->data():nullptr, numRanges?numRanges->data():nullptr);
}

void testPlotStat2();
void testPlotStat2()
{
  QVector<double> avg, min, max, delta;
  getStatOptim2(testSource2.data(), QList<HEF::Range>({HEF::Range(108.5,109.5)}), &min, &max, &avg, nullptr, &delta);
  qDebug()<<min<<max<<avg<<delta;

//  // The integral of this call fails
//  QVector<double> avg, min, max, delta;
//  getStatOptim2(testSource2.data(), QList<HEF::Range>({HEF::Range(108.5,109.5)}), &min, &max, &avg, nullptr, &delta);
//  qDebug()<<min<<max<<avg<<delta;
}

void testPlotStat();
void testPlotStat()
{
  /* 3/166 divisions / 10000x
     getStat: 10000 cycles with total of  330.841  s / average of 33084.1 uS per run

  */
  {
    constexpr int numDiv=1000, nTest=10000;
    constexpr double vMin=3, vMax=166;
    QList<double> boundary;
    for(int i=0;i<=numDiv;i++)
      boundary.append((vMax-vMin)*i/numDiv+vMin);

    auto ranges=HEF::linearGainOffset(1, 0)->preimage(boundary);
    QVector<double> avg, min, max;
    QElapsedTimer timer;
    timer.start();
    for(int j=0;j<nTest;j++)
    {
      auto r2=ranges;
      getStatOptim2(testSource2.data(), r2, &min, &max, &avg);
//      break;
    }
//      testSource2->getStat(ranges, &min, &max, &avg);
    double us=timer.elapsed()*1000.;
    qDebug()<<nTest<<"cycles with total of "<<us/1e6<<" s / average of"<<us/nTest<<"uS per run";
    qDebug()<<avg.size()<<min.size()<<max.size();
  }
}


Area2D::Area2D()
{
  m_data=testSource2;
  m_plotResolution=3;
  m_maxPrepare=m_maxGetStat=m_maxPaint=0;
}


void Area2D::paintOptimized(QPainter &painter, bool screen)
{
  auto bandX=findAncestorPostDFS<BandScale>(parentH(), false);
  auto bandY=findAncestorPostDFS<BandScale>(parentV(), false);
  auto scaleX=bandX?bandX->function():nullptr;
  auto scaleY=bandY?bandY->function():nullptr;
//  qint64 t0,t1,t2,t3;
  if(scaleX && scaleY && m_data)
  {
    auto size=this->size();
    painter.setClipRect(QRectF(QPointF(0,0), size), Qt::IntersectClip);
    qint64 nDiv=qBound(1., std::ceil(size.width()/m_plotResolution), 5000.);
    double delta=size.width()/nDiv;
    QList<double> boundary, center;
    boundary.reserve(nDiv+2);
    center.reserve(nDiv+1);
    for(int j=0;j<=nDiv+1;j++)
      boundary.append((j-0.5)*delta);
    for(int j=0;j<=nDiv;j++)
      center.append(j*delta);
    auto ranges=scaleX->preimage(boundary);
    QVector<double> avg, min, max, value;
    QVector<qint64> numRanges;
    getStatOptim2(m_data.data(), ranges, &min, &max, &avg, nullptr, nullptr, nullptr, nullptr, &value, &numRanges);
//    qDebug()<<numRanges;
//    PlotArea2DSpanPainter span(0, size.width());
    PlotArea2DSpanPainterV3 span(0, size.height());
    PlotArea2DLinePainter stroke(0, size.height());
    PlotArea2DPointPainter point(0, size.height());
//    qDebug()<<size.height();
    for(int i=0;i<center.size();i++)
    {
      double avgY=qIsNaN(avg[i])?qQNaN():scaleY->f(avg[i]);
      double valY=qIsNaN(value[i])?qQNaN():scaleY->f(value[i]);
      HEF::Range rangeY=HEF::Range(min[i], max[i]);
      auto nr=numRanges[i];
      if(!rangeY.isEmpty())
        rangeY=scaleY->image(rangeY);
//      span.feed(painter, center[i], scaleY->image(HEF::Range(min[i], max[i])));
      span.feed(painter, center[i], rangeY, nr==1?avgY:(nr==2?scaleY->f(value[i]):qQNaN()));
      stroke.feed(painter, center[i], nr==2?valY:avgY);
      point.feed(painter, center[i], valY, numRanges[i]);
//      if(numRanges[i]==2)
//      {
//        painter.setBrush(Qt::green);
//        painter.setPen(Qt::green);
//        painter.drawEllipse(QPointF(center[i],scaleY->f(value[i])),2,2);
//      }

    }
    span.finish(painter);
    stroke.finish(painter);
    point.finish(painter);
  }
}

void Area2D::paint(QPainter &painter, bool screen)
{
  paintOptimized(painter, screen);
  return;
  auto bandX=findAncestorPostDFS<BandScale>(parentH(), false);
  auto bandY=findAncestorPostDFS<BandScale>(parentV(), false);
  auto scaleX=bandX?bandX->function():nullptr;
  auto scaleY=bandY?bandY->function():nullptr;
  qint64 t0,t1,t2,t3;
  if(scaleX && scaleY && m_data)
  {
    auto size=this->size();
    painter.setClipRect(QRectF(QPointF(0,0), size), Qt::IntersectClip);
    qint64 nDiv=qBound(1., std::ceil(size.width()/m_plotResolution), 5000.);
    double delta=size.width()/nDiv;
    t0=highPrecisionTime();
    QList<double> boundary, center;
    for(int j=0;j<=nDiv+1;j++)
      boundary.append((j-0.5)*delta);
    for(int j=0;j<=nDiv;j++)
      center.append(j*delta);
    auto ranges=scaleX->preimage(boundary);
    t1=highPrecisionTime();
    Q_ASSERT(ranges.size()==center.size());
    QVector<double> avg, min, max;
//    m_data->getStat(ranges, &min, &max, &avg);
    getStatOptim2(m_data.data(), ranges, &min, &max, &avg);
    for(int i=0;i<avg.size();i++)
    {
      if(qAbs(avg[i])>100)
        qDebug()<<avg[i]<<ranges[i].toString();
    }
//    qDebug()<<avg;
    t2=highPrecisionTime();
    PlotArea2DSpanPainter span(0, size.width());
    PlotArea2DLinePainter stroke(0, size.width());
    for(int i=0;i<center.size();i++)
    {
      span.feed(painter, center[i], scaleY->image(HEF::Range(min[i], max[i])));
      stroke.feed(painter, center[i], scaleY->f(avg[i]));

    }
    span.finish(painter);
    stroke.finish(painter);
    t3=highPrecisionTime();
    qint64 timePrepare=t1-t0, timeGetStat=t2-t1,timePaint=t3-t2;
    if(timePrepare>m_maxPrepare)
    {
      m_maxPrepare=timePrepare;
      qDebug()<<"Worst prepare"<<m_maxPrepare*1e6/highPrecisionFreq()<<"us"<<nDiv;
    }
    if(timeGetStat>m_maxGetStat)
    {
      m_maxGetStat=timeGetStat;
      qDebug()<<"Worst get stat"<<m_maxGetStat*1e6/highPrecisionFreq()<<"us";
    }
    if(timePaint>m_maxPaint)
    {
      m_maxPaint=timePaint;
      qDebug()<<"Worst paint"<<m_maxPaint*1e6/highPrecisionFreq()<<"us";
    }//    qDebug()<<"Prepare"<<t1-t0<<"GetStat"<<t2-t1<<"Paint"<<t3-t2;
  }
}

void PlotArea2DSpanPainter::feed(QPainter &painter, double x, const HEF::Range &range)
{
  if(!range.isEmpty())
  {
    double d=m_maxY-m_minY;
    double curMin=qBound(range.min(), m_minY-2*d, m_maxY+2*d), curMax=qBound(range.max(), m_minY-2*d, m_maxY+2*d);
    m_min.append(curMin);
    m_max.append(curMax);
    m_x.append(x);
  }
  else if(!m_x.isEmpty())
  {
    QPainterPath filled;
    filled.moveTo(m_x[0], m_min[0]);
    for(int i=1;i<m_x.size();i++)
    {
      filled.lineTo(m_x[i], m_min[i]);
    }
    for(int i=m_x.size()-1;i>=0;i--)
    {
      filled.lineTo(m_x[i], m_max[i]);
    }
    filled.lineTo(m_x[0], m_min[0]);
    painter.fillPath(filled, QBrush(QColor(255,0,0,64)));
    m_x.clear();
    m_min.clear();
    m_max.clear();
  }
}

void PlotArea2DSpanPainter::finish(QPainter &painter)
{
  feed(painter, qQNaN(), HEF::Range());
}

void PlotArea2DLinePainter::feed(QPainter &painter, double x, double y)
{
  if(!qIsNaN(y))
  {
    double d=m_maxY-m_minY;
    y=qBound(y, m_minY-2*d, m_maxY+2*d);
    m_x.append(x);
    m_y.append(y);
  }
  else if(!m_x.isEmpty())
  {
    QPainterPath stroked;
    stroked.moveTo(m_x[0], m_y[0]);
    for(int i=(m_x.size()>1);i<m_x.size();i++)
    {
      stroked.lineTo(m_x[i], m_y[i]);
    }
    painter.strokePath(stroked, QPen(QColor(255,0,0),1));
    m_x.clear();
    m_y.clear();
  }
}

void PlotArea2DLinePainter::finish(QPainter &painter)
{
  feed(painter, qQNaN(), qQNaN());
}

void PlotArea2DPointPainter::feed(QPainter &painter, double x, double y, qint64 numRanges)
{
  constexpr double minDistance=7.99;

  if(!qIsNaN(m_lastY) && x-m_lastX>=minDistance)
  {
//    qDebug()<<"Stroke"<<m_lastX<<m_lastY<<x<<y;
    strokePoint(painter, m_lastX, m_lastY);
    m_lastY=qQNaN();
  }

  if(numRanges==2)
  {
    m_lastY=(qIsNaN(m_lastX)||x-m_lastX>=minDistance)?y:qQNaN();
    m_lastX=x;
//    qDebug()<<"Potential point"<<x<<y<<m_lastY;
  }
  else if(numRanges>2)
  {
    m_lastX=x;
    m_lastY=qQNaN();
  }
}

void PlotArea2DPointPainter::finish(QPainter &painter)
{
  if(!qIsNaN(m_lastY))
    strokePoint(painter, m_lastX, m_lastY);
}

void PlotArea2DPointPainter::strokePoint(QPainter &painter, double x, double y)
{
  if(y>=m_minY && y<=m_maxY)
  {
    painter.setBrush(Qt::red);
    painter.setPen(Qt::red);
    painter.drawEllipse(QPointF(x,y),3,3);
  }
}

void PlotArea2DSpanPainterV2::feed(QPainter &painter, double x, const HEF::Range &yRange, double yVal)
{
  painter.setPen(QPen(QColor(255,0,0,64), 0));
  if(qIsNaN(x) || !qIsNaN(yVal) || yRange.isEmpty())
  {
    double d=m_maxY-m_minY;
//    auto limit=[this,d](double v){return qBound(m_minY-2*d,v,m_maxY+2*d);};
    auto limit=[this,d](double v){return qBound(m_minY,v,m_maxY);};
    if(m_x.isEmpty())
    {
      if(!qIsNaN(x) && !qIsNaN(yVal) && !qIsNaN(m_firstY))
        painter.drawLine(QPointF(m_firstX,limit(m_firstY)), QPointF(x, limit(yVal)));
    }
    else
    {
      QPointF firstPoint;
      QPainterPath filled;
      if(!qIsNaN(m_firstX))
        firstPoint=QPointF(m_firstX, limit(m_firstY));
      else
        firstPoint=QPointF(m_x[0], limit(m_min[0]));
      filled.moveTo(firstPoint);
      for(int i=0;i<m_x.size();i++)
        filled.lineTo(m_x[i], limit(m_min[i]));
      if(!qIsNaN(x) && !qIsNaN(yVal))
        filled.lineTo(x, limit(yVal));
      for(int i=m_x.size()-1;i>=0;i--)
        filled.lineTo(m_x[i], limit(m_max[i]));
      filled.lineTo(firstPoint);
      painter.fillPath(filled, QBrush(QColor(255,0,0,64)));
      m_x.clear();
      m_min.clear();
      m_max.clear();
    }
    m_firstX=m_firstY=qQNaN();
  }
  if(!qIsNaN(x))
  {

    if(!qIsNaN(yVal))
    {
      m_firstX=x;
      m_firstY=yVal;
    }
    else if(!yRange.isEmpty())
    {
      m_x.append(x);
      m_min.append(yRange.min());
      m_max.append(yRange.max());
    }
  }
}

void PlotArea2DSpanPainterV3::feed(QPainter &painter, double x, const HEF::Range &yRange, double yVal)
{
  painter.setPen(QPen(QColor(255,0,0,64), 0));
  double d=m_maxY-m_minY;
  auto limit=[this,d](double v){return qBound(m_minY-2*d,v,m_maxY+2*d);};

  auto addPoint=[this, x, yRange, yVal, &limit]()
  {
    if(!qIsNaN(x))
    {
      if(!qIsNaN(yVal))
      {
        m_x.append(x);
        m_min.append(limit(yVal));
        m_max.append(limit(yVal));
      }
      else if(!yRange.isEmpty())
      {
        m_x.append(x);
        m_min.append(limit(yRange.min()));
        m_max.append(limit(yRange.max()));
      }
    }
  };
  addPoint();
  if((qIsNaN(x) || !qIsNaN(yVal) || yRange.isEmpty()) && !m_x.isEmpty())
  {
    auto xMax=m_x, xMin=m_x;
//    const double maxAngle=85, maxSlope=tan(maxAngle*M_PI/180.);
    double maxSlope=d/10.; // px/px
    simplify(xMax, m_max, maxSlope, true);
    simplify(xMin, m_min, maxSlope, false);
    if(xMax.size()>=2 && xMin.size()>=2)
    {
      QVector<QPointF> points;
      points.reserve(xMax.size()+xMin.size());
      for(int i=0;i<xMax.size();i++)
        points.append(QPointF(xMax[i], limit(m_max[i])));
      for(int i=xMin.size()-1;i>=0;i--)
      {
        QPointF cur=QPointF(xMin[i], limit(m_min[i]));
        if(cur!=points.last() && cur!=points.first())
          points.append(cur);
      }
      QPainterPath p;
//      p.setFillRule(Qt::WindingFill);
      p.moveTo(points.first());
      for(int i=1;i<points.size();i++)
        p.lineTo(points[i]);
      p.lineTo(points.first());
      painter.fillPath(p, QBrush(QColor(255,0,0,64)));
    }
    m_x.clear();
    m_max.clear();
    m_min.clear();
    addPoint();
  }
}
//void PlotArea2DSpanPainterV3::simplifyMax(QVector<double> &x, QVector<double> &y, double maxSlope)
//{
//  Q_ASSERT(x.size()==y.size());
//  double reducedMaxSlope=maxSlope*0.99;
//  enum Policy
//  {
//    Keep, // Keep all three points
//    Remove1, // Remove central point
//    SetSlopeBoth, // Interpolate left and right side to maxSlope
//    SetSlopeLeft,  // Set slope of left side only to maxSlope
//    SetSlopeRight // Set slope of right side only to maxSlope
//  } policy;
//  enum SlopeType
//  {
//    OverSlope, // Slope > max slope
//    NormalSlope,    // -max slope <= slope <= max slope
//    UnderSlope // Slope < -max slope
//  };
//  auto slope=[](double x0, double y0, double x1, double y1){return (y1-y0)/(x1-x0);};
//  auto classify=[maxSlope](double slope){return slope>maxSlope?OverSlope: (slope<-maxSlope?UnderSlope:NormalSlope);};
//  if(x.size()>=2)
//  {
//    int lastIndex=1;
//    for(int readIndex=2;readIndex<x.size();)
//    {
//      double slope01=slope(x[lastIndex-1], y[lastIndex-1], x[lastIndex], y[lastIndex]), slope12=slope(x[lastIndex], y[lastIndex], x[readIndex], y[readIndex]), slope02=slope(x[lastIndex-1], y[lastIndex-1], x[readIndex], y[readIndex]);
//      SlopeType type01=classify(slope01), type12=classify(slope12);
//      qDebug()<<lastIndex-1<<"("<<x[lastIndex-1]<<y[lastIndex-1]<<") ;"<<lastIndex<<"("<<x[lastIndex]<<y[lastIndex]<<") ;"<<readIndex<<"("<<x[readIndex]<<y[readIndex]<<"): "<<slope01;
//      switch(type12)
//      {
//      case UnderSlope:
//        switch(type01)
//        {
//        case UnderSlope:
//          policy=(slope01>slope12)?Keep:Remove1; break;
//        case NormalSlope:
//          policy=Keep; break;
//        case OverSlope:
//          policy=Keep; break;
//        }
//        break;
//      case NormalSlope:
//        switch(type01)
//        {
//        case UnderSlope:
//          policy=(classify(slope02)==NormalSlope)?SetSlopeLeft:Remove1; break;
//        case NormalSlope:
//          policy=Keep; break;
//        case OverSlope:
//          policy=Keep; break;
//        }
//        break;
//      case OverSlope:
//        switch(type01)
//        {
//        case UnderSlope:
//          policy=(classify(slope02)==NormalSlope)?SetSlopeBoth:Remove1; break;
//        case NormalSlope:
//          policy=(classify(slope02)==NormalSlope)?SetSlopeRight:Remove1; break;
//        case OverSlope:
//          policy=(slope01>slope12)?Keep:Remove1; break;
//        }
//        break;
//      }
//      switch(policy)
//      {
//      case Keep:
//        lastIndex++;
//        if(lastIndex!=readIndex)
//        {
//          x[lastIndex]=x[readIndex];
//          y[lastIndex]=y[readIndex];
//        }
//        readIndex++;
//        break;
//      case Remove1:
//        if(lastIndex!=1)
//        {
//          lastIndex--; // Remove the last element and repeats the test
//        }
//        else
//        {
//          x[1]=x[readIndex];
//          y[1]=y[readIndex];
//          readIndex++;
//        }
//        break;
//      case SetSlopeLeft:
//      case SetSlopeRight:
//      case SetSlopeBoth:
//        double x0=x[lastIndex-1], y0=y[lastIndex-1], x2=x[readIndex], y2=y[readIndex];
//        double dx=x2-x0, dy=y2-y0;
//        double slopeLeft=qQNaN(), slopeRight=qQNaN();
//        // System: {y=slopeLeft*x y=slopeRight(x-dx)+dy } => { ... slopeRight(x-dx)+dy-slopeLeft*x=0 } => { ... x(slopeRight-slopLeft)=-dy+slopeRight*dx } {x=(slopeRight*dx-dy)/(slopeRight-slopeLeft); y=slopeLeft*x; }
//        switch(policy)
//        {
//        case SetSlopeBoth:
//          slopeLeft=-reducedMaxSlope; slopeRight=reducedMaxSlope; break;
//        case SetSlopeLeft:
//          slopeLeft=-reducedMaxSlope; slopeRight=slope12; break;
//        case SetSlopeRight:
//          slopeLeft=slope01; slopeRight=reducedMaxSlope; break;
//        case Keep:
//        case Remove1:
//          break; // Note: unreachable
//        }
//        double dx1=(slopeRight*dx-dy)/(slopeRight-slopeLeft), x1=dx1+x0, y1=y0+slopeLeft*dx1;
//        if(lastIndex!=1)
//        {
//          x[readIndex-1]=x1;
//          y[readIndex-1]=y1;
//          readIndex--;
//          lastIndex--;
//        }
//        else
//        {
//          x[lastIndex]=x1;
//          y[lastIndex]=y1;
//        }
//        break;
//      }
//    }
//    x.resize(lastIndex+1);
//    y.resize(lastIndex+1);
//  }
//}

/*
 Test caseses for max (invert y signs to check min, should also work when mirrored)
  // Test1
  //  QVector<double> x({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}), y({0, -2, 5, 4, 1, 8.2, 1, 4, 5, -2, 0});
  //  QVector<double> expectedX({0, 2, 5, 8, 10}), expectedY({0, 5, 8.2, 5, 0});
  // Test 2
  //  QVector<double> x({0,1,2,3,4,5,6,7,8,9,10, 11, 12}), y({0, 1, 1.1, 10, 3, 2, 11, 10, 11, 2, 5, 3, 8});
  //  QVector<double> expectedX({0, 3, 4, 6, 7, 8, 11.5, 12}), expectedY({0, 10, 9, 11, 10, 11, 7.5, 8});
  // Test 3
  //  QVector<double> x({-1,0,1,2,3,4,14}), y({-2, 0, -2, -5, -9, -30, 2});
  //  QVector<double> expectedX({-1, 0, 6, 14}), expectedY({-2, 0, -6, 2});

  QVector<double> x(...), y(...);
  // QVector<double> expectedX({-1, 0, 6, 14}), expectedY({-2, 0, -6, 2});
  Plot::PlotArea2DSpanPainterV3::simplify(x, y, 1, false);
  for(int i=0;i<x.size();i++)
    qDebug()<<i<<x[i]<<y[i];
  qDebug()<<"Result"<<x<<y;
  exit(1);

*/
void PlotArea2DSpanPainterV3::simplify(QVector<double> &x, QVector<double> &y, double maxSlope, bool max)
{
  Q_ASSERT(x.size()==y.size());
  int sign=max?1:-1;
  double reducedMaxSlope=maxSlope*0.99;
  enum Policy
  {
    Keep, // Keep all three points
    Remove1, // Remove central point
    SetSlopeBoth, // Interpolate left and right side to maxSlope
    SetSlopeLeft,  // Set slope of left side only to maxSlope
    SetSlopeRight // Set slope of right side only to maxSlope
  } policy;
  enum SlopeType
  {
    OverSlope, // Slope > max slope
    NormalSlope,    // -max slope <= slope <= max slope
    UnderSlope // Slope < -max slope
  };
  auto slope=[sign](double x0, double y0, double x1, double y1){return sign*(y1-y0)/(x1-x0);};
  auto classify=[maxSlope](double slope){return slope>maxSlope?OverSlope: (slope<-maxSlope?UnderSlope:NormalSlope);};
  if(x.size()>=2)
  {
    int lastIndex=1;

    for(int readIndex=2,cycle=0;readIndex<x.size();cycle++)
    {
      double slope01=slope(x[lastIndex-1], y[lastIndex-1], x[lastIndex], y[lastIndex]), slope12=slope(x[lastIndex], y[lastIndex], x[readIndex], y[readIndex]), slope02=slope(x[lastIndex-1], y[lastIndex-1], x[readIndex], y[readIndex]);
      SlopeType type01=classify(slope01), type12=classify(slope12);
//      qDebug()<<cycle<<lastIndex-1<<"("<<x[lastIndex-1]<<y[lastIndex-1]<<") ;"<<lastIndex<<"("<<x[lastIndex]<<y[lastIndex]<<") ;"<<readIndex<<"("<<x[readIndex]<<y[readIndex]<<"): "<<slope01;
      switch(type12)
      {
      case UnderSlope:
        switch(type01)
        {
        case UnderSlope:
          policy=(slope01>slope12)?Keep:Remove1; break;
        case NormalSlope:
          policy=Keep; break;
        case OverSlope:
          policy=Keep; break;
        }
        break;
      case NormalSlope:
        switch(type01)
        {
        case UnderSlope:
          policy=(classify(slope02)==NormalSlope)?SetSlopeLeft:Remove1; break;
        case NormalSlope:
          policy=Keep; break;
        case OverSlope:
          policy=Keep; break;
        }
        break;
      case OverSlope:
        switch(type01)
        {
        case UnderSlope:
          policy=(classify(slope02)==NormalSlope)?SetSlopeBoth:Remove1; break;
        case NormalSlope:
          policy=(classify(slope02)==NormalSlope)?SetSlopeRight:Remove1; break;
        case OverSlope:
          policy=(slope01>slope12)?Keep:Remove1; break;
        }
        break;
      }
      // Note: two steps (slope joining may give incorrect results due to numerical instabilities)
      switch(policy)
      {
      default:
        break;
      case SetSlopeLeft:
      case SetSlopeRight:
      case SetSlopeBoth:
        double x0=x[lastIndex-1], y0=y[lastIndex-1], x2=x[readIndex], y2=y[readIndex];
        double dx=x2-x0, dy=y2-y0;
        double slopeLeft=qQNaN(), slopeRight=qQNaN();
        // System: {y=slopeLeft*x y=slopeRight(x-dx)+dy } => { ... slopeRight(x-dx)+dy-slopeLeft*x=0 } => { ... x(slopeRight-slopLeft)=-dy+slopeRight*dx } {x=(slopeRight*dx-dy)/(slopeRight-slopeLeft); y=slopeLeft*x; }
        switch(policy)
        {
        case SetSlopeBoth:
          slopeLeft=-sign*reducedMaxSlope; slopeRight=sign*reducedMaxSlope; break;
        case SetSlopeLeft:
          slopeLeft=-sign*reducedMaxSlope; slopeRight=sign*slope12; break;
        case SetSlopeRight:
          slopeLeft=sign*slope01; slopeRight=sign*reducedMaxSlope; break;
        case Keep:
        case Remove1:
          break; // Note: unreachable
        }
        double dx1=(slopeRight*dx-dy)/(slopeRight-slopeLeft), x1=dx1+x0, y1=y0+slopeLeft*dx1;
        if(dx1<=0 || dx1>=dx)
          policy=Remove1; // Numerical instabilities lead us to a wrong point (very similar left and right slopes). Just ignores the point.
        else
        {
          if(lastIndex!=1)
          {
            x[readIndex-1]=x1;
            y[readIndex-1]=y1;
            readIndex--;
            lastIndex--;
          }
          else
          {
            x[lastIndex]=x1;
            y[lastIndex]=y1;
          }
          break;
        }
      }

      switch(policy)
      {
      case Keep:
        lastIndex++;
        if(lastIndex!=readIndex)
        {
          x[lastIndex]=x[readIndex];
          y[lastIndex]=y[readIndex];
        }
        readIndex++;
        break;
      case Remove1:
        if(lastIndex!=1)
        {
          lastIndex--; // Remove the last element and repeats the test
        }
        else
        {
          x[1]=x[readIndex];
          y[1]=y[readIndex];
          readIndex++;
        }
        break;
      case SetSlopeBoth:
      case SetSlopeLeft:
      case SetSlopeRight:
        break;
      }
    }
    x.resize(lastIndex+1);
    y.resize(lastIndex+1);
  }
}
