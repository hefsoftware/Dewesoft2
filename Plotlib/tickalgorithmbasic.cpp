#include "tickalgorithmbasic.h"
#include "ticklist.h"
#include <QtMath>
#include <QDebug>

QList<TickAlgorithmBasic::LogDivision> TickAlgorithmBasic::m_logOptimizedDiv;
QList<TickAlgorithmBasic::LogDivision> TickAlgorithmBasic::m_logBackupDiv;


TickAlgorithmBasic::TickAlgorithmBasic(const HEF::FunctionPtr &function, double min, double max, TickList *result): m_function(function), m_result(result), m_min(min), m_max(max), m_delta(max-min)
{
  m_minMainDistance=50;
  m_minSubDistance=qQNaN();
}

HEF::Range TickAlgorithmBasic::fullRange()
{
  return HEF::Range(m_min-m_delta*epsilon(), m_max+m_delta*epsilon());
}

bool TickAlgorithmBasic::recalculate()
{
  bool ok=false;
  m_ticks.clear();
  auto fullrange=fullRange();
  if(m_function && m_max>m_min)
  {
    m_delta=m_max-m_min;
    auto preimage=m_function->preimage(fullrange);
    HEF::Range image=m_function->image(preimage);
    TickListInserter insert(m_ticks, fullrange, preimage, m_function);
    if(preimage.range()>0 && image.range()>0 && preimage.isFinite() && image.isFinite())
    {
      if(image.range()>m_minMainDistance)
      {
        double k;
        if((k=linearConstant(preimage))!=0. && qIsFinite(k))
          ok=recalculateLinear(insert, k);
        if(!ok && (k=logarithmicConstant(preimage))>0 && qIsFinite(k))
        {
//          qDebug()<<"Logarithmic"<<k;
          ok=recalculateLogarithmic(insert, k);
        }
      }
//          if(!ok)
//        if(!ok) // As last resort tries to find a single mapped point
//          ok=recalculateSinglePoint(image, preimage);
    }
  }
  if(m_result)
  {
    m_result->clear();
    if(ok)
      *m_result=m_ticks;
  }
  return ok;
}


double TickAlgorithmBasic::logarithmicConstant(const HEF::Range &preimage)
{
  double ret;
  ret=logarithmicConstant(preimage, {0.05, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65, 0.75, 0.85, 0.95});
  return ret;
}
double TickAlgorithmBasic::logarithmicConstant(const HEF::Range &preimage, const QList<double> &values)
{
  bool isExponential=false;
  auto image=m_function->image(preimage);
  double yFullDelta=(image.max()-image.min()), yMin=yFullDelta*values.first()+image.min(), yMax=yFullDelta*values.last()+image.min(), yDelta=yMax-yMin;
  double xMin=m_function->invert(yMin), xMax=m_function->invert(yMax);
  double xRatio=qQNaN();
  if(xMin>0 && xMax>0)
  {
    xRatio=xMax/xMin;
    isExponential=true;
    for(int i=1;i<values.size()-1;i++)
    {
      double cur=values[i];
      qreal curRel=(cur-values.first())/(values.last()-values.first());
      isExponential=(qAbs((curRel*yDelta+yMin)-m_function->f(xMin*pow(xRatio, curRel)))/yFullDelta)<0.01; // Less than 2% error
    }
  }
  return isExponential?yDelta/log(xRatio):qQNaN();
}

void TickAlgorithmBasic::createLinearTicks(TickListInserter &insert, double ticks, int numSubTick, int precision)
{
//  auto fullrange=fullRange();
  if(insert.preimage().min()<=0 && insert.preimage().max()>=0)
  {
    insert.addMain(0., precision);
    double x;
    for(x=0;;x-=ticks)
    {
      for(int i=1;i<numSubTick;i++)
        insert.addSub(x-ticks*i/numSubTick);
      insert.addMain(x, precision);
      if(x<insert.preimage().min())
        break;
    }
    for(x=0;;x+=ticks)
    {
      for(int i=1;i<numSubTick;i++)
        insert.addSub(x+ticks*i/numSubTick);
      insert.addMain(x, precision);
      if(x>insert.preimage().max())
        break;
    }
  }
  else
  {
    double x=floor(insert.preimage().min()/ticks)*ticks;
    for(;;x+=ticks)
    {
      for(int i=1;i<numSubTick;i++)
        insert.addSub(x+ticks*i/numSubTick);
      insert.addMain(x,precision);
      if(x>insert.preimage().max())
        break;
    }
  }
}

bool TickAlgorithmBasic::recalculateLinear(TickListInserter &insert, double k)
{
  bool ret=false;
  const HEF::Range &preimage(insert.preimage());
  double preimageDelta=insert.preimage().range();
  double imageDelta=insert.image().range();
  double kAbs=qAbs(k);
  double deltaPerTick=m_minMainDistance/kAbs/0.9999999;

  if(imageDelta>=m_minMainDistance*1.4) // Easy case: we can always fit at least two ticks
  {
    int order=floor(log10(deltaPerTick));
    double ticks;
    int numSubTick;
    int precision;
    double subTickDist=qQNaN();
    numSubTick=1;
    ret|=recalculateLinearSubTicks(preimageDelta,order+1, 5, kAbs, ticks, numSubTick, subTickDist, precision);
    ret|=recalculateLinearSubTicks(preimageDelta,order+1, 2, kAbs, ticks, numSubTick, subTickDist, precision);
    ret|=recalculateLinearSubTicks(preimageDelta,order+1, 1, kAbs, ticks, numSubTick, subTickDist, precision);
    ret|=recalculateLinearSubTicks(preimageDelta,order+1, 4, kAbs, ticks, numSubTick, subTickDist, precision);
    ret|=recalculateLinearSubTicks(preimageDelta,order+2, 4, kAbs, ticks, numSubTick, subTickDist, precision);
    ret|=recalculateLinearSubTicks(preimageDelta,order+2, 5, kAbs, ticks, numSubTick, subTickDist, precision);
    if(ret)
      createLinearTicks(insert, ticks, numSubTick, precision);
  }
  if(!ret && imageDelta>=m_minMainDistance*1.05) // At least distance of a single tick+5%
  {
    int order=floor(log10(preimageDelta));
    constexpr int bestScoreBase=100000000;
    double best=qQNaN();
    int bestDiv=1;
    int bestScore=bestScoreBase;
    int bestPrecision=0;
    for(int i=0;i<5 && qIsNaN(best); i++, order--)
    {
      double ordP=pow(10, order);
      double testDelta=ceil(deltaPerTick/ordP)*ordP;
      for(int j=0;j<20;j++, testDelta+=ordP)
      {
        if(testDelta>preimageDelta)
          break;
        int curDiv=1;
        int curScore=bestScoreBase-1;
        if(m_minSubDistance>0)
        {
          if(testDelta/ordP<400)
          {
            int n=round(testDelta/ordP);
            if(n==1 || n==2 || n==4 || n==5)
              n*=10;
            for(int div=2;div<=10;div++)
              if(!(n%div) && k*testDelta/div>m_minSubDistance)
              {
                int d=qAbs(div*2-9);
                if(d<curScore)
                  curScore=d+j*10;
                curDiv=div;
              }
          }
        }
        if(curScore<bestScore)
        {
          bestScore=curScore;
          bestDiv=curDiv;
          best=testDelta;
          bestPrecision=order;
        }
      }
    }
    if(!qIsNaN(best))
    {
      double origin=ceil(preimage.min()/best)*best;
      if(!preimage.contains(origin) || (!preimage.contains(origin-best) && !preimage.contains(origin+best)))
      {
        origin=qQNaN();
        int ordOrigin=ceil(log10(preimageDelta));
        for(int i=0; qIsNaN(origin) && i<10;i++, ordOrigin--)
        {
          double ordPOrigin=pow(10,ordOrigin);
          double curOrigin=ceil(preimage.min()/ordPOrigin)*ordPOrigin;
          if(preimage.contains(curOrigin) && (preimage.contains(curOrigin-best) || preimage.contains(curOrigin+best)))
          {
            origin=curOrigin;
            if(ordOrigin<bestPrecision)
              bestPrecision=ordOrigin;
            break;
          }
        }
      }
      if(!qIsNaN(origin))
      {
        for(int i=0;;i++)
        {
          double p=origin+best*i/bestDiv;
          if(p>preimage.max())
            break;
          if(i%bestDiv)
            insert.addSub(p);
          else
            insert.addMain(p, bestPrecision);
        }
        for(int i=1;;i++)
        {
          double p=origin-best*i/bestDiv;
          if(p<preimage.min())
            break;
          if(i%bestDiv)
            insert.addSub(p);
          else
            insert.addMain(p, bestPrecision);
        }
        ret=true;
      }
    }
  }

  return ret;
}

/*
 * Extrema ratio: tries to find a point as close as possible to the center
 */
bool TickAlgorithmBasic::recalculateSinglePoint(TickListInserter &insert)
{
  bool ret=false;
  constexpr int nExtra=10;
  double r=insert.image().range();
  for(int i=0;i<1+2*nExtra;i++)
  {
    double centerY;
    if(i==0)
      centerY=r*.5+insert.image().min();
    else
      centerY=r*(.5+(i%2?1:-1)*((i+1)/2)*.49/nExtra)+insert.image().min();
    double centerX=m_function->invert(centerY);
    if(qIsNaN(centerX))
      continue;
    double v=qQNaN();
    int order=ceil(log10(insert.preimage().maxAbsolute()));
    for(int i=0;qIsNaN(v) && i<5; i++, order--)
    {
      double ordP=pow(10, order);
      double x1=floor(centerX/ordP)*ordP;
      double x2=ceil(centerX/ordP)*ordP;
      double y1=m_function->f(x1), y2=m_function->f(x2);
      double delta1=insert.image().contains(y1)?qAbs(y1-centerY):qQNaN();
      double delta2=insert.image().contains(y2)?qAbs(y2-centerY):qQNaN();
      if(!qIsNaN(delta1) && !(delta1>delta2))
        v=x1;
      else if(!qIsNaN(delta2) && !(delta2>delta1))
        v=x2;
    }
    ret=insert.addMain(qIsNaN(v)?centerX:v);
    if(ret)
      break;
  }
  return ret;

}

double TickAlgorithmBasic::linearConstant(const HEF::Range &preimage)
{
  double ret;
  ret=linearConstant(preimage, {0.05, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65, 0.75, 0.85, 0.95});
  return ret;
}

double TickAlgorithmBasic::linearConstant(const HEF::Range &preimage, const QList<double> &values)
{
  auto xDelta=preimage.max()-preimage.min(), min=preimage.min();
  double vMin=values.first(), vMax=values.last(), vDelta=vMax-vMin;
  double xMin=min+xDelta*vMin, xMax=min+xDelta*vMax;
  double yMin=m_function->f(xMin), yMax=m_function->f(xMax), yDelta=yMax-yMin;
  bool isLinear=true;
  for(int i=1;isLinear && i<values.size()-1;i++)
  {
    double cur=values[i];
    isLinear=(qAbs(m_function->f(min+xDelta*cur)-(yDelta*(cur-vMin)/vDelta+yMin))/yDelta)<0.02; // Less than 2% error is considerated linear
  }
  return isLinear?(yDelta/(xMax-xMin)):qQNaN();
}

bool TickAlgorithmBasic::recalculateLinearSubTicks(double preimageRange, int fullTickOrder, int mainDiv, double k, double &ticks, int &numSubTicks, double &subTickDist, int &precision)
{
  bool ret=false;
  double mainTick=pow(10,fullTickOrder)/mainDiv;
  if(preimageRange>=mainTick*2*0.99999 && mainTick*k>=m_minMainDistance*0.99999)
  {
    recalculateLinearSubTicks(fullTickOrder, mainDiv, k, ticks, numSubTicks, subTickDist, precision);
    ret=true;
  }
  return ret;
}

void TickAlgorithmBasic::recalculateLinearSubTicks(int fullTickOrder, int mainDiv, double k, double &ticks, int &numSubTicks, double &subTickDist, int &precision)
{
  QList<int> tests;
  switch(mainDiv)
  {
    case 1:
      tests=QList<int>{10, 5, 4, 2}; break;
    case 2:
      tests=QList<int>{10,5,2}; break;
    case 4:
      tests=QList<int>{5}; break;
    case 5:
      tests=QList<int>{2}; break;
  }
  double mainFullTick=pow(10,fullTickOrder);
  double numDiv=(mainFullTick/mainDiv)/(m_minSubDistance/k), normNumDiv=numDiv;
  int ordNumDiv=1;


  int idx=0;
  int curNumSub=ordNumDiv;
  for(;idx<tests.size();idx++)
    if(normNumDiv>=tests[idx])
    {
      curNumSub=ordNumDiv*tests[idx];
      break;
    }
  double curSubTickDist=mainFullTick/(mainDiv*curNumSub);
  bool improved=false;

  if(qIsNaN(subTickDist) || subTickDist>curSubTickDist)
    improved=true;
  else if(subTickDist==curSubTickDist && (curNumSub==4 || curNumSub==5) )
  {
    if(numSubTicks!=4 && numSubTicks!=5)
      improved=true;
  }
  if(improved)
  {
    subTickDist=curSubTickDist;
    numSubTicks=curNumSub;
    ticks=mainFullTick/mainDiv;
    precision=fullTickOrder-(mainDiv==1?0:(mainDiv==4?2:1));
  }
}

//bool TickAlgorithmBasic::recalculateLinearSubTicks(double preimageRange, double mainFullTick, int mainDiv, double k, double &ticks, int &numSubTicks, double &subTickDist, int &precision)
//{
//  bool ret=false;
//  double mainTick=mainFullTick/mainDiv;
//  if(preimageRange>=mainTick*2*0.99999 && mainTick*k>=m_minMainDistance*0.99999)
//  {
//    recalculateLinearSubTicks(mainFullTick, mainDiv, k, ticks, numSubTicks, subTickDist, precision);
//    ret=true;
//  }
//  return ret;
//}

//void TickAlgorithmBasic::recalculateLinearSubTicks(double mainFullTick, int mainDiv, double k, double &ticks, int &numSubTicks, double &subTickDist, int &precision)
//{
//  QList<int> tests;
//  switch(mainDiv)
//  {
//    case 1:
//      tests=QList<int>{10, 5, 4, 2}; break;
//    case 2:
//      tests=QList<int>{10,5,2}; break;
//    case 4:
//      tests=QList<int>{5}; break;
//    case 5:
//      tests=QList<int>{2}; break;
//  }
//  double numDiv=(mainFullTick/mainDiv)/(m_minSubDistance/k), normNumDiv=numDiv;
//  int ordNumDiv=1;


//  int idx=0;
//  int curNumSub=ordNumDiv;
//  for(;idx<tests.size();idx++)
//    if(normNumDiv>=tests[idx])
//    {
//      curNumSub=ordNumDiv*tests[idx];
//      break;
//    }
//  double curSubTickDist=mainFullTick/(mainDiv*curNumSub);
//  bool improved=false;

//  if(qIsNaN(subTickDist) || subTickDist>curSubTickDist)
//    improved=true;
//  else if(subTickDist==curSubTickDist && (curNumSub==4 || curNumSub==5) )
//  {
//    if(numSubTicks!=4 && numSubTicks!=5)
//      improved=true;
//  }
//  if(improved)
//  {
//    subTickDist=curSubTickDist;
//    numSubTicks=curNumSub;
//    ticks=mainFullTick/mainDiv;
//  }
//}


void TickAlgorithmBasic::recalculateLogarithmicSubDiv(TickListInserter &insert, double k, double min, double max, int numDiv, int ord, int prevDiv)
{
  double ratioPerTick=exp(m_minMainDistance/k/0.9999999);
  double ratioPerSubTick=exp(m_minSubDistance/k/0.9999999);
  double xMin=insert.preimage().min(), xMax=insert.preimage().max();
  double delta=(max-min)/numDiv;
  int startIndex=0,endIndex=numDiv;
  if(min<xMin)
    startIndex=floor((xMin-min)/delta);
  if(max>xMax)
    endIndex=ceil((xMax-min)/delta);
  qDebug()<<"Log sub"<<min<<".."<<max<<"/"<<numDiv<<"delta="<<delta<<"10^"<<ord<<"Calculating from"<<min+delta*startIndex<<"to"<<min+delta*endIndex<<xMin<<xMax;

  for(int i=startIndex;i<endIndex;i++)
  {
    double start=(max-min)*i/numDiv+min;
    double end=(max-min)*(i+1)/numDiv+min;
    if((start<=xMin && end>=xMin) || (start<=xMax && end>=xMax) || (start>xMin && end<=xMax))
    {
      if((prevDiv==1) && ratioPerTick<end/(end-delta*0.1))
        recalculateLogarithmicSubDiv(insert, k, start, end, 10, ord-1, 1);
      else if((prevDiv==1) && ratioPerTick<end/(end-delta*0.2))
        recalculateLogarithmicSubDiv(insert, k, start, end, 5, ord-1, 5);
      else if((prevDiv==1) && ratioPerTick<end/(end-delta*0.25))
        recalculateLogarithmicSubDiv(insert, k, start, end, 4, ord-2, 4);
      else if((prevDiv==1 || prevDiv==2 || prevDiv==5) && ratioPerTick<end/(end-delta*0.5))
        recalculateLogarithmicSubDiv(insert, k, start, end, 2, prevDiv==5?ord:ord-1, prevDiv*2);
      else
      {
        switch(prevDiv)
        {
          case 1: case 10:
            addLogSubTicks(insert, ratioPerSubTick, start, end, {2,4,5,10}); break;
          case 2:
            addLogSubTicks(insert, ratioPerSubTick, start, end, {2,5}); break;
          case 4:
            addLogSubTicks(insert, ratioPerSubTick, start, end, {2,5}); break;
          case 5:
            addLogSubTicks(insert, ratioPerSubTick, start, end, {2, 4}); break;
        }
//        qDebug()<<"End"<<start<<end<<prevDiv;
        insert.addMain(start, ord);
      }
    }
  }
}

void TickAlgorithmBasic::initializeLogTables()
{
  if(m_logBackupDiv.isEmpty() && m_logOptimizedDiv.isEmpty())
  {
    addLogTableRow(m_logOptimizedDiv, {100}, {{2,5}});
    addLogTableRow(m_logOptimizedDiv, {5,25,100}, {{2,5},{2,5},{2,3}});
    addLogTableRow(m_logOptimizedDiv, {10}, {{2, 5, 9}});
    addLogTableRow(m_logOptimizedDiv, {3, 10}, {{3}, {2,7}});
    addLogTableRow(m_logOptimizedDiv, {2, 3, 5.5, 10}, {{2,4}, {2,4}, {5}, {5}});
    addLogTableRow(m_logOptimizedDiv, {2, 4, 10}, {{2}, {2}, {2,3,6}});
    addLogTableRow(m_logOptimizedDiv, {1.5, 2.5, 4, 6, 10}, {{2}, {2,4}, {3,5}, {2,4}, {2,4}});
    m_logBackupDiv=m_logOptimizedDiv;
    for(int nDiv=5;;nDiv++)
    {
      QList<double> l;
      QList<QList<int>> lDiv;
      double minRatio=qInf(), last=1.;
      for(int i=0;i<nDiv;i++)
      {
        double c=pow(10,(i+1.)/(nDiv+1.));
        double approx=nDiv>=10?0.1:(nDiv>=6?0.2:0.5);
        c=round(c/approx)*approx;
        if(c/last<minRatio)
          minRatio=c/last;
        last=c;
        l.append(c);
        lDiv.append({2,4});
      }
      if(10./last<minRatio)
        minRatio=10./last;
      l.append(10);
      lDiv.append({2,4});
      if(minRatio<10./9.)
        break;
//      qDebug()<<nDiv;
      addLogTableRow(m_logBackupDiv, l, lDiv);
    }

//    qDebug()<<"Done";
//    for(int i=0;i<m_logOptimizedDiv.size();i++)
//      qDebug()<<i /*m_logOptimizedDiv[i].ratioMin<<m_logOptimizedDiv[i].ratioMax*/<<m_logOptimizedDiv[i].setPoints<<m_logOptimizedDiv[i].precision;
//    qDebug()<<"Done backup";
//    for(int i=0;i<m_logBackupDiv.size();i++)
//      qDebug()<<i /*m_logBackupDiv[i].ratioMin<<m_logBackupDiv[i].ratioMax*/<<m_logBackupDiv[i].setPoints<<m_logBackupDiv[i].precision;
  }
}
const TickAlgorithmBasic::LogDivision *TickAlgorithmBasic::findLogTableIndex(const QList<TickAlgorithmBasic::LogDivision> &source, double ratioMin, double ratioMax)
{
  int ret;
  for(ret=-1;ret<source.size()-1 && source[ret+1].ratioMin>ratioMin;ret++)
  {

  }
  if(ret>=0 && source[ret].ratioMax>ratioMax)
    ret=-1;
  return ret>=0?&source[ret]:nullptr;
}
void TickAlgorithmBasic::addLogTableRow(QList<TickAlgorithmBasic::LogDivision> &dest, const QList<double> &setPoints, const QList<QList<int> > &allowedDivisions)
{
  LogDivision cur;
  Q_ASSERT(allowedDivisions.size()==setPoints.size());
  Q_ASSERT(!setPoints.isEmpty());
  int lastOrder=round(log10(setPoints.last()));
  // First copy the data
  cur.setPoints=setPoints;
  cur.allowedDivisions=allowedDivisions;
  cur.ratioMax=0.;
  cur.ratioMin=qInf();
  // Computes the mimum ratio and the maximum ratio (maximum product between two consecutive ratios)
  double firstRatio=qQNaN(), lastRatio=0.;
  for(int i=0;i<setPoints.size();i++)
  {
    double curRatio=setPoints[i]/(i>0?setPoints[i-1]:1);
    if(qIsNaN(firstRatio))
      firstRatio=curRatio;
    if(curRatio<cur.ratioMin)
      cur.ratioMin=curRatio;
    if(lastRatio*curRatio>cur.ratioMax)
      cur.ratioMax=lastRatio*curRatio;
    if(i==setPoints.size()-1 && firstRatio*curRatio>cur.ratioMax)
      cur.ratioMax=firstRatio*curRatio;
    lastRatio=curRatio;
  }
  // Calculates the precision needed for each setpoint
  int worstPrecision=1000;
  for(int i=setPoints.size()-2;i>=0;i--)
  {
    double temp=setPoints[i];
    double prec=0;
    if(fmod(temp,1.)<0.001 || fmod(temp,1.)>0.999)
      for(int j=0;j<15 && (fmod(temp,10.)<0.001 || fmod(temp,10.)>9.999); j++,prec++,temp/=10)
      {

      }
    else
      for(int j=0;j<15 && (fmod(temp,1.)>0.001 && fmod(temp,1.)<9.999); j++,prec--,temp*=10)
      {

      }
    if(prec<worstPrecision)
      worstPrecision=prec;
    cur.precision.insert(0, worstPrecision);
  }
  cur.setPoints.insert(0, 1.);
  int firstPrecision=cur.precision.isEmpty()?0:cur.precision.first();
  int lastPrecision=cur.precision.isEmpty()?0:cur.precision.last();
  cur.precision.insert(0, qMax(firstPrecision, lastPrecision-lastOrder));
  cur.precision.insert(cur.precision.size(), cur.precision.first()+lastOrder);
  // Now find the correct insertion position in list (optimized for decrease by ratioMin)
  int index;
  if(!dest.isEmpty() && cur.ratioMin>=dest.first().ratioMin)
    index=0;
  else
    for(index=dest.size();index>0;index--)
      if(dest[index-1].ratioMin>cur.ratioMin)
        break;
  // Insert it and optimizes the list (rows with lower ratioMin but higher ratioMax than another are discarded)
  for(;index<dest.size();)
  {
    if(dest[index].ratioMax<cur.ratioMax)
      break;
//    qDebug()<<"Delete"<<dest[index].setPoints<<"worst than"<<setPoints;
    dest.removeAt(index);
  }
  if(index==0 || dest[index-1].ratioMax>cur.ratioMax)
    dest.insert(index, cur);
//  else
//    qDebug()<<"Discarded"<<setPoints<<"worst than"<<dest[index-1].setPoints;
}
void TickAlgorithmBasic::addLogSubTicks(TickListInserter &insert, double minRatioPerSubTick, double start, double end, const QList<int> &allowedSubs)
{
  int numSubTicks=1;
  double delta=qAbs(end-start);
  end=qMax(start,end);
  for(int j=0;j<allowedSubs.size();j++)
  {
    int ndiv=allowedSubs[j];
    if(end/(end-delta/ndiv)>minRatioPerSubTick)
      numSubTicks=ndiv;
    else
      break;
  }
  if(numSubTicks>1)
    for(int j=1;j<numSubTicks;j++)
      insert.addSub(end-delta*j/numSubTicks);
}

void TickAlgorithmBasic::addLogTicksByTable(TickListInserter &insert, const LogDivision *row, double k, int order)
{
  // Note first item in row->setPoints is always 1.0
  double b=pow(10,order);
  double last=b;
  double deltaPerSubTick=m_minSubDistance>=0?exp(m_minSubDistance/k):qQNaN();
  for(int i=1;i<row->setPoints.size();i++)
  {
    double cur=row->setPoints[i]*b;
    insert.addMain(last, order+row->precision[i-1]);
    addLogSubTicks(insert, deltaPerSubTick, last, cur, row->allowedDivisions[i-1]);
    last=cur;
  }
}
bool TickAlgorithmBasic::recalculateLogarithmic(TickListInserter &insert, double k)
{
  initializeLogTables();
  bool ret=false;
  double deltaPerTick=exp(m_minMainDistance/k/0.9999999);
  double totalDelta=insert.preimage().max()/insert.preimage().min();
  int ordmin=floor(log10(insert.preimage().min()));
  int ordmax=ceil(log10(insert.preimage().max()));
  if(deltaPerTick<10/9.)
  {
//    qDebug()<<"Small index (divide et impera)";
    for(int o=ordmin;o<ordmax;o++)
      recalculateLogarithmicSubDiv(insert, k, pow(10,o), pow(10,o)*10, 9,o);
  }
  else
  {
    const LogDivision *divisionTable=nullptr;
    divisionTable=findLogTableIndex(m_logOptimizedDiv, deltaPerTick, totalDelta);
    if(!divisionTable)
      divisionTable=findLogTableIndex(m_logBackupDiv, deltaPerTick, qInf());
    if(divisionTable)
    {
      int r=round(log10(divisionTable->setPoints.last()));
      ordmin=floor(ordmin*1./r)*r;
      ordmax=ceil(ordmax*1./r)*r;
      for(int o=ordmin;o<ordmax;o+=r)
        addLogTicksByTable(insert, divisionTable, k, o);
      insert.addMain(pow(10,ordmax), divisionTable->precision.first()+ordmax);
    }
  }
  ret=insert.ticks().size()>=2;
  if(!ret)
  {
    if(insert.ticks().size()==1) // If we inserted at least one value tries to use it
    {
      double v=insert.ticks().x(0);
      for(int j=0;j<15;j++)
      {
        double b0=pow(10,ordmin-j), b1=pow(10,ordmax-j-1);
        double v1=ceil(insert.preimage().min()/b0)*b0;
        double v2=floor(insert.preimage().max()/b1)*b1;
        if(v>v1 && v/v1>deltaPerTick)
        {
          insert.addMain(v1,ordmin-j);
          ret=true;
          break;
        }
        if(v2>v && v/v2>deltaPerTick)
        {
          insert.addMain(v2,ordmax-j-1);
          ret=true;
          break;
        }
      }
      qDebug()<<v;
    }
    if(!ret)
    {
//      qDebug()<<"Log fail"<<ordmin<<ordmax;
      for(int j=0;j<15;j++)
      {
        double b0=pow(10,ordmin-j), b1=pow(10,ordmax-j-1);
        double v1=ceil(insert.preimage().min()/b0)*b0;
        double v2=floor(insert.preimage().max()/b1)*b1;
        if(v2>v1 && v2/v1>deltaPerTick)
        {
          insert.ticks().clear();
          insert.addMain(v1,ordmin-j);
          insert.addMain(v2,ordmax-j-1);
          ret=true;
          break;
        }
      }
    }
    if(!ret)
      ret=insert.ticks().size()>0;
  }
  return ret;
}

bool TickListInserter::addMain(double x, int precision)
{
  bool ret=false;
  auto y=m_function->f(x);
  if(m_preimage.contains(x) && m_image.contains(y))
  {
    m_list.addMain(y, x, precision);
    ret=true;
  }
  return ret;
}

bool TickListInserter::addMain(double x)
{
  bool ret=false;
  auto y=m_function->f(x);
  if(m_image.contains(y))
  {
    m_list.addMain(y, x);
    ret=true;
  }
  return ret;
}

bool TickListInserter::addSub(double x)
{
  bool ret=false;
  auto y=m_function->f(x);
  if(m_preimage.contains(x) && m_image.contains(y))
  {
    m_list.addSub(y, x);
    ret=true;
  }
  return ret;
}

HEF::Range TickListInserter::image() const
{
  return m_image;
}

HEF::Range TickListInserter::preimage() const
{
  return m_preimage;
}

HEF::FunctionPtr TickListInserter::function() const
{
  return m_function;
}

TickList &TickListInserter::ticks()
{
  return m_list;
}
