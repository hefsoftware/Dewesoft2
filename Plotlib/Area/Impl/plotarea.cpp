#include "plotarea.h"
#include "../../Band/band.h"
#include <QTransform>
using namespace Plot;

Area::Area(): m_zOrder(0.), m_parentH(nullptr), m_parentV(nullptr), m_parentArea(nullptr)
{

}

double Area::zOrder() const
{
  return m_zOrder;
}

bool Area::setZOrder(double zOrder)
{
  m_zOrder = zOrder;
  layoutDirty();
  return true;
}

Band *Area::parentH() const
{
  return dynamic_cast<Band *>(m_parentH);
}

bool Area::setParentH(Band *parentH)
{
  bool ret=false;
  if(!parentH || parentH->horizontal())
    ret=setParentPointer(m_parentH, parentH);
  return ret;
}

Band *Area::parentV() const
{
  return dynamic_cast<Band *>(m_parentV);
}

bool Area::setParentV(Band *parentV)
{
  bool ret=false;
  if(!parentV || parentV->vertical())
    ret=setParentPointer(m_parentV, parentV);
  return ret;
}

Area *Area::parentArea() const
{
  return dynamic_cast<Area *>(m_parentArea);
}

bool Area::setParentArea(Area *parent)
{
  bool ret=setParentPointer(m_parentArea, parent);
  return ret;
}


QTransform Plot::Area::transform()
{
  return QTransform::fromTranslate(m_rect.left(), m_rect.top());
}

