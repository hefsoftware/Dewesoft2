#include "sum.h"
using namespace HEF;

StatAccumulator::StatAccumulator()
{
  m_max=m_min=m_lastX=m_lastY=qQNaN();
}

void StatAccumulator::clear()
{
  m_max=m_min=m_lastX=m_lastY=qQNaN();
  m_deltaX=0.;
  m_sum.clear();
  m_squaredSum.clear();
}

void StatAccumulator::feed(double x, double y)
{
  m_min=fmin(y,m_min);
  m_max=fmax(y,m_max);
  if(!qIsNaN(m_lastX) && !qIsNaN(m_lastY) && !qIsNaN(x) && !qIsNaN(y))
  {
    m_deltaX+=x-m_lastX;
    m_sum+=integralSawtooth(m_lastX, m_lastY, x, y);
    m_squaredSum+=integralSquaredSawtooth(m_lastX, m_lastY, x, y);
  }
  m_lastX=x;
  m_lastY=y;
}
