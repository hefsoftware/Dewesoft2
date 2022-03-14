#include "range.h"
#include <QList>
#include <QString>

//#include <limits>

using namespace HEF;



Range::Range(double value)
{
  if(!qIsNaN(value))
    m_data.append(BasicRange(value,value));
}

Range::Range(): m_data()
{
}

Range::Range(BasicRange range): m_data()
{
  if(range.isValid())
    m_data.append(range);
}
Range::Range(double min, double max, bool sorted): m_data()
{
  if(!sorted || max>=min)
  {
    BasicRange br(min,max);
    if(br.isValid())
      m_data.append(br);
  }
}
Range Range::fromList(QList<BasicRange> range)
{
  Range temp;
  Q_FOREACH(BasicRange r,range)
  {
    temp=temp|r;
  }
  return temp;
}
Range Range::fromList(QList<Range> range)
{
  Range temp;
  Q_FOREACH(Range r,range)
  {
    temp=temp|r;
  }
  return temp;
}
bool Range::contains(double value) const
{
  if(!qIsNaN(value))
  {
    int left=0,right=m_data.size()-1;
    while (left <= right) {
      int middle = (left + right) / 2;

      if (value < m_data[middle].min())
        right = middle - 1;
      else if (value > m_data[middle].max())
        left = middle + 1;
      else
        return true;
    }
  }
  return false;
}

double Range::firstGE(double value) const
{
  int left=0,right=m_data.size();
  while (left != right) {
    int middle = (left + right) / 2;

    if (value < m_data[middle].min())
      right = middle;
    else if (value > m_data[middle].max())
      left = middle + 1;
    else
      return value;
  }
  if(left<m_data.size())
    return m_data[left].min();
  else
    return qQNaN();
}
#include <QDebug>
double Range::firstLE(double value) const
{
  // To do
  int left=0,right=m_data.size();
  while (left != right) {
    int middle = (left + right) / 2;

    if (value < m_data[middle].min())
      right = middle;
    else if (value > m_data[middle].max())
      left = middle + 1;
    else
      return value;
  }
  if(left==0 || m_data.isEmpty())
    return qQNaN();
 else
    return m_data[left-1].max();
}

double Range::min() const
{
  return m_data.empty()?qQNaN():m_data.first().min();
}

double Range::max() const
{
  return m_data.empty()?qQNaN():m_data.last().max();
}


// Complements the range
Range Range::complement() const
{
  Range ret;
  double lastmax=-qInf();
  foreach(BasicRange r,m_data)
  {
    if(r.min()!=-qInf())
      ret.m_data.append(BasicRange(lastmax,nextafter(r.min(),-qInf())));
    lastmax=nextafter(r.max(),qInf());
  }
  if(lastmax!=qInf())
    ret.m_data.append(BasicRange(lastmax,qInf()));
  return ret;
}


Range Range::shrinkEpsilon() const
{
  Range ret;
  for(int i=0;i<m_data.count();i++)
  {
    BasicRange r=m_data[i].shrinkEpsilon();
    if(r.isValid())
    {
      ret.m_data.append(r);
    }
  }
  return ret;
}
bool Range::isEqual(const Range &r1,bool distinct) const
{
  bool ret=true;
  const Range &r0=*this;
  if(distinct)
  {
    int i,n;
    if(r0.m_data.count()!=r1.m_data.count())
      ret=false;
    else
    {
      for(i=0,n=m_data.count();i<n;i++)
      {
        if(r0.m_data[i]!=r1.m_data[i])
        {
          ret=false;
          break;
        }
      }
    }
  }
  else
  {
    int i0,n0,i1,n1;
    bool newrange=true;
    BasicRange v0,v1;
    for(i0=i1=0,n0=r0.m_data.count(),n1=r1.m_data.count();i0<n0 || i1<n1;)
    {
      if(newrange)
      {
        if(! (i0<n0 && i1<n1))
        {
          ret=false;
          break;
        }
        v0=r0.m_data[i0];
        v1=r1.m_data[i1];
        i0++; i1++;
        if(v0.min()!=v1.min())
        {
          ret=false;
          break;
        };
        newrange=false;
      };
      if(v0.max()==v1.max())
        newrange=true;
      else if(v0.max()<v1.max())
      {
        if(i0>=n0)
        {
          ret=false;
          break;
        }
        v0=BasicRange(v0.min(),r0.m_data[i0].max());
        i0++;
      }
      else if(v1.max()<v0.max())
      {
        if(i1>=n1)
        {
          ret=false;
          break;
        }
        v1=BasicRange(v1.min(),r1.m_data[i1].max());
        i1++;
      }
    }
    // If one range reached end while the other didn't they're not the same
    // If ranges are not empty and ends of last basic range does not match arrays are not the same
    if(i0<n0 || i1<n1 || (n0 && n1 && v1.max()!=v0.max()))
      ret=false;
  }
  return ret;
}

QString Range::toString(int digits) const
{
  QString ret="<";
  bool first=true;
  foreach(BasicRange r, m_data)
  {
    if(!first)ret+=",";
    first=false;
    if(r.min()==r.max())
      ret+=QString::number(r.min(),'g',digits);
    else
      ret+=QString("%1:%2").arg(r.min(),0,'g',digits).arg(r.max(),0,'g',digits);
  }

  return ret+">";
}

Range Range::remapBasic(const std::function<Range(const BasicRange &range)> &mapFunc) const
{
  Range ret;
  for(int i=0,n=m_data.count();i<n;i++)
  {
    ret=ret|mapFunc((const BasicRange &)m_data[i]);
  }
  return ret;
}

Range HEF::Range::boolOperation(const std::function<bool (bool, bool)> &f, const Range &r1) const
{
  const Range &r0(*this);
  Range ret;
  double cur=-qInf(),next;
  int index_0,index_1;
  bool in_0,in_1,val,active=false;
  bool next_0,next_1,inc_0,inc_1;
  for(index_0=index_1=0;;)
  {
    in_0=index_0<r0.m_data.count() && cur>=r0.m_data[index_0].min();
    in_1=index_1<r1.m_data.count() && cur>=r1.m_data[index_1].min();
    next_0=index_0<r0.m_data.count();
    next_1=index_1<r1.m_data.count();
    next=qInf();
    inc_0=inc_1=false;
    if(in_0)
    {
      next=r0.m_data[index_0].max();
      inc_0=true;
    }
    else if(next_0)
      next=nextafter(r0.m_data[index_0].min(),-qInf());
    if(in_1)
    {
      double next1;
      next1=r1.m_data[index_1].max();
      if(next1<next)
      {
        next=next1;
        inc_0=false,
            inc_1=true;
      }
      else if(next1==next)
        inc_1=true;
    }
    else if(next_1)
    {
      double next1;
      next1=nextafter(r1.m_data[index_1].min(),-qInf());
      if(next1<next)
      {
        next=next1;
        inc_0=false;
      }
    }

    val=f(in_0,in_1);
    if(val)
    {
      if(active)
        ret.m_data.append(HEF::BasicRange(ret.m_data.takeLast().min(),next));
      else
        ret.m_data.append(HEF::BasicRange(cur,next));
    }
    cur=nextafter(next,qInf());
    active=val;
    if(inc_0)
      index_0++;
    if(inc_1)
      index_1++;
    if(next==qInf())
      break;
  }
  return ret;
}

void Range::clear()
{
  m_data.resize(0);
}

//QList<Range> &operator &=(QList<Range> &op1, const Range &op2);
QList<Range> &operator &=(QList<Range> &op1, const Range &op2)
{
  int op1size=op1.size();
  if(op2.numBasicRanges()==0) // & with empty range, clears all data
  {
    for(int i=0;i<op1size;i++)
    {
      op1[i].clear();
    }
  }
  else
  {
    int nb2=op2.numBasicRanges();
    for(int i=0;i<op1size;)
    {
      int nb=op1[i].numBasicRanges();
      if(nb==1)
      {
        if(op1[i].max()<op2.min())
        {
          // Current range is under op2
          do
          {
            op1[i].clear();
            i++;
          }while(i<op1size && op1[i].max()<op2.min());
        }
        else if(op1[i].min()>op2.max())
        {
          // Current range is over op2
          do
          {
            op1[i].clear();
            i++;
            if(i>=op1size || op1[i].min()<=op2.max())
              break;
          }while(i<op1size && op1[i].min()>op2.max());
        }
        else
        {
          bool ok=false;
          for(int j=0;j<nb2;j++)
          {
            if(op1[i].min()>=op2.basicRange(j).min() && op1[i].max()<=op2.basicRange(j).max())
            {
              // Fully contained inside a basic range
              ok=true;
              do{
                i++;
              }while(i<op1size && op1[i].min()>=op2.basicRange(j).min() && op1[i].max()<=op2.basicRange(j).max());
              break;
            }
            else if(j+1<nb2 && op1[i].min()>op2.basicRange(j).max() && op1[i].max()<op2.basicRange(j+1).min())
            {
              // Fully contained between valid basic ranges (and is empty)
              ok=true;
              do{
                op1[i].clear();
                i++;
              }while(i<op1size && op1[i].min()>op2.basicRange(j).max() && op1[i].max()<op2.basicRange(j+1).min());
              break;
            }
          }
          if(!ok)
          {
            op1[i]=op1[i]&op2;
            i++;
          }
        }
      }
      else if(nb==0)
      {
        i++;
      }
      else if(nb>1)
      {
        op1[i]=op1[i]&op2;
        i++;
      }
    }
  }
  return op1;
}
