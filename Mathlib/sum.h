#pragma once
#include <Qt>
#include <QtMath>
namespace HEF
{
  constexpr double square(double v)
  {
    return v*v;
  }
  constexpr double avgSawtooth(double y0, double y1)
  {
    return ((y0+y1)*.5);
  }
  constexpr double integralSawtooth(double x0, double y0, double x1, double y1)
  {
    return (x1-x0)*avgSawtooth(y0,y1);
  }
  // RMS of sawtooth/triangularWave
  //
  constexpr inline double rmsSawtooth(double y0, double y1)
  {
    return (square((y0+y1)*.5)+square((y0-y1)*.5)/3.);
  }

  // Note: To calculate RMS: sqrt(integralSquared(x0,y0,x1,y1)/(x1-x0))
  //
  constexpr double integralSquaredSawtooth(double x0, double y0, double x1, double y1)
  {
    return (x1-x0)*rmsSawtooth(y0,y1);
  }

  constexpr double integralHermite(double x, double y0, double c1, double c2, double c3)
  {
    double sqX=square(x);
    return x*(y0+x*c1/2.+sqX*c2/3+sqX*x*c3/4);
  }

  constexpr double integralSquaredHermite(double x, double y0, double c1, double c2, double c3)
  {
    double sqX=square(x);
    return x*(210*y0*y0+x*210*y0*c1+sqX*70*(2*y0*c2+c1*c1)+sqX*x*105*(y0*c3+c2*c3)+sqX*sqX*42*(2*c1*c3+c2*c2)+sqX*sqX*x*70*c2*c3+sqX*sqX*sqX*30*c3*c3)/210;
  }

  class Sum
  {
  public:
    Sum(double start=0.) {m_sum=start;}
    virtual ~Sum() { }
    virtual double sum() const { return m_sum; }
    virtual void add(double value) = 0;
    virtual void clear() = 0;
    inline void add(std::initializer_list<double> args)
    {
      add(args.begin(), args.end());
    }
    template <typename InputIterator> void add(InputIterator first, InputIterator last)
    {
      std::for_each(first, last, [this](double v) { add(v); } );
    }
    inline Sum &operator +=(double val) { add(val); return *this; };
    inline operator double() { return sum(); }
  protected:
    double m_sum;
  };
  class KahanStandardSum final: public Sum
  {
  public:
    KahanStandardSum(double start=0.): Sum(start), m_acc(0) { }
    inline double sum() const final { return m_sum; }
    inline void clear() final { m_acc=m_sum=0; }
    using Sum::add;
    inline void add(double value) override final
    {
      double y=value-m_acc;
      volatile double t=m_sum+y;
      volatile double z=t-m_sum;
      m_acc=z-y;
      m_sum=t;
    };
  protected:
    double m_acc;
  };

  class KahanBabushkaNeumaierSum final: public Sum
  {
  public:
    KahanBabushkaNeumaierSum(double start=0.): Sum(start), m_acc(0) { }
    inline double sum() const override final { return m_sum+m_acc; }
    using Sum::add;
    inline void add(double value) override final
    {
      volatile double t=m_sum+value;
      if(qAbs(m_sum)>=qAbs(value))
      {
        volatile double y=m_sum-t;
        m_acc += y + value;
      }
      else
      {
        volatile double y=value-t;
        m_acc += y+m_sum;
      }
      m_sum=t;
    };
    inline void clear() final { m_acc=m_sum=0; }
  protected:
    double m_acc;
  };
  using KahanSum=KahanBabushkaNeumaierSum;

  inline KahanSum kahanSum(std::initializer_list<double> args)
  {
    KahanSum sum;
    sum.add(args);
    return sum.sum();
  }

  template <typename InputIterator> inline KahanSum kahanSum(InputIterator first, InputIterator last)
  {
    KahanSum sum;
    sum.add(first, last);
    return sum.sum();
  }

  class StatAccumulator
  {
  public:
    StatAccumulator();
    void clear();
    double min() const { return m_min; }
    double max() const { return m_max; }
    double sum() const { return m_sum.sum(); }
    double squaredSum() const { return m_squaredSum.sum(); }
    void feed(double x, double y);
  protected:
    double m_lastX, m_lastY;
    double m_min, m_max;
    KahanBabushkaNeumaierSum m_deltaX, m_sum, m_squaredSum;
  };
}
