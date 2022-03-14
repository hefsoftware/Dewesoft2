#include "basicfunctions.h"
#include <QtMath>

using namespace HEF;

FunctionPtr LogarithmicFunction::optimizedComposition(const FunctionPtr &other) const
{
  FunctionPtr ret;
  if(LinearFunction *g=dynamic_cast<LinearFunction *>(other.data()))
    ret=LogarithmicFunction::newGeneric(m_a, 0, m_c*g->gain(), m_d+m_c*g->offset());
  return ret;
}
FunctionPtr LogarithmicFunction::optimizedInversedComposition(const FunctionPtr &other) const
{
  FunctionPtr ret;
  if(LinearFunction *g=dynamic_cast<LinearFunction *>(other.data()))
    ret=LogarithmicFunction::newGeneric(m_a*g->gain(), g->offset(), m_c, m_d);
  return ret;
}

FunctionPtr LogarithmicFunction::newFromTwoPoints(double x0,double y0, double x1, double y1)
{
  x0=log(x0);
  x1=log(x1);
  double a=(y1-y0)/(x1-x0);
  double c=exp((y0-a*x0)/a);
  return FunctionPtr(new LogarithmicFunction(a,c,0));
}

QString LogarithmicFunction::toString(const QString &x) const
{
  return QString("(%1ln(%2%4%3))").arg(m_a).arg(m_c).arg(nonZeroToSignedString(m_d),x);
}

//FunctionPtr ExponentialFunction::optimizedInversedComposition(const FunctionPtr &other) const
//{
//  LinearFunction *f;
//  LogarithmicFunction *g;
//  FunctionPtr ret;
//  if((f=dynamic_cast<LinearFunction *>(other.data())))
//  {
//    qDebug()<<"Optimizing linear";
//    // f(x)=ax+b f(this(x))
//    //\f$ y = a p ^{ c x + d} +b \f$
//    // m_a*exp(x*m_c)+m_b
//    ret=ExponentialFunction::newGeneric(M_E,f->gain()*m_a,f->gain()*m_b+f->offset(),m_c);
//  }
//  else if((g=dynamic_cast<LogarithmicFunction *>(other.data())))
//  {
//    /**
//      - Inversed composition with \f$ y=a ln (c x +d) \f$
//      \f[ f_1(x)= a_1 ln ( c_1 x  + d_1) \f]
//      \f[ f_2(x)= a_2 e^{c_2 x } + b_2 \f]
//      \f[ f_1 \circ f_2 = a_1 ln ( c_1 ( a_2 e^{c_2 x } + b_2 ) + d_1) = a_1 ln ( (c_1  a_2) e^{c_2 x } + (c_1 b_2  + d_1)) \f]
//      If \f$ c_1 b_2  + d_1 = 0 \Rightarrow f_1 \circ f_2 = a_1 ln ( (c_1  a_2) e^{c_2 x }) = a_1 (ln(e^{c_2 x })+ln (c_1 a_2 )) = a_1 (c_2 x + ln (c_1 a_2 ) )=a_1 c_2 x + a_1 ln (c_1 a_2) \f$
//    */
//    if(fuzzyEqual(g->c()*b(),-g->d()))
//      ret=LinearFunction::newFromGainOffset(g->a()*c(), g->a()* log (g->c()*a()));
//  }
//  return ret;
//}

FunctionPtr ExponentialFunction::newGenericInverse(double base, double a, double b, double c, double d)
{
  return LogarithmicFunction::newGenericExponentialInverse(base,a,b,c,d);
}

//FunctionPtr ExponentialFunction::optimizedComposition(const FunctionPtr &other) const
//{
//  LinearFunction *f;
//  LogarithmicFunction *g;
//  FunctionPtr ret=NULL;
//  if((f=dynamic_cast<LinearFunction *>(other.data())))
//  {
//    // f(x)=ax+b this(f(x))
//    //\f$ y = a p ^{ c x + d} +b \f$
//    // m_a*exp(x*m_c)+m_b
//    ret=ExponentialFunction::newGeneric(M_E,m_a,m_b,m_c*f->gain(),m_c*f->offset());
//  }
//  else if((g=dynamic_cast<LogarithmicFunction *>(other.data())))
//  {
//    /**
//      - Composition with \f$ y=a ln (c x +d) \f$
//      \f[ f_1(x)= a_1 e^{c_1 x } + b_1 \f]
//      \f[ f_2(x)= a_2 ln ( c_2 x  + d_2) \f]
//      \f[ f_1 \circ f_2 = a_1 e^{c_1 a_2 ln ( c_2 x  + d_2) } + b_1 = a_1 ( e^ { ln ( c_2 x  + d_2) } ) ^ {c_1 a_2} + b_1 = a_1 (  c_2 x  + d_2 ) ^ {c_1 a_2} + b_1  \f]
//      If \f$ c_1 a_2 = 1 \Rightarrow f_1 \circ f_2 = a_1 (  c_2 x  + d_2 ) ^ {c_1 a_2} + b_1 = a_1 (  c_2 x  + d_2 ) + b_1 = (a_1 c_2) x  + (a_1 d_2  + b_1) \f$
//    */
//    /* \f{ c_1 a_2 = 1 \f} : \f{ f_1 \circ f_2 = a_1 (  c_2 x  + d_2 ) ^ {c_1 a_2} + b_1 = a_1 (  c_2 x  + d_2 ) ^ + b_1 = (a_1 c_2) x  + (a_1 d_2  + b_1) \f} */
//    if(fuzzyEqual(m_c*g->a(),1.0))
//    {
//      double infValue=g->c()*INFINITY;

//      //ret=WindowFunctionOn::newWindowOnNoCloneCheck(LinearFunction::newFromGainOffset(m_a*g->c(),m_a * g->d()  + m_b),Range(nextafter(-g->d()/g->c(),infValue),infValue));
//      ret=WindowFunctionOn::newWindowOnCheck(LinearFunction::newFromGainOffset(m_a*g->c(),m_a * g->d()  + m_b),Range(-g->d()/g->c(),infValue));
//    }
//  }
//  return ret;
//}

LinearFunction::Ptr LinearFunction::newFromTwoPoints(double x0, double y0, double x1, double y1)
{
  double k=(y1-y0)/(x1-x0);
  double y_orig=y0-x0*k;
  return Ptr(new LinearFunction(k,y_orig));
}

HEF::FunctionPtr HEF::linearTwoPoints(double x0, double y0, double x1, double y1)
{
  return LinearFunction::newFromTwoPoints(x0, y0, x1, y1);
}

HEF::FunctionPtr HEF::linearGainOffset(double k, double y0)
{
  return LinearFunction::newFromGainOffset(k, y0);
}

FunctionPtr HEF::linearGainOffset(FunctionPtr ptr, double k, double y0)
{
  return ptr?linearGainOffset(k, y0)->fOf(ptr):nullptr;
}

HEF::FunctionPtr HEF::logarithmicTwoPoints(double x0, double y0, double x1, double y1)
{
  return LogarithmicFunction::newFromTwoPoints(x0, y0, x1, y1);
}

QString LinearFunction::toString(const QString &x) const
{
  return QString("(%1*%3%2)").arg(m_k).arg(nonZeroToSignedString(m_y0),x);
}
FunctionPtr LinearFunction::inverse(const Range &range) const
{
  FunctionPtr ret;
  if(range.isEqual(Range(-INFINITY,INFINITY)))
    ret=FunctionPtr(new LinearFunction(1./m_k,-m_y0/m_k));
  return ret;
}
//Function *LinearFunction::optimizedComposition(Function *other) const
//{
//  LinearFunction *f;
//  Function *ret=NULL;
//  if(NULL!=(f=dynamic_cast<LinearFunction *>(other)))
//  {
//    // f(x)=ax+b this(f(x))
//    ret=new LinearFunction(m_k*f->m_k,m_k*f->m_y0+m_y0);
//  }
//  return ret;
//}
//Function *WindowFunctionOn::newWindowOnNoClone(Function *f, Range domain, Range codomain)
//{
//  if(f==NULL)
//    return NULL;
//  domain=domain & f->domain();
//  if(!codomain.isEqual(Range(-INFINITY,INFINITY)))
//    domain=domain & f->preimage(codomain);
//  codomain=f->image(domain);
//  return new WindowFunctionOn(f,domain,codomain);
//}
//FunctionPtr WindowFunctionOn::newWindowOnNoCloneCheck(const FunctionPtr &f, Range domain, Range codomain)
//{
//  WindowFunctionOn *winOn=dynamic_cast<WindowFunctionOn *>(f);
//  FunctionPtr ret=f;
//  if(winOn)
//  {
//    ret=newWindowOnNoClone(winOn->m_function,winOn->m_domain&domain,codomain);
//    winOn->m_function=NULL;
//    delete winOn;
//  }
//  else
//    ret=newWindowOn(f,domain,codomain);
//  return ret;
//}


