#include "visualizationwidget.h"
#include "../Manager/manager.h"
#include "../Band/band.h"
#include <QWheelEvent>
#include <QPainter>
#include <QDebug>
#include <math.h>

PlotWidget::PlotWidget(Plot::Manager *manager, QWidget *parent): QWidget(parent), PlotVisualizationPublic(manager)
{

}

void PlotWidget::layoutChanged()
{
  qDebug()<<"PlotWidget::layoutChanged (Widget layout changed)";
  update();
}

void PlotWidget::notifyUpdate(QSet<Plot::Band *> band)
{
//  qDebug()<<"PlotWidget::update"<<band.size()<<" bands (Band need redraw)";
  update();
}

bool PlotWidget::updateSizeCache() const
{
  QSize minSize,bestSize;
  if(manager())
  {
    ensureLayoutNotDirty();
    auto minSizeF=manager()->minimumSize(), bestSizeF=manager()->bestSize();
    minSize=QSize(ceil(minSizeF.width())+1, ceil(minSizeF.height())+1);
    bestSize=QSize(ceil(bestSizeF.width())+1, ceil(bestSizeF.height())+1);
  }
  bool ret=(minSize!=m_minSize) || (bestSize!=m_bestSize);
  if(ret)
  {
//    qDebug()<<"Updated size cache"<<minSize<<bestSize;
    m_minSize=minSize;
    m_bestSize=bestSize;
  }
  return ret;
}

void PlotWidget::mousePressEvent(QMouseEvent *event)
{
  Plot::MouseEvent ev(event->screenPos(),event->pos(), event->button(), event->buttons(), event->modifiers());
  emitPressEvent(ev);
  event->accept();
}

void PlotWidget::mouseReleaseEvent(QMouseEvent *event)
{
  Plot::MouseEvent ev(event->screenPos(),event->pos(), event->button(), event->buttons(), event->modifiers());
  emitReleaseEvent(ev);
  event->accept();
}

void PlotWidget::mouseMoveEvent(QMouseEvent *event)
{
  Plot::MouseEvent ev(event->screenPos(),event->pos(), event->button(), event->buttons(), event->modifiers());
  emitMoveEvent(ev);
  event->accept();
}

void PlotWidget::wheelEvent(QWheelEvent *event)
{
  emitWheelEvent(*event);
  event->accept();
}


QSize PlotWidget::sizeHint() const
{
  ensureLayoutNotDirty();
  updateSizeCache();
  return m_bestSize;
}

QSize PlotWidget::minimumSizeHint() const
{
  ensureLayoutNotDirty();
  updateSizeCache();
  return m_minSize;
}


void PlotWidget::paintEvent(QPaintEvent *event)
{
  constexpr bool debugPaint=true;
  if(manager())
  {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    bool changedLayout=false;
    changedLayout=ensureLayoutNotDirty();
    if(debugPaint)
    {
      auto b=manager()->bands();
      QSet<Plot::Band *> dirty,updated,all;
      for(auto it=b.begin();it!=b.end();it++)
      {
        if((*it)->isPlotArea())
          continue;
        all.insert(*it);
        if((*it)->isDirty())
          dirty.insert(*it);
        else if((*it)->needsUpdate())
          updated.insert(*it);
      }
//      const char *relayoutStr=changedLayout?"relayout":"";
//      if(dirty==all)
//        qDebug()<<"PlotWidget::paintEvent "<<relayoutStr<<" Dirty all "<<all.size()<<"bands";
//      else if(updated==all)
//        qDebug()<<"PlotWidget::paintEvent "<<relayoutStr<<" Update all "<<all.size()<<"bands";
//      else if(!dirty.isEmpty() || !updated.isEmpty())
//      {
//        if(dirty.isEmpty())
//          qDebug()<<"PlotWidget::paintEvent "<<relayoutStr<<" Update: "<<updated;
//        else if(updated.isEmpty())
//          qDebug()<<"PlotWidget::paintEvent "<<relayoutStr<<" Dirty: "<<dirty;
//        else
//          qDebug()<<"PlotWidget::paintEvent "<<relayoutStr<<" Dirty: "<<dirty<<" Update: "<<updated;
//      }
    }
    manager()->render(p);
  }
}


void PlotWidget::resizeEvent(QResizeEvent *event)
{
  qDebug()<<"Resize"<<size();
  setManagerSize(size()-QSize(1,1));
  QWidget::update();
}
