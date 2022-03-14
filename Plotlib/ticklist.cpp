#include "ticklist.h"


void TickList::addMain(double y, double x, int precision)
{
  m_ticksUpdate++;
  m_ticks.insert(y, x);
  m_ticksPrecision.insert(y, precision);
}

void TickList::addMain(double y, double x)
{
  m_ticksUpdate++;
  m_ticks.insert(y, x);
}

void TickList::addSub(double y, double x)
{
  m_subTicksUpdate++;
  m_subTicks.insert(y, x);
}

bool TickList::hasPrecision(int i, int *precision)
{
  double yV=y(i);
  bool ret=m_ticksPrecision.contains(yV);
  if(precision && ret)
    *precision=m_ticksPrecision[yV];
  return ret;
}

int TickList::precision(int i, bool *valid)
{
  double yV=y(i);
  if(valid)
    *valid=m_ticksPrecision.contains(yV);
  return m_ticksPrecision.value(yV, 0);
}
