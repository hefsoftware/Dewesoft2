#pragma once

#include "plotarea.h"
class TickList;
namespace Plot
{
  class AreaGrid: public Area
  {
    Q_OBJECT
  public:
    AreaGrid();
    void paint(QPainter &painter, bool screen) override;
  protected:
    void paint(QPainter &painter, const TickList &ticks, double width, double height);
  };
}
