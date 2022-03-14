#pragma once

#include "function.h"

namespace HEF
{
/** \brief Handles linear function.

    A linear function is any function in form y=x*k+y0
*/
class LinearFunction: public MonotonicFunction
{
protected:
  LinearFunction(double k, double y0, Range domain=Range(-INFINITY,INFINITY)){Q_UNUSED(domain); m_k=k;m_y0=y0;}
public:
  typedef QSharedPointer<LinearFunction> Ptr;
  virtual ~LinearFunction(){}
  using MonotonicFunction::f;
  double f(double val) const override {return val*m_k+m_y0;}
  double invert(double val) const override {return (val-m_y0)/m_k;}
  FunctionPtr inverse(const Range &range) const override;
//  virtual Function *clone() const{return new LinearFunction(m_k,m_y0);}
  static Ptr newFromTwoPoints(double x0,double y0, double x1, double y1);
  static Ptr newFromGainOffset(double k,double y0){return Ptr(new LinearFunction(k,y0));}
//  FunctionPtr optimizedComposition(const FunctionPtr &other) const override;
  double gain(void){return m_k;}
  double offset(void){return m_y0;}
  QString toString(const QString &x) const override;
protected:
  double m_k,m_y0;
  Range m_domain;
};

/**
  \brief Exponential function

  This class handles any function of kind: \f$ y = a e ^{ c x } +b \f$ with domain windowing.

*/
class ExponentialFunction: public MonotonicFunction
{
protected:
  ExponentialFunction(double a=1.0, double b=0.0, double c=1.0):m_a(a),m_b(b),m_c(c){}
public:
  typedef QSharedPointer<ExponentialFunction> Ptr;
  virtual ~ExponentialFunction(){}
  Range domain() const{return Range(-INFINITY,INFINITY);}
  Range codomain() const{return Range(0.0,INFINITY);}
  double f(double x) const
  {
    return m_a*exp(x*m_c)+m_b;
  }
  double invert(double y) const override
  {
    double x;
    x=log((y-m_b)/m_a)/m_c;
    return x;
  }
//  virtual Function *clone() const{return new ExponentialFunction(m_a,m_b,m_c);}
//  virtual FunctionPtr optimizedComposition(const FunctionPtr &other) const;
//  FunctionPtr optimizedInversedComposition(const FunctionPtr &other) const override;

  /**
    \brief Exponential function

    Return an exponential in the form: \f$ y = a p ^{ c x + d} +b \f$.

  */
  static Ptr newGeneric(double base, double a, double b, double c, double d)
  {
    double logBase=log(base);
    return Ptr(new ExponentialFunction(a*exp(logBase*d) ,b ,logBase*c));
  }
  static Ptr newGeneric(double base, double a=1.0, double b=0.0, double c=1.0)
  {
      return Ptr(new ExponentialFunction(a ,b ,log(base)*c));
  }
  static FunctionPtr newGenericInverse(double base, double a=1.0, double b=0.0, double c=1.0, double d=0.0);

  static Ptr newNatural(){return newGeneric(M_E);}
  static Ptr newDecimal(){return newGeneric(10.0);}
  double a() const{return m_a;}
  double b() const{return m_b;}
  double c() const{return m_c;}
  QString toString(const QString &x) const override { return QString("(%1*e^(%2*%4)+%3)").arg(m_a).arg(m_c).arg(m_b).arg(x); }
protected:
  double m_a, m_b, m_c;
};
/**
  \brief Logarithmic function

  This class handles any function of kind: \f$ y = a log_p( c x  + d) + b \f$.

*/
class LogarithmicFunction: public MonotonicFunction
{
protected:
  LogarithmicFunction(double a=1.0, double c=1.0, double d=0.0):m_a(a),m_c(c),m_d(d){}
public:
  virtual ~LogarithmicFunction(){}
  Range codomain() const override {return Range(-INFINITY,INFINITY);}
  Range domain() const override {return Range(0.0,INFINITY);}
  double f(double val) const override {return m_a*log(m_c*val+m_d);}
  double invert(double val) const override {return (exp(val/m_a)-m_d)/m_c;}
  FunctionPtr optimizedComposition(const FunctionPtr &other) const override;
  FunctionPtr optimizedInversedComposition(const FunctionPtr &other) const override;
//  virtual Function *clone() const{return new LogarithmicFunction(m_a,m_c,m_d);}
  //static MonotonicFunction *newGeneric(double base){return newGeneric();}
  static FunctionPtr newNatural(){return newBase(M_E);}
  static FunctionPtr newDecimal(){return newBase(10.0);}
//  static FunctionPtr newGenericBase(double base)
//  {
//    double w;
//    double a=1./log(base);
//    double w=exp(b/a);
//    c=w*c;
//    d=w*d;
//    return FunctionPtr(new LogarithmicFunction(a,c,d));
//  }
  static FunctionPtr newBase(double base)
  {
    return FunctionPtr(new LogarithmicFunction(1/log(base),1.,0.));
  }
  static FunctionPtr newGeneric(double base, double a, double b, double c, double d)
  {
    double w;
    a/=log(base);
    w=exp(b/a);
    c=w*c;
    d=w*d;
    return FunctionPtr(new LogarithmicFunction(a,c,d));
  }
  static FunctionPtr newGeneric(double a, double b, double c, double d)
  {
    double w;
    w=exp(b/a);
    c=w*c;
    d=w*d;
    return FunctionPtr(new LogarithmicFunction(a,c,d));
  }
//  static FunctionPtr newGeneric(double base, double a=1.0, double b=0.0, double c=1.0, double d=0.0)
//  {
//    double w;
//    a=a/log(base);
//    w=exp(b/a);
//    c=w*c;
//    d=w*d;
//    return FunctionPtr(new LogarithmicFunction(a,c,d));
//  }
  static FunctionPtr newFromTwoPoints(double x0, double y0, double x1, double y1);

  /**
    \brief Generic exponential function inverse

    Return the inverse of an exponential in the form: \f$ y = a p ^{ c x + d} +b \f$.

  */
  static FunctionPtr newGenericExponentialInverse(double base, double a, double b, double c, double d)
  {
    c=1/c;
    a=1/a;
    return newGeneric(base,c,-d*c,a,-b*a);
  }
  QString toString(const QString &x) const override;
  double a() const{return m_a;}
  double c() const{return m_c;}
  double d() const{return m_d;}
protected:
  double m_a,m_c,m_d;
};

class WindowFunctionOn: public Function
{
protected:
  WindowFunctionOn(FunctionPtr base, Range domain, Range codomain): m_function(base),m_domain(domain&m_function->domain()), m_codomain(codomain&m_function->domain())
  {
    Q_ASSERT(base);
  }
  public:
    Range domain() const override {return m_domain;}
    Range codomain() const override {return m_codomain;}
    double f(double val) const override {if(!m_domain.contains(val) || !m_function)return qQNaN(); else return m_function->f(val);}
    double min(const Range &source) const override {return m_function->min(source&m_domain);}
    double max(const Range &source) const override {return m_function->max(source&m_domain);}
    double avg(const Range &source) const override {return m_function->avg(source&m_domain);}
    using Function::image;
    Range image(const BasicRange &source) const override {return m_function->image(Range(source)&m_domain);}
    Range image(const Range &source) const override {return m_function->image(source&m_domain);}
    Range preimage(double val) const override {
      if(!m_codomain.contains(val))
        return Range();
      else
        return m_function->preimage(val);
    }
    Range preimage(const BasicRange & source) const override {return m_function->preimage(Range(source)&m_codomain); }
    Range preimage(const Range & source) const override {return m_function->preimage(source & m_codomain); }
    double invert(double val) const override {if(!m_codomain.contains(val))return qQNaN();else return m_function->invert(val); }
//    Function *inverse override(const Range &domain=Range(-INFINITY,INFINITY)) const
//    {
//      Range newDomain=m_domain&domain;
//      Function *ret;
//      ret=m_function->inverse(newDomain);
//      if(ret==NULL)
//        return NULL;
//      else
//        return newWindowOnNoCloneCheck(ret,newDomain);
//    }

  double derive(double val) const override {if(!m_domain.contains(val))return qQNaN();else return m_function->derive(val);}
  double integrate(double val) const override {if(!m_domain.contains(val))return qQNaN();else return m_function->integrate(val);}
////  virtual Function *clone() const
////  {
////    Function *f=m_function->clone();
////    return new WindowFunctionOn(f,m_domain,m_codomain);
////  }

  Flags flags() const override {return m_function->flags();}

  static Function *newWindowOnNoCheck(FunctionPtr f, Range domain, Range codomain=Range(-INFINITY,INFINITY));
////  static Function *newWindowOnNoClone(Function *f, Range domain, Range codomain=Range(-INFINITY,INFINITY));
  static FunctionPtr newWindowOn(FunctionPtr f, Range domain, Range codomain=Range(-INFINITY,INFINITY))
  {
    WindowFunctionOn *winOn=dynamic_cast<WindowFunctionOn *>(f.data());
    if(winOn)
      return FunctionPtr(new WindowFunctionOn(winOn->m_function,winOn->m_domain&domain,codomain));
    else
      return FunctionPtr(new WindowFunctionOn(f,domain,codomain));
  }
  QString toString(const QString &x) const override { return QString("([...] on %1)").arg(x); }
protected:
  FunctionPtr m_function;
  Range m_domain;
  Range m_codomain;
};

}
