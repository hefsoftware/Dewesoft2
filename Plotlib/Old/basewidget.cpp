#include "basewidget.h"

BaseWidget::BaseWidget(QWidget *parent) : QWidget(parent)
{

}

bool BaseWidget::dragStart(const QMouseEvent &event)
{
  Q_UNUSED(event);
  return false;
}

void BaseWidget::dragEnd(const QMouseEvent &event)
{
  Q_UNUSED(event);
}

void BaseWidget::mouseClick(const QMouseEvent &event)
{
  Q_UNUSED(event);
}

void BaseWidget::mouseMove(const QMouseEvent &event)
{
  Q_UNUSED(event);
}

void BaseWidget::mouseMoveEvent(QMouseEvent *event)
{
  for(auto it=m_dragStart.begin();it!=m_dragStart.end();)
  {
    if((event->screenPos()-(*it).screenPos()).manhattanLength()>m_dragDistance)
    {
      auto ev=it.value();
      ev.setModifiers(event->modifiers());
      if(dragStart(ev))
        m_dragActive.insert(it.key(), ev);
      it=m_dragStart.erase(it);
    }
    else
      it++;
  }
  mouseMove(*event);
  event->accept();
}

void BaseWidget::mousePressEvent(QMouseEvent *event)
{
  m_dragStart.insert(event->button(), QMouseEvent(*event));
  event->accept();
}

void BaseWidget::mouseReleaseEvent(QMouseEvent *event)
{
  if(m_dragActive.contains(event->button()))
  {
    m_dragActive.remove(event->button());
    dragEnd(*event);
  }
  else
  {
    mouseClick(*event);
    m_dragStart.remove(event->button());
  }
  event->accept();
}
