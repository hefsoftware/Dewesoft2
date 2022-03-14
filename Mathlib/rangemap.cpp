#ifndef RANGEMAP_INC
#define RANGEMAP_INC
#include "rangemap.h"

#include "range.h"
#include <QList>
#include <QString>
#include <limits>
namespace HEF
{
//  QList

template <typename V, typename B> RangeMapBase<V, B>::RangeMapBase(V key,double value)
{
  m_data.append(B(key,value,value));
}

template <typename V, typename B> RangeMapBase<V, B>::RangeMapBase(const B &range): m_data()
{
  m_data.append(range);
}

template <typename V, typename B> RangeMapBase<V, B>::RangeMapBase(V key,double min, double max): m_data()
{
  m_data.append(BasicRangeMap<V>(key,min,max));
}

template <typename V, typename B> bool RangeMapBase<V, B>::valueInRange(double value) const
{
  int left=0,right=m_data.size()-1;
  while (left <= right) {
    int middle = (left + right) / 2;

    if (value < m_data[middle].min())
      right = middle - 1;
    else if (value > m_data[middle].max())
      left = middle + 1;
    else
      return 1;
  }
  return 0;
}
template <typename V, typename B> V RangeMapBase<V, B>::valueAt(double value) const
{
  int left=0,right=m_data.size()-1;
  while (left <= right) {
    int middle = (left + right) / 2;

    if (value < m_data[middle].min())
      right = middle - 1;
    else if (value > m_data[middle].max())
      left = middle + 1;
    else
      return m_data[middle].value();
  }
  return V();

}

template <typename V, typename B> double RangeMapBase<V, B>::min()
{
  return m_data.empty()?NAN:m_data.first().min();
}

template <typename V, typename B> double RangeMapBase<V, B>::max()
{
  return m_data.empty()?NAN:m_data.last().max();
}

template <typename V, typename B> RangeMapBase<V,B> RangeMapBase<V, B>::join(const RangeMapBase<V, B> &r1, RangeFlag flags)
{
  const RangeMap<V> &r0=*this;
  RangeMap<V> ret;
  BasicRangeMap<V> r0A, r1A,lastRange,curRange;
  int i0=0,i1=0;
  int n0,n1;
  n0=r0.m_data.size();
  n1=r1.m_data.size();
  double curVal=NAN,endVal,nextVal=NAN;
  bool in0=false,in1=false;
  r0A=n0? r0.m_data[0] : BasicRangeMap<V>();
  r1A=n1? r1.m_data[0] : BasicRangeMap<V>();
  while(i0<n0 || i1<n1)
  {
    Q_ASSERT((i0>=n0 && !r0A.isValid()) || (i0<n0 && r0A==r0.m_data[i0]));
    Q_ASSERT((i1>=n1 && !r1A.isValid()) || (i1<n1 && r1A==r1.m_data[i1]));
    if(!in0 && !in1)
    {
      if(lastRange.isValid())
      {
        ret.m_data.append(lastRange);
        lastRange=BasicRangeMap<V>();
      }
      if(r0A.isValid() || r1A.isValid())
      {
        if((r0A.isValid() && !r1A.isValid()) || (r0A.isValid() && r1A.isValid() && r0A.min()<r1A.min()))
        {
          curVal=r0A.min();
          in0=true;
        }
        else if((r1A.isValid() && !r0A.isValid()) || (r0A.isValid() && r1A.isValid() && r1A.min()<r0A.min()))
        {
          curVal=r1A.min();
          in1=true;
        }
        else if(r0A.isValid() && r0A.isValid() && r0A.min()==r1A.min())
        {
          curVal=r0A.min();
          in0=in1=true;
        }
      }
      else
        break;
    }
    endVal=nextVal=INFINITY;
    if(in0)
    {
      endVal=qMin(r0A.max(),endVal);
    }
    else if(!in0 && r0A.isValid() )
    {
      endVal=qMin(nextafter(r0A.min(),-INFINITY),endVal);
    };
    if(in1 && curVal<=r1A.max())
    {
      endVal=qMin(r1A.max(),endVal);
    }
    else if(!in1 && r1A.isValid() )
    {
      endVal=qMin(nextafter(r1A.min(),-INFINITY),endVal);
    };
    if((in0 && !in1) || (in0 && in1 && (flags&Range::PriorityA)))
      curRange=BasicRangeMap<V>(r0A.value(),curVal,endVal);
    else //if((in1 && !in0) || (in0 && in1 && !(m_flags&PriorityA)))
      curRange=BasicRangeMap<V>(r1A.value(),curVal,endVal);
    RangeFlag flags=Range::KeepNone;
    if(in0 && endVal==r0A.max())
    {
      i0++;
      flags|=Range::KeepAEnd;
      if(i0<n0)
      {
        r0A=r0.m_data[i0];
        if(r0A.min()==nextafter(endVal,INFINITY))
        {
          flags|=Range::KeepAStart;
          in0=true;
        }
        else
          in0=false;
      }
      else
      {
        r0A=BasicRangeMap<V>();
        in0=false;
      }
    }
    else if(!in0 && r0A.isValid() && endVal==nextafter(r0A.min(),-INFINITY))
    {
      flags|=Range::KeepAStart;
      in0=true;
    };
    if(in1 && endVal==r1A.max())
    {
      i1++;
      flags|=Range::KeepBEnd;
      if(i1<n1)
      {
        r1A=r1.m_data[i1];
        if(r1A.min()==nextafter(endVal,INFINITY))
        {
          flags|=Range::KeepBStart;
          in1=true;
        }
        else
          in1=false;
      }
      else
      {
        r1A=BasicRangeMap<V>();
        in1=false;
      }
    }
    else if(!in1 && r1A.isValid() && endVal==nextafter(r1A.min(),-INFINITY))
    {
      flags|=Range::KeepBStart;
      in1=true;
    };
    curVal=nextafter(endVal,INFINITY);
    if(lastRange.isValid())
    {
      if(lastRange.value()!=curRange.value())
      {
        ret.m_data.append(lastRange);
        lastRange=curRange;
      }
      else
      {
        lastRange=BasicRangeMap<V>(lastRange.value(),lastRange.min(),curRange.max());
      }
    }
    else
      lastRange=curRange;
    if((flags & (flags&Range::KeepAll)))
    {
      ret.m_data.append(lastRange);
      lastRange=BasicRangeMap<V>();
    }
  }
  if(lastRange.isValid())
    ret.m_data.append(lastRange);
  return ret;
}

template <typename V, typename B> RangeMapBase<V, B> RangeMapBase<V,B>::operator &(const RangeMapBase<V, B> &range1) const
{
  RangeMap<V> ret;
  const RangeMap<V> &range0=*this;
  int i0=0,i1=0,n0,n1;
  double min0,min1;
  double max0,max1;
  n0=range0.m_data.count();
  n1=range1.m_data.count();
  V key0,key1;
  while(i0<n0 && i1<n1)
  {
    min0=range0.m_data[i0].min();
    min1=range1.m_data[i1].min();
    max0=range0.m_data[i0].max();
    max1=range1.m_data[i1].max();
    key0=range0.m_data[i0].value();
    key1=range1.m_data[i1].value();
    if(max1 < min0 || max0<min1)
    {
      // Basic ranges does not overlap anywhere, increase the smallest one
      if(min0<min1)
        i0++;
      else
        i1++;
    }
    else
    {
      // We have some overlap. First finds it and adds it to ret
      if(key0==key1)
        ret.m_data.append(BasicRangeMap<V>(key0,qMax(min0,min1),qMin(max0,max1)));
      // Then moves forward the range finishing before
      if(max0==max1)
      {
        i0++;
        i1++;
      }
      else if(max0<max1)
        i0++;
      else
        i1++;
    }
  }
  return ret;
}
template <typename V, typename B> Range RangeMapBase<V, B>::unmapped_range(void) const
{
  Range ret;
  foreach(const B &r, m_data)
  {
    ret=ret|BasicRange(r.min(),r.max());
  }
  return ret;
}
//template <typename V> template <typename R,class F> RangeMap<R> RangeMap<V>::remap(F &mapFunc) const
//{
//  RangeMap<R> ret(flags());
//  for(int i=0,n=m_data.count();i<n;i++)
//  {
//    ret=ret|BasicRangeMap<R>(mapFunc(m_data[i].value()),m_data[i].min(),m_data[i].max());
//    //return func(4);
//  }
//  return ret;
//}

}
#endif // RANGEMAP_INC
