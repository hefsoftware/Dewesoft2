#include "scrollmanager.h"
#include <cmath>
#include <Qt>
ScrollManager::ScrollManager(std::function<bool (double, double)> notifyFunc): m_notifyFunc(notifyFunc), m_scale(1), m_offset(0)
{
  m_isScrolling=false;
}

void ScrollManager::startScroll(double mousePos)
{
  if(!m_isScrolling)
  {
    m_scrollScaleBackup=m_scale;
    m_scrollOffsetBackup=m_offset;
    m_scrollMouseStart=mousePos;
    m_isScrolling=true;
  }
}

void ScrollManager::updateScroll(double mousePos)
{
  if(m_isScrolling)
    recalculateScroll(mousePos, m_scale);
}

void ScrollManager::endScroll(double mousePos)
{
  if(m_isScrolling)
  {
    recalculateScroll(mousePos, m_scale);
    m_isScrolling=false;
  }
}

void ScrollManager::endScroll()
{
  if(m_isScrolling)
    m_isScrolling=false;
}

void ScrollManager::scale(double mousePos, double scale)
{
  if(m_isScrolling)
    recalculateScroll(mousePos, scale);
  else
    setScroll(scale, (m_offset-mousePos)*scale/m_scale+mousePos);
}
void ScrollManager::scaleRelative(double mousePos, double scaleMul)
{
  scale(mousePos, m_scale*scaleMul);
}

void ScrollManager::scaleWheelUnit(double mousePos, double unit)
{
  scaleWheelAngle(mousePos, unit/8.);
}

void ScrollManager::scaleWheelAngle(double mousePos, double angle)
{
  const double singleStepZoom=1.2;
  const int singleStepAngle=15;

  double zoom=std::pow(singleStepZoom,angle/singleStepAngle);
  scale(mousePos, m_scale*zoom);
}

bool ScrollManager::isScrolling() const
{
  return m_isScrolling;
}

double ScrollManager::currentScale()
{
  return m_scale;
}

double ScrollManager::currentOffset()
{
  return m_offset;
}

void ScrollManager::setScroll(double scale, double offset)
{
  if(qIsFinite(scale) && qIsFinite(offset) && scale!=0 &&
     (scale!=m_scale || m_offset!=offset))
  {
    m_scale=scale;
    m_offset=offset;
    if(m_notifyFunc)
      m_notifyFunc(scale, offset);
  }
}
void ScrollManager::recalculateScroll(double mousePos, double scale)
{
  double offset=(m_scrollOffsetBackup-m_scrollMouseStart)*scale/m_scrollScaleBackup+mousePos;
  setScroll(scale, offset);
}

void ScrollManager::setOffset(double offset)
{
    setScroll(m_scale, offset);
}

void ScrollManager::setNotifyFunc(const std::function<bool (double, double)> &notifyFunc)
{
  m_notifyFunc = notifyFunc;
}

void ScrollManager::setNotifyFunc(const std::function<bool ()> &notifyFunc)
{
  setNotifyFunc([notifyFunc](double scale, double offset) { Q_UNUSED(scale); Q_UNUSED(offset); return notifyFunc(); });
}

void ScrollManager::setNotifyFunc(const std::function<void ()> &notifyFunc)
{
  setNotifyFunc([notifyFunc](double scale, double offset) { Q_UNUSED(scale); Q_UNUSED(offset); notifyFunc(); return true; });
}
/*
  y0=x0*s0+o0

  y1=y0*s1+o1

  y=(x*s0+o0-MousePos)*s1+mousePos =>
  y=x*s0*s1 + (o0-mousePos)*s1+mousePos

  y=3*x+40
*/

void ScrollManager::setScale(double scale)
{
  setScroll(scale, m_offset);
}
