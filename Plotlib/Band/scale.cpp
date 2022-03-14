#include "scale.h"
#include "../tickalgorithmbasic.h"
#include <QPainter>
#include <QDebug>

using namespace Plot;



void BandScaleDefault::paint(QPainter &p, bool screen)
{
  Q_UNUSED(screen);
  bool ticksOnTop=true;
  auto curt=ticks();
  p.setPen(QPen(Qt::black, 1.5));
  double h=height();
  QFont f;
  f.setPixelSize(qMin(h*0.6,12.));
  p.setFont(f);
  double tickH=h*0.2;
  for(int i=0;i<curt.size();i++)
  {
    double x=curt.y(i);
    p.drawLine(x,ticksOnTop?0:h-tickH, x, ticksOnTop?tickH:h);
    p.drawText(x-200,h*(ticksOnTop?0.3:0.),400, h*0.7, Qt::AlignCenter, curt.label(i));
//    qDebug()<<"Scale"<<i<<curt.x(i)<<curt.y(i)<<p.transform().map(QPointF(x,ticksOnTop?0:h-tickH))<<p.transform().map(QPointF(x, ticksOnTop?tickH:h));
  }
  p.setPen(QPen(Qt::black, 1));
  for(int j=0;j<curt.numSubTicks();j++)
  {
    double x=curt.subTickY(j);
    p.drawLine(x,ticksOnTop?0:h-tickH, x, ticksOnTop?tickH:h);
  }
}

void BandScaleDefault::checkNotDirty()
{
  if(m_dirty)
  {
    m_scrolledFunction=HEF::linearGainOffset(m_function, m_scroll.currentScale(), m_scroll.currentOffset());
    TickAlgorithmBasic scale(m_scrolledFunction, 0., width(), &m_ticks);
    scale.setMinTickDistance(60,20);
    scale.recalculate();
    m_dirty=false;
  }
}

void BandScaleDraw::paint(QPainter &p, bool screen)
{
  Q_UNUSED(screen);
//  if(BandScale *scale=dynamic_cast<BandScale *>(rootBand()))
//  {
//    bool ticksOnTop=false;
//    auto curt=scale->ticks();
//    p.setPen(QPen(Qt::black, 1.5));
//    double ratioH=height()/minHeight();
//    if(m_drawLabel)
//    {
//      QFont f;
//      f.setPixelSize(qMin(ratioH*7,12.));
//      p.setFont(f);
//    }
//    double tickH=ratioH*3;
//    for(int i=0;i<curt.size();i++)
//    {
//      double x=curt.y(i);
//      if(m_drawTicks)
//        p.drawLine(x,ticksOnTop?0:height()-tickH, x, ticksOnTop?tickH:height());
//      if(m_drawLabel)
//        p.drawText(x-200,m_drawTicks?(ratioH*(ticksOnTop?3:0.)):0,400, ratioH*9, Qt::AlignCenter, curt.label(i));
//    }
//    p.setPen(QPen(Qt::black, 1));
//    if(m_drawTicks)
//    {
//      for(int j=0;j<curt.numSubTicks();j++)
//      {
//        double x=curt.subTickY(j);
//        p.drawLine(x,ticksOnTop?0:height()-tickH*0.6, x, ticksOnTop?tickH*0.6:height());
//      }
//    }
//  }
}

bool BandScaleDraw::drawTicks() const
{
  return m_drawTicks;
}

void BandScaleDraw::setDrawTicks(bool drawTicks)
{
  m_drawTicks = drawTicks;
  emit dataDirty();
}

bool BandScaleDraw::drawLabel() const
{
  return m_drawLabel;
}

void BandScaleDraw::setDrawLabel(bool drawLabel)
{
  m_drawLabel = drawLabel;
  emit dataDirty();
}

bool BandScaleDefault::startDragMove(const QPointF &pt)
{
  m_scroll.startScroll(pt.x());
  return true;
}

bool BandScaleDefault::updateDragMove(const QPointF &pt)
{
  m_scroll.updateScroll(pt.x());
  return true;
}

void BandScaleDefault::endDragMove()
{
  m_scroll.endScroll();
}

bool Plot::BandScaleDefault::zoom(const QPointF &pt, double angle)
{
  m_scroll.scaleWheelUnit(pt.x(), angle);
  return true;
}
