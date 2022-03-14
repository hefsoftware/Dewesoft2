#include "manager.h"
#include "../Visualization/visualization.h"
#include "../Band/band.h"
#include "../Area/area.h"
#include <QPainter>
#include <QDebug>
using namespace Plot;
void Manager::widgetPressEvent(const MouseEvent &event)
{
  m_dragStart.insert(event.button(), event);
}

void Manager::widgetMoveEvent(const MouseEvent &event)
{
  ensureLayoutNotDirty();
  if(m_dragMoveActive)
    updateDragMove(event.pos());
  for(auto it=m_dragStart.begin();it!=m_dragStart.end();)
  {
    if((event.screenPos()-(*it).screenPos()).manhattanLength()>m_dragDistance)
    {
      auto e(it.value());
      MouseEvent ev(e.screenPos(), e.pos(), e.button(), e.buttons(), event.modifiers());
      if(dragStart(ev))
        m_dragActive.insert(it.key(), ev);
      it=m_dragStart.erase(it);
    }
    else
      it++;
  }
  mouseMove(event);
}

void Manager::widgetReleaseEvent(const MouseEvent &event)
{
  ensureLayoutNotDirty();
  if(m_dragActive.contains(event.button()))
  {
    m_dragActive.remove(event.button());
    dragEnd(event);
  }
  else
  {
    mouseClick(event);
    m_dragStart.remove(event.button());
  }
}

Manager::Manager(QObject *parent): QObject(parent), m_layoutDirty(true), m_dragMoveActive(false), m_widget(nullptr)
{
  m_debugView=false;
}

QSizeF Manager::size() const
{
  return m_size;
}

QList<Item *> Manager::items()
{
  QList<Item *> ret;
  auto b=bands();
  auto a=areas();
  ret.append(QList<Item *>(b.begin(), b.end()));
  ret.append(QList<Item *>(a.begin(), a.end()));
  return ret;
}

void Manager::render(QPainter &painter, bool screen)
{
  auto b=bands();
  QList<Plot::Band *> bl(b.begin(), b.end());
  render(painter, bl, areas(), screen);
}

void Manager::invalidateLayout()
{
  if(!m_layoutDirty)
  {
    m_layoutDirty=true;
    if(m_widget)
      m_widget->layoutChanged();
  }
}

void Manager::invalidateData(Band *band)
{
  if(band)
  {
    bool updated=false;
    updated=band->markDirty();
    if(m_widget && updated)
      m_widget->notifyUpdate({band});
  }
}

void Manager::update(Band *band)
{
  if(band)
  {
    bool updated=false;
    updated=band->markUpdate();
    if(m_widget && updated)
      m_widget->notifyUpdate({band});
  }
}

void Manager::update()
{
  auto b=bands();
  QSet<Plot::Band *> dirtyBands;
  for(auto it=b.begin();it!=b.end();it++)
    if((*it)->markUpdate())
      dirtyBands.insert(*it);
  if(m_widget && !dirtyBands.isEmpty())
    m_widget->notifyUpdate(dirtyBands);
}

void Manager::setSize(const QSizeF &size)
{
  if(m_size!=size)
  {
    m_size = size;
    invalidateLayout();
  }
}

bool Manager::ensureLayoutNotDirty()
{
  bool ret=m_layoutDirty;
  if(ret)
  {
    relayout();
    m_layoutDirty=false;
  }
  return ret;
}

void Manager::render(QPainter &painter, const QList<Plot::Band *> &bands, const QList<Plot::Area *> &areas, bool screen)
{
  for(int i=0;i<bands.size();i++)
  {
    auto cb=bands[i];
    if(cb->isShown() && !cb->isPlotArea())
    {
      cb->ensureNotDirty();
      painter.save();
      painter.setTransform(cb->transform(), true);
      if(m_debugView)
      {
        auto oldp(painter.pen());
        painter.setPen(QPen(Qt::darkGray));
        painter.drawRect(cb->fullDrawRect());
        painter.drawText(cb->fullDrawRect(), Qt::AlignCenter, QString::fromUtf8(cb->metaObject()->className()));
        painter.setPen(oldp);
      }
      cb->paint(painter, screen);
      cb->markClean();
      painter.restore();
    }
  }
  for(int i=0;i<areas.size();i++)
  {
    auto ca=areas[i];
    ca->ensureNotDirty();
    painter.save();
    painter.setTransform(ca->transform(), true);
    ca->paint(painter, screen);
    ca->markClean();
    painter.restore();
  }
}

bool Manager::debugView() const
{
  return m_debugView;
}

void Manager::setDebugView(bool debugView)
{
  m_debugView = debugView;
  update();
}

void Manager::onDataDirty()
{
  if(auto band=dynamic_cast<Band *>(sender()))
    invalidateData(band);
}

void Manager::onVisualizationDirty()
{
  if(auto band=dynamic_cast<Band *>(sender()))
    update(band);
}


bool Manager::startDragMove(const QPointF &point)
{
  bool ret=false;
  if(!m_dragMoveActive)
  {
    m_dragMoveActive=true;
    m_dragMovePoint=point;
    ret=true;
  }
  return ret;
}

void Manager::updateDragMove(const QPointF &point)
{
  if(m_dragMoveActive)
  {
    bool ok=true;
    for(auto it=m_dragMoveBands.begin();ok && it!=m_dragMoveBands.end();it++)
      ok&=(*it)->updateDragMove((*it)->transform().inverted().map(point));
    if(!ok)
      for(auto it=m_dragMoveBands.begin();it!=m_dragMoveBands.end();it++)
        (*it)->updateDragMove((*it)->transform().inverted().map(m_dragMovePoint));
  }
}

void Manager::endDragMove()
{
  if(m_dragMoveActive)
  {
    for(auto it=m_dragMoveBands.begin();it!=m_dragMoveBands.end();it++)
      (*it)->endDragMove();
    m_dragMoveBands.clear();
    m_dragMoveActive=false;
  }
}

bool Manager::isDragMoving()
{
  return m_dragMoveActive;
}

QSet<Plot::Band *> Manager::dragMovedBands()
{
  return m_dragMoveBands;
}


bool Manager::addDragMoveBands(const QSet<Plot::Band *> &bands)
{
  bool ret=true;
  for(auto cb: bands)
  {
    ret&=addDragMoveBand(cb);
  }
  return ret;
}
bool Manager::addDragMoveBand(Plot::Band *band)
{
  bool ret=false;
  if(m_dragMoveActive && band && !m_dragMoveBands.contains(band))
  {
    if(band->startDragMove(band->transform().inverted().map(m_dragMovePoint)))
    {
      ret=true;
      m_dragMoveBands.insert(band);
    }
  }
  return ret;
}

void Manager::removeDragMoveBand(Plot::Band *band)
{
  if(m_dragMoveBands.contains(band))
  {
    band->endDragMove();
    m_dragMoveBands.remove(band);
  }
}
