#include "plotareagrid.h"
#include "ticklist.h"
PlotAreaGrid::PlotAreaGrid()
{

}


void PlotAreaGrid::plot(QPainter &p, QSizeF size)
{
  if(m_ticksX)
  {
    for(int i=0;i<m_ticksX->size();i++)
    {
      auto y=m_ticksX->y(i);
      if(y>1 && y<size.width()-1)
      {
        p.drawLine(y,0,y, size.height());
      }
    }
  }
  if(m_ticksY)
  {
    for(int i=0;i<m_ticksY->size();i++)
    {
      auto y=m_ticksY->y(i);
      if(y>1 && y<size.height()-1)
      {
        p.drawLine(0,y,size.width(),y);
      }
    }
  }
}

TickList *PlotAreaGrid::ticksX() const
{
  return m_ticksX;
}

void PlotAreaGrid::setTicksX(TickList *ticksX)
{
  m_ticksX = ticksX;
}

TickList *PlotAreaGrid::ticksY() const
{
  return m_ticksY;
}

void PlotAreaGrid::setTicksY(TickList *ticksY)
{
  m_ticksY = ticksY;
}
