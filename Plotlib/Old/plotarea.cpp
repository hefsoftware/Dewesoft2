#include "plotarea.h"

PlotArea::PlotArea(QObject *parent) : QObject(parent)
{

}

void PlotArea::setScaleX(const HEF::FunctionPtr &scaleX)
{
  m_scaleX = scaleX;
}

void PlotArea::setScaleY(const HEF::FunctionPtr &scaleY)
{
  m_scaleY = scaleY;
}
