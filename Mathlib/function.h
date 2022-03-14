#pragma once

#include "range.h"
#include "rangemap.h"
#include <QVector>
#include <QSharedPointer>
namespace HEF
{
/*class FunctionFlagException: std::exception
{
public:
  FunctionFlagException(QString desc){m_desc=desc;}
  virtual const char* what() const throw(){return qPrintable(m_desc);}

protected:
  QString m_desc;
};*/
  bool fuzzyEqual(double a, double b, double epsilon=1e-10);
/** \brief A generic function

*/
class Function: public QEnableSharedFromThis<Function>
{
public:
    virtual ~Function() { }
    typedef QSharedPointer<Function> Ptr;
    inline Ptr ptr() { Ptr ret=sharedFromThis(); if(!ret)ret=Ptr(this); return ret; }
    enum Flags
    {
      noProperty=0,
      // Function properties (invertible, bijective, montone)
      loosely_invertible=0x0002, ///< Loosely invertible function: \f$ R \subseteq Dom(f) \Rightarrow f^{-1}(f(R))=R \land S \subseteq Cod(f) \Rightarrow f(f^{-1}(S))=S \f$
      invertible=0x0004, ///< Invertible function: \f$ x \in Dom(f) \Rightarrow f^{-1}(f(x))=x \land y \in Cod(f) \Rightarrow f(f^{-1}(y))=y \f$
      bijective=0x0008,
      loosely_monotone=0x0010, ///< Loosely monotone function: \f$x_1 < x_2 \Rightarrow f(x_1) <= f(x_2) \f$
      monotone=0x0020, ///< Monotone function: \f$x_1 < x_2 \Rightarrow f(x_1) < f(x_2) \f$
      // Data properties (sampling type)
      sampled=0x0040,
      // Calculation properties (computed, long computation)
      computed=0x0080,
      // Has statistic functions
      hasMin=0x1000,
      hasMax=0x2000,
      hasAvg=0x4000,
      hasVariance=0x8000
    };

    /** \brief Domain of a function
        \return The domain of a function

        The domain is defined as the set of "input" or argument values for which the function is defined.
        e.g. domain(sqrt)=[0,+inf)
    */
    virtual Range domain() const{return Range(-qInf(),qInf());}
    /** \brief Codomain of a function
        \return The codomain of a function

        The codomain is defined as the set into which all of the output of the function is constrained to fall.
        e.g. codomain(sin)=[-1,+1]
    */
    virtual Range codomain() const {return Range(-qInf(),qInf());}
    /** \brief Value of a function in a point
        \param val Value for which calculating the function
        \return f(x)
    */
    virtual double f(double val) const=0;

    /** \brief Value of the function for a x that has a value as high as possible within the given range
        \param range Range in which selecting the values
        \return f(x)
    */
    virtual double superiorF(const Range &range) const;

    /** \brief Value of the function for a x that has a value as low as possible within the given range
        \param range Range in which selecting the values
        \return f(x)
    */
    virtual double inferiorF(const Range &range) const;

    /** \brief Returns f(g(x))
        \param g Function to combine with this
        \return A function h such that h(x)=f(g(x))
    */
    Ptr fOf(Ptr g);

    //! \defgroup StatisticalOperations Statistical operations

    /** \brief Minimum of a function in a range
        \param source Range for which calculating the minimum
        \return Minimum of the function in the given range
        \ingroup StatisticalOperations

        Return the minimum value of f(x) for each x in the given range.
    */
    virtual double min(const Range &source) const {Q_UNUSED(source); return qQNaN();}
    /** \brief Maximum of a function in a range
        \param source Range for which calculating the maximum
        \return Maximum of the function in the given range
        \ingroup StatisticalOperations

        Return the maximum value of f(x) for each x in the given range.
    */
    virtual double max(const Range &source) const {Q_UNUSED(source); return qQNaN();}
    /** \brief Average of a function in a range
        \param source Range for which calculating the average
        \return Average of the function in the given range
        \ingroup StatisticalOperations

        Return the maximum value of f(x) for each x in the given range.
    */

    virtual double avg(const Range &source) const {Q_UNUSED(source); return qQNaN();}
    /** \brief Image of a basic range
        \param source Domain range for which calculate the image
        \return The mapped codomain for range source

        The image of a range R is defined as the the set I for which \f$ \forall x \in R: f(x) \in I\f$
    */
    virtual Range image(const BasicRange &source) const=0;//{Q_UNUSED(source);return Range();}
    /** \brief Image of a range
        \param source Domain range for which calculate the image
        \return The mapped codomain for range source

        The image of a range R is defined as the the set I for which \f$ \forall x \in R: f(x) \in I\f$
    */
    virtual Range image(const Range &source) const {Range ret;for(int i=0;i<source.numBasicRanges();i++)ret=ret|image(source.basicRange(i));return ret;}
    /** \brief Preimage of a series of ranges
        \param val Value for which calculating the preimage
        \return List of preimages of function in ranges (val[0]..val[1], val1[1]..val[2], val[n-2]...val[n-1], val[n-1]..val[n])

        The preimage of a value y for function f is the subset of its domain X defined by \f$ \{ x \in X | f(x) = y \} \f$.
        Note: returned size is 1 less than argument size
    */
    virtual QList<Range> preimage(const QList<double> &val);

    /** \brief Preimage of a value
        \param val Value for which calculating the preimage
        \return Preimage of function for value val

        The preimage of a value y for function f is the subset of its domain X defined by \f$ \{ x \in X | f(x) = y \} \f$.
        For a bijective function preimage(y)==invert(y)
    */
    virtual Range preimage(double val) const {return preimage(BasicRange(val,val));}
    /** \brief Preimage of a basic range
        \param source Range for which calculating the preimage
        \return Preimage of function for range source

        The preimage of a range R for function f is the subset of its domain X defined by \f$ { x \in X | f(x) \in R } \f$
    */
    virtual Range preimage(const BasicRange & source) const=0;
    /** \brief Preimage of a range
        \param source Range for which calculating the preimage
        \return Preimage of function for range source

        The preimage of a range R for function f is the subset of its domain X defined by \f$ { x \in X | f(x) \in R } \f$
    */
    virtual Range preimage(const Range & source) const {Range ret;for(int i=0;i<source.numBasicRanges();i++)ret=ret|preimage(source.basicRange(i));return ret;}

    /** \brief Inverse of a function in a point
        \param val Value for which calculating the inverse
        \return The inverse of the function for point val or qQNaN() on error

        This function calculates \f$ f^-1(val) \f$
    */
    virtual double invert(double val)const {Q_UNUSED(val); return qQNaN();}

    /** \brief Value of the function for which the function has a value as high as possible within the given range
        \param range Range in which selecting the values
        \return f(x)
    */
    virtual double invertSuperiorF(const Range &range) const;

    /** \brief Value of the function for which the function has a value as low as possible within the given range
        \param range Range in which selecting the values
        \return f(x)
    */
    virtual double invertInferiorF(const Range &range) const;

    /** \brief Inversed function
        \param domain Part of the domain for which calculating the inverse
        \return The inverse of the function in given domain range or nullptr if the function is not invertible in given range.

        This function returns \f$f^-1\f$, if possible.
    */
    virtual Ptr inverse(const Range &domain=Range(-qInf(),qInf()))const {Q_UNUSED(domain); return nullptr;}

    //! \defgroup MathOperations Mathematical operations

    /** \brief Get the derivate of the function in a point
        \ingroup MathOperations
    */
    virtual double derive(double val) const {Q_UNUSED(val); return qQNaN();}
    /** \brief Gets the integral of the function in a point
        \ingroup MathOperations
    */
    virtual double integrate(double val) const {Q_UNUSED(val); return qQNaN();}

//    /** \brief Clones the function
//        \return A perfect copy of the function

//        This function return a copy of the function. Any call for the copy must return the same values as if the calls where
//        made to the original instance
//    */
//    virtual Function *clone() const=0;

    /** \brief Optimized composition
        \param other The other function to compose with
        \return \f$this \circ other\f$ if an optimized version is available, nullptr otherwise

        This function returns a function that calculates \f$this \circ other\f$. In other word f->composition(other)(x) = f(other(x)).
        The returned value should not be nullptr if and only if an optimized composition is available (should not return a mere instance of FunctionComposition or a derived class).
    */
    virtual Ptr optimizedComposition(const Ptr &other) const{Q_UNUSED(other); return nullptr;}
    /** \brief Optimized inversed composition
        \param other The other function to compose with
        \return \f$other \circ this\f$ if an optimized version is available, nullptr otherwise

        This function returns a function that calculates \f$other \circ this\f$. In other word f->inversedComposition(other)(x) = other(f(x)).
        The returned value should not be nullptr if and only if an optimized composition is available (should not return a mere instance of FunctionComposition or a derived class).
    */
    virtual Ptr optimizedInversedComposition(const Ptr &other) const{Q_UNUSED(other); return nullptr;}

    virtual Flags flags() const {return noProperty;}

    virtual QString toString(const QString &x="x") const = 0;
protected:
    static QString numToSignedString(double value);
    static QString nonZeroToSignedString(double value);
};
typedef QSharedPointer<Function> FunctionPtr;


typedef QList<FunctionPtr> FunctionList;
/** Class for describing invertible functions
*/
class InvertibleFunction: public Function
{
public:
  virtual ~InvertibleFunction() {}
  using Function::preimage;
  Range preimage(double val) const override {return preimage(Range(val,val));}
  Range preimage(const Range & source) const override=0;
};
/** Class for describing bijective functions

    All derivations from this class must implement at least following methods:
    f(), invert(), clone()
*/
class BijectiveFunction: public InvertibleFunction
{
public:
  virtual ~BijectiveFunction(){}
  using InvertibleFunction::preimage;
  Range preimage(double val) const override {val=invert(val);return Range(val,val);}
  double invert(double val)const override=0;
};

/** \brief Loosely monotonic functions

    A loosely monotonic function is one where \f$x_1 < x_2 \Rightarrow f(x_1) <= f(x_2) \f$
    By default a loosely monotonic function is loosely invertible
    All derivations from this class must implement at least following methods:
    f(), image(), preimage()
*/
class LooselyMonotonicFunction: public BijectiveFunction
{
public:
  using BijectiveFunction::image;
  Range image(const BasicRange &source) const override=0;
  Range image(const Range &source) const override;

  // Needed only if function is invertible
  using BijectiveFunction::preimage;
  Range preimage(const BasicRange &source) const override=0;
  Range preimage(const Range &source) const override;

  double f(double val) const override;
  double invert(double val) const override;
};

/** \brief Strictly monotonic functions

    A (strictly) monotonic function is one where \f$x_1 < x_2 \Rightarrow f(x_1) < f(x_2) \f$
    All derivations from this class must implement at least following methods:
    f(), invert(), clone()
*/
class MonotonicFunction: public LooselyMonotonicFunction
{
public:
  using BijectiveFunction::image;
  Range image(const BasicRange &source) const override;
  using BijectiveFunction::preimage;
  Range preimage(const BasicRange &source) const override;

  double f(double val) const override=0;
  double invert(double val) const override=0;
};

// Computes g(f(h(x))). Note if no functions is given this will behave as f(x)=x
class FunctionCompositionMulti: public Function
{
protected:
  /** \brief Composes multiple functions into a single one
      \param functions List of functions to compose
      Note: if functions is the list f,g,h the composed function will calculate f(g(h(x)))
      The function DOES NOT clone functions, they must already be a clone
  */
  FunctionCompositionMulti(QList<FunctionPtr> functions): m_functions(functions), m_domainCalculated(functions.isEmpty()), m_codomainCalculated(functions.isEmpty()) { }
public:
  Range domain() const override {
    if(!m_domainCalculated) // Note: If function list size is 0 domainCalculated will be true and domain will
    {
      m_domain=m_functions.last()->domain();
      for(int i=m_functions.size()-2;i>=0;i--)
        m_domain=m_domain & m_functions[i]->preimage(m_domain);
       m_domainCalculated=true;
    }
    return m_domain;
  }
  Range codomain() const override
  {
    if(!m_codomainCalculated) // Note: If function list size is 0 codomainCalculated will be true
    {
       m_codomain=m_functions.first()->codomain();
       for(int i=1;i<m_functions.count();i++)
         m_codomain=m_codomain & m_functions[i]->image(m_codomain);
       m_codomainCalculated=true;
    }
    return m_domain;
  }
  double f(double val) const override
  {
    for(int i=m_functions.size()-1;i>=0;i--)
      val=m_functions[i]->f(val);
    return val;
  }
  double min(const Range &source) const override {Q_UNUSED(source); return qQNaN();}
  double max(const Range &source) const override {Q_UNUSED(source); return qQNaN();}
  double avg(const Range &source) const override {Q_UNUSED(source); return qQNaN();}
  using Function::image;
  Range image(const BasicRange & source) const override {return image(Range(source));}
  Range image(const Range &source) const override;
  using Function::preimage;
  Range preimage(const BasicRange & source) const override {return preimage(Range(source));}
  Range preimage(const Range & source) const override;
  double invert(double val)const override
  {
    for(int i=0;i<m_functions.size();i++)
      val=m_functions[i]->invert(val);
    return val;
  }
  FunctionPtr inverse(const Range &domain=Range(-qInf(),qInf())) const override {Q_UNUSED(domain); return nullptr;}
  double derive(double val) const override {Q_UNUSED(val); return qQNaN();}
  double integrate(double val) const override {Q_UNUSED(val); return qQNaN();}
  Flags flags() const override {return noProperty;}
  static FunctionPtr composeDumb(const FunctionList &functions);
  static FunctionPtr compose(const QList<FunctionPtr> &functions);
  // Returns a composition of two lists. Only the boundary between the two is checked for optimizations
  static FunctionPtr compose(const QList<FunctionPtr> &list1, const QList<FunctionPtr> &list2);

  FunctionPtr optimizedComposition(const Ptr &other) const override;
  FunctionPtr optimizedInversedComposition(const Ptr &other) const override;
  int numFunctions(){return m_functions.size();}
  QString toString(const QString &x) const override;
private:
  QList<FunctionPtr> m_functions;
  mutable bool m_domainCalculated;
  mutable bool m_codomainCalculated;
  mutable Range m_domain;
  mutable Range m_codomain;
};
FunctionPtr linearGainOffset(FunctionPtr base, double k, double y0);
FunctionPtr linearGainOffset(double k, double y0);
FunctionPtr linearTwoPoints(double x0, double y0, double x1, double y1);
FunctionPtr logarithmicTwoPoints(double x0, double y0, double x1, double y1);

}
Q_DECLARE_METATYPE(HEF::FunctionPtr)
