#ifndef TICKLIST_H
#define TICKLIST_H
#include "../Mathlib/function.h"
#include <QMap>
#include <QSet>


class TickList
{
public:
  TickList(): m_ticksUpdate(0), m_ticksYUpdate(0) { }
  void addMain(double y, double x, int precision);
  void addMain(double y, double x);
  void addSub(double y, double x);
  int size() const { return m_ticks.size(); }
  double x(int i) const { updateTickY(); return (i>=0 && i<m_ticksY.size())?m_ticks[m_ticksY[i]]:qQNaN(); }
  double y(int i) const { updateTickY(); return (i>=0 && i<m_ticksY.size())?m_ticksY[i]:qQNaN(); }
  bool hasPrecision(int i, int *precision=nullptr);
  int precision(int i, bool *valid=nullptr);
  QString label(int i) const { return QString::number(x(i)); }

  int numSubTicks() const { return m_subTicks.size(); }
  int subTickX(int i) const { updateSubTickY(); return (i>=0 && i<m_subTicksY.size())?m_subTicks[m_subTicksY[i]]:qQNaN(); }
  int subTickY(int i) const { updateSubTickY(); return (i>=0 && i<m_subTicksY.size())?m_subTicksY[i]:qQNaN();  }



  const QMap<double, double> &mainTicks() const { return m_ticks; }
  const QMap<double,double> &subTicks() const          { return m_subTicks; }
  void clear() { m_ticks.clear(); m_subTicks.clear(); m_ticksUpdate++; }
protected:
  int m_ticksUpdate;
  QMap<double, double> m_ticks; // Y->X
  QMap<double, int> m_ticksPrecision; // Y->Precision

  int m_subTicksUpdate;
  QMap<double, double> m_subTicks; // Y->X

  void updateTickY() const { if(m_ticksYUpdate!=m_ticksUpdate){ m_ticksYUpdate=m_ticksUpdate; m_ticksY=m_ticks.keys();} }
  void updateSubTickY() const { if(m_subTicksYUpdate!=m_subTicksUpdate){ m_subTicksYUpdate=m_subTicksUpdate; m_subTicksY=m_subTicks.keys();} }
  mutable QList<double> m_ticksY;
  mutable int m_ticksYUpdate;

  mutable QList<double> m_subTicksY;
  mutable int m_subTicksYUpdate;

};

#endif // TICKLIST_H
