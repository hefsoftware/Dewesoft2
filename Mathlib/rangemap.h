#ifndef RANGEMAP_H
#define RANGEMAP_H

#include <QList>
#include <cmath>
#include "range.h"
#include <functional>

namespace HEF
{
template <typename V> class BasicRangeMap: public BasicRange
{
  public:
    constexpr explicit BasicRangeMap<V>(V value,double min=qQNaN(), double max=qQNaN()): BasicRange(min, max), m_value(value) { }
    V value() const {return m_value;}
    bool operator !=(const BasicRangeMap &val2) const {return val2.m_value!=m_value || val2.m_min!=m_min || val2.m_max!=m_max;}
    bool operator ==(const BasicRangeMap &val2) const {return val2.m_value==m_value && val2.m_min==m_min && val2.m_max==m_max;}
  protected:
      V m_value;
};

template <typename V, typename B> class RangeMapBase
{
public:
  RangeMapBase<V,B>() { }
  explicit RangeMapBase<V,B>(const B &range);
  RangeMapBase<V,B>(V key,double min, double max);
  RangeMapBase<V,B>(V key,double value);
  V valueAt(double val) const;

  bool valueInRange(double val) const;
  double min();
  double max();

  // inverse not available... to which value would we map?
  //RangeMap<V> inverse();
  //inline StdRange operator ~(){return inverse();}

  RangeMapBase<V, B> join(const RangeMapBase<V, B> &range2, RangeFlag flags);
  RangeMapBase<V, B> join(const B &range2, RangeFlag flags){return join(RangeMapBase(range2), flags);}

  RangeMapBase<V, B> operator &(const RangeMapBase<V, B> &range2) const;
  RangeMapBase<V, B> operator &(const Range &range2) const;

  RangeMapBase<V, B> withFlags(RangeFlag flags) const{return RangeMapBase<V, B>(flags)|*this;}
//  RangeFlag flags() const{return m_flags;}
  Range unmapped_range(void) const;

  bool isEqual(const RangeMapBase<V, B> &r1,bool distinct=false) const;

  int numBasicRanges() const{return m_data.count();}
  const B &basicRange(int i) const {return m_data[i];}
  //http://www.gamedev.net/topic/432425-passing-function-as-template-argument-c/

  //std::function<bool (uint recno, T val)>
  //template < typename R, class F > RangeMap<R> remap(F func) const;
  template <typename R,class F> RangeMap<R> remap(F &f) const;
  template < typename R, class F > R remapEasy(const F &func, V k) const;
  template <typename R,class F> R remy(F &f, V k) const;

  QString toString(int digits=6)
  {
    QString ret="<";
    bool first=true;
    foreach(const B &r, m_data)
    {
      if(!first)ret+=",";
      first=false;
      if(r.min()==r.max())
        ret+=QString("%1 \'%2\'").arg(r.min(),0,'g',digits).arg(toQString(r.value()));
      else
        ret+=QString("%1:%2 \'%3\'").arg(r.min(),0,'g',digits).arg(r.max(),0,'g',digits).arg(toQString(r.value()));
    }
    return ret+">";
  }

protected:
  QList<B> m_data;
//  RangeFlag m_flags;
};

}

#endif // RANGEMAP_H
