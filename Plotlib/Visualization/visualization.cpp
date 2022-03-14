#include "visualization.h"
#include "../Manager/manager.h"
using namespace Plot;
PlotVisualization::PlotVisualization(Manager *manager): m_manager(manager)
{
  if(manager)
    manager->m_widget=this;
}

void PlotVisualization::setManagerSize(const QSizeF &size)
{
  if(m_manager)
    m_manager->setSize(size);
}

Manager *PlotVisualization::manager() const
{
  return m_manager;
}

void PlotVisualization::setManager(Manager *manager)
{
    if(m_manager)
      m_manager->m_widget=nullptr;
    m_manager = manager;
    if(m_manager)
      m_manager->m_widget=this;
}

bool PlotVisualization::ensureLayoutNotDirty() const
{
  bool ret=false;
  if(m_manager)
    ret=m_manager->ensureLayoutNotDirty();
  return ret;
}

bool PlotVisualization::isLayoutDirty() const
{
  return (m_manager && m_manager->isLayoutDirty());
}

void PlotVisualization::recalculateLayout() const
{
  if(m_manager)
  {
    m_manager->relayout();
  }
}

void PlotVisualization::emitPressEvent(const MouseEvent &event)
{
  if(m_manager)
      m_manager->widgetPressEvent(event);
}

void PlotVisualization::emitMoveEvent(const MouseEvent &event)
{
    if(m_manager)
        m_manager->widgetMoveEvent(event);
}

void PlotVisualization::emitReleaseEvent(const MouseEvent &event)
{
    if(m_manager)
        m_manager->widgetReleaseEvent(event);
}

void PlotVisualization::emitWheelEvent(const QWheelEvent &event)
{
  if(m_manager)
    m_manager->widgetWheelEvent(event);
}

PlotVisualizationPublic::PlotVisualizationPublic(Manager *manager): PlotVisualization(manager)
{

}
