#pragma once

#include <Qt>
#include <QList>
#include <cmath>
namespace HEF
{
  template <typename V> class RangeMap;
  class BasicRange
  {
  public:
    explicit BasicRange(): m_min(qQNaN()), m_max(qQNaN()) { }
    explicit constexpr BasicRange(double val): m_min(val), m_max(val) { }
    explicit constexpr BasicRange(double min, double max): m_min((max>=min)?min:((max<min)?max:qQNaN())), m_max((max>=min)?max:((max<min)?min:qQNaN())) { }
    constexpr double min() const {return m_min;}
    constexpr double max() const {return m_max;}
    inline bool isValid() const { return !qIsNaN(m_min) && !qIsNaN(m_max) && m_min<=m_max;}
    inline bool contains(double val) const {return isValid() && m_min<=val && m_max>=val;}
    constexpr bool operator !=(const BasicRange &val2) const {return val2.m_min!=m_min || val2.m_max!=m_max;}
    constexpr bool operator ==(const BasicRange &val2) const {return val2.m_min==m_min && val2.m_max==m_max;}
    inline BasicRange shrinkEpsilon() const{if(!isValid() || m_min==m_max || std::nextafter(m_min,qInf())==m_max)return BasicRange(); else return BasicRange(nextafter(m_min,qInf()),nextafter(m_max,-qInf()));}
    inline HEF::BasicRange growToInclude(double value) const{if(qIsNaN(value))return *this; else if(!isValid())return BasicRange(value,value);else return BasicRange(qMin(m_min,value),qMax(m_max,value));}
  protected:
    double m_min;
    double m_max;
  };

  class Range
  {
    template <typename V> friend class RangeMap;
  public:
    enum FlagsEnum
    {
      KeepNone=0x00,
      KeepAStart=0x01,
      KeepAEnd=0x02,
      KeepA=KeepAStart|KeepAEnd,
      KeepBStart=0x04,
      KeepBEnd=0x08,
      KeepB=KeepBStart|KeepBEnd,
      KeepAll=KeepA|KeepB,
      PriorityA=0x10,
      PriorityB=0x20, // Default
      PriorityMask=PriorityA|PriorityB
    };
    Range();
    explicit Range(BasicRange range);
    // If sorted is true range will be created only if max>min. If sorted is false and max<min then the range will be created with ranges inverted
    explicit Range(double min, double max, bool sorted=false);
    explicit Range(double value);
    ~Range(){}
    bool contains(double val) const;
    double firstGE(double base) const;
    double firstLE(double value) const;
    double min() const;
    double max() const;
    double maxAbsolute() const {return isEmpty()?qQNaN():qMax(qAbs(max()), qAbs(min()));}
    double minAbsolute() const {return isEmpty()?qQNaN():qMin(qAbs(max()), qAbs(min()));}
    double range() const { return m_data.isEmpty()?qQNaN():(max()-min()); }
    Range complement() const;
    inline Range operator ~()const {return complement();}

    inline Range operator |(const Range &range2) const { return boolOperation([](bool a, bool b) {return a||b;}, range2); }
    inline Range operator |(const BasicRange &range2) const{return (*this)|Range(range2);}
    inline Range operator &(const Range &range2) const { return boolOperation([](bool a, bool b) {return a&&b;}, range2); }
    inline Range operator &(const BasicRange &range2) const{return (*this)&Range(range2);}
    inline Range operator -(const Range &range2) const  { return boolOperation([](bool a, bool b) {return a && !b;}, range2); }
    inline Range operator -(const BasicRange &range2) const{return (*this)-Range(range2);}
    inline Range operator ^(const Range &range2) const  { return boolOperation([](bool a, bool b) {return (a&&!b)||(b&&!a);}, range2); }
    Range shrinkEpsilon() const;
    QString toString(int digits=6) const;
    inline bool isEmpty() const { return m_data.isEmpty(); }
    inline bool isValid() const { return !m_data.isEmpty(); }
    inline bool isFinite() const { return !m_data.isEmpty() && qIsFinite(m_data.first().min()) && qIsFinite(m_data.last().max()); }
    static inline Range full(){return Range(-qInf(),qInf());}
    static Range fromList(QList<BasicRange> ranges);
    static Range fromList(QList<Range> ranges);
    bool isEqual(const Range &r1,bool distinct=false) const;
    inline int numBasicRanges() const{return m_data.count();}
    const BasicRange &basicRange(int i) const {return m_data[i];}
    Range remapBasic(const std::function<Range(const BasicRange &range)> &mapFunc) const;
    Range boolOperation(const std::function<bool (bool, bool)> &f, const Range &r1) const;
    void clear();
  protected:
    QVector<BasicRange> m_data;
  };
  Q_DECLARE_FLAGS ( RangeFlag, HEF::Range::FlagsEnum );
  Q_DECLARE_OPERATORS_FOR_FLAGS(RangeFlag);
//  typedef QList<BasicRange> BasicRangeList;
//  typedef QList<Range> RangeList;
}
// Optimized inline join of a list of range and a single range
QList<HEF::Range> &operator &=(QList<HEF::Range> &op1, const HEF::Range &op2);
