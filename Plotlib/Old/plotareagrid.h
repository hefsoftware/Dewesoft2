#ifndef PLOTAREAGRID_H
#define PLOTAREAGRID_H

#include "plotarea.h"
class TickList;
class PlotAreaGrid : public PlotArea
{
  Q_OBJECT
public:
  PlotAreaGrid();
  void plot(QPainter &p, QSizeF size) override;
  TickList *ticksX() const;
  void setTicksX(TickList *ticksX);
  TickList *ticksY() const;
  void setTicksY(TickList *ticksY);

protected:
  TickList *m_ticksX, *m_ticksY;
};

#endif // PLOTAREAGRID_H
