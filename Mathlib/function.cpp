#include "function.h"
#include "basicfunctions.h"
#include <QDebug>
//#include <math.h>
namespace HEF
{
  bool fuzzyEqual(double a, double b, double epsilon)
  {
      const double absA = qAbs(a);
      const double absB = qAbs(b);
      const double diff = qAbs(a - b);

      if (a == b) { // shortcut, handles infinities
          return true;
      } else if (a * b == 0) { // a or b or both are zero
          // relative error is not meaningful here
          return diff < (epsilon * epsilon);
      } else { // use relative error
          return diff / (absA + absB) < epsilon;
      }
  }

  // LooselyMonotonicFunction
  double LooselyMonotonicFunction::f(double val) const
  {
    Range s=image(BasicRange(val));
    double min,max;
    min=s.min();
    max=s.max();
    if(min==-INFINITY && max!=-INFINITY)
    {
      min=nextafter(min,INFINITY);
    }
    else if(max==INFINITY && min!=INFINITY)
    {
      max=nextafter(max,-INFINITY);
    }
    return (min+max)*0.5;
  }
  double LooselyMonotonicFunction::invert(double val) const
  {
    Range s=preimage(BasicRange(val));
    double min,max;
    min=s.min();
    max=s.max();
    if(min==-INFINITY && max!=-INFINITY)
    {
      min=nextafter(min,INFINITY);
    }
    else if(max==INFINITY && min!=INFINITY)
    {
      max=nextafter(max,-INFINITY);
    }
    return (min+max)*0.5;
  }

  Range LooselyMonotonicFunction::image(const Range &source) const
  {
//    return (source&domain()).remapBasic()
//    Function::MapToImageBasic mapper(*this);
    return (source&domain()).remapBasic([this](const BasicRange &r){return this->image(r); });
  }
  Range LooselyMonotonicFunction::preimage(const Range &source) const
  {
//    Function::MapToPreimageBasic mapper(*this);
//    return (source&codomain()).remapBasic(mapper);
    return (source&codomain()).remapBasic([this](const BasicRange &r){return this->preimage(r); });
  }

  // MonotonicFunction
  Range MonotonicFunction::image(const BasicRange &source) const
  {
    return Range(f(source.min()),f(source.max()));
  }
  Range MonotonicFunction::preimage(const BasicRange &source) const
  {
    return Range(invert(source.min()),invert(source.max()));
  }

 /* Range MonotonicFunction::image(const Range &source) const
  {
    MapToImageBasic mapper(*this);
    return (source&domain()).remapBasic(mapper);
  }
  Range MonotonicFunction::preimage(const Range &source) const
  {
    MapToPreimageBasic mapper(*this);
    return (source&codomain()).remapBasic(mapper);
  }*/


  FunctionPtr FunctionCompositionMulti::compose(const QList<FunctionPtr> &functions)
  {
    QList<FunctionPtr> ret;
    FunctionPtr last;
    FunctionPtr current;
    for(int i=0;;)
    {
      if(!current)
      {
        if(i>=functions.size())
          break;
        else
        {
          current=functions[i];
          i++;
          if(!current) // Skip null functions
            continue;
        }
      }
      if(!last)
        current.swap(last); // Moves current to last and then sets it to null
      else
      {
        FunctionPtr optimized;
        if((optimized=last->optimizedComposition(current)) || (optimized=current->optimizedInversedComposition(last)))
        {
          qDebug()<<"Combine"<<last->toString()<<"and"<<current->toString()<<"into"<<optimized->toString();
          current=optimized;
          last=ret.isEmpty()?nullptr:ret.takeLast();
        }
        else
        {
          // Shifts everything left
          qDebug()<<"Failed to combine"<<last->toString()<<current->toString();
          ret.append(last);
          last=current;
          current.reset();
        }
      }
    }
    if(last)
      ret.append(last);
    for(int i=0;i<ret.size();i++)
      qDebug()<<i<<ret[i]->toString();
    if(ret.size()==1)
      return ret.first();
    else
      return FunctionPtr(new FunctionCompositionMulti(ret));
  }

  FunctionPtr FunctionCompositionMulti::compose(const QList<FunctionPtr> &list1, const QList<FunctionPtr> &list2)
  {
    FunctionList ret;
    ret.append(list1);
    ret.append(list2);
    if(!list1.isEmpty() && !list2.isEmpty())
    {
      FunctionPtr optimized;
      int index=list1.size(); // Check this item and the next/previous
      if((optimized=ret[index-1]->optimizedComposition(ret[index])) || (optimized=ret[index]->optimizedInversedComposition(ret[index-1]))) // First case in the boundary
      {
        //We get an optimization on the boundary
        ret.removeAt(index);
        index--;
        ret[index]=optimized;
        while(true)
        {
          if(index<ret.size()-1 && ((optimized=ret[index]->optimizedComposition(ret[index+1])) || (optimized=ret[index+1]->optimizedInversedComposition(ret[index]))))
          {
            ret[index]=optimized;
            ret.removeAt(index+1);
          }
          else if(index>0 && ((optimized=ret[index-1]->optimizedComposition(ret[index])) || (optimized=ret[index]->optimizedInversedComposition(ret[index-1]))))
          {
            ret.removeAt(index);
            index--;
            ret[index]=optimized;
          }
          else
            break; // No further optimization possible
        }
      }
    }
    if(ret.size()==1)
      return ret.first();
    else
      return FunctionPtr(new FunctionCompositionMulti(ret));
  }

  Range FunctionCompositionMulti::image(const Range &source) const {
    Range r=source;
    for(int i=m_functions.size()-1;i>=0;i--)
      r=m_functions[i]->image(r);

    return r;
  }
  Range FunctionCompositionMulti::preimage(const Range & source) const {
    Range r=source;
    for(int i=0;i<m_functions.size();i++)
      r=m_functions[i]->preimage(r);
    return r;
  }

  FunctionPtr FunctionCompositionMulti::composeDumb(const FunctionList &functions)
  {
    return FunctionPtr(new FunctionCompositionMulti(functions));
  }
  QString FunctionCompositionMulti::toString(const QString &x) const
  {
    QString ret=x;
    for(int i=m_functions.size()-1;i>=0;i--)
      ret=m_functions[i]->toString(ret);
    return ret;
  }

  double Function::superiorF(const Range &range) const
  {
    auto r=range&domain();
    double ret=qQNaN();
    if(r.isEmpty())
      ret=f(r.max());
    return ret;
  }

  double Function::inferiorF(const Range &range) const
  {
    auto r=range&domain();
    double ret=qQNaN();
    if(r.isEmpty())
      ret=f(r.min());
    return ret;
  }

  Function::Ptr Function::fOf(Function::Ptr g)
  {
    return FunctionCompositionMulti::compose({ptr()}, {g});
  }

  QList<Range> Function::preimage(const QList<double> &val)
  {
    QList<Range> ret;
    for(int i=0;i<val.size()-1;i++)
      ret.append(preimage(BasicRange(val[i],val[i+1])));
    return ret;
  }

  double Function::invertSuperiorF(const Range &range) const
  {
    auto r=range&codomain();
    double ret=qQNaN();
    if(r.isEmpty())
      ret=invert(r.max());
    return ret;
  }

  double Function::invertInferiorF(const Range &range) const
  {
    auto r=range&codomain();
    double ret=qQNaN();
    if(r.isEmpty())
      ret=invert(r.min());
    return ret;
  }

  QString Function::numToSignedString(double value)
  {
    return QString(value<0?"-":"+")+QString::number(qAbs(value));
  }
  QString Function::nonZeroToSignedString(double value)
  {
    return fuzzyEqual(value,0)?QString(""):(QString(value<0?"-":"+")+QString::number(qAbs(value)));
  }

  FunctionPtr FunctionCompositionMulti::optimizedComposition(const FunctionPtr &other) const
  {
    FunctionPtr ret;
    if(FunctionCompositionMulti *f=dynamic_cast<FunctionCompositionMulti *>(other.data()))
      ret=compose(m_functions, f->m_functions);
    else
      ret=compose(m_functions, {other});
    return ret;
  }

  FunctionPtr FunctionCompositionMulti::optimizedInversedComposition(const Function::Ptr &other) const
  {
    FunctionPtr ret;
    if(FunctionCompositionMulti *f=dynamic_cast<FunctionCompositionMulti *>(other.data()))
      ret=compose(f->m_functions, m_functions);
    else
      ret=compose({other}, m_functions);
    return ret;
  }

  //  FunctionPtr *FunctionCompositionMulti::composeReverse(QList<FunctionPtr> functions)
  //  {
//    Function *current=NULL;
//    Function *optimized=NULL;
//    QList<HEF::Function *> f_list;
//    if(!functions.isEmpty())
//    {
//      f_list.append(functions.takeLast());
//    else
//      f_list.append(functions.takeLast()->clone());

//    for(;;)
//    {
//      if(current==NULL)
//      {
//        if(functions.size()==0)
//          break;
//        if(removeOriginal)
//          current=functions.takeLast();
//        else
//          current=functions.takeLast()->clone();
//      }
//      if((optimized=f_list.first()->optimizedInversedComposition(current))!=NULL)
//      {
//        delete f_list.takeFirst();
//        delete current;
//        if(f_list.isEmpty())
//        {
//          current=NULL;
//          f_list.append(optimized);
//        }
//        else
//          current=optimized;
//      }
//      else if((optimized=current->optimizedComposition(f_list.first()))!=NULL)
//      {
//        delete f_list.takeFirst();
//        delete current;
//        if(f_list.isEmpty())
//        {
//          current=NULL;
//          f_list.append(optimized);
//        }
//        else
//          current=optimized;
//      }
//      else
//      {
//        f_list.prepend(current);
//        current=NULL;
//      }
//    }
//    }
//    if(f_list.size()==1)
//      return f_list[0];
//    else
//      return new FunctionCompositionMulti(f_list);
//  }

//  Function *FunctionCompositionMulti::dumbCompose(QList<HEF::Function *> functions, bool removeOriginal)
//  {
//    QList<HEF::Function *> f_list;
//    if(removeOriginal)
//      f_list=functions;
//    else
//    {
//      Q_FOREACH(Function *f,functions)
//      {
//        f_list.append(f->clone());
//      }
//    }
//    return new FunctionCompositionMulti(f_list);
//  }

}
