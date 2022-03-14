#include "plotareagrid.h"
#include "../../Band/scale.h"
#include <QPainter>
#include <QDebug>
using namespace Plot;
AreaGrid::AreaGrid()
{

}


void AreaGrid::paint(QPainter &painter, bool screen)
{
  auto ticksH=findAncestorPostDFS<BandTicks>(parentH(), false);
  auto ticksV=findAncestorPostDFS<BandTicks>(parentV(), false);
  if(ticksH)
    paint(painter, ticksH->ticks(), width(), height());
  if(ticksV)
  {
    painter.setTransform(QTransform().rotate(90).scale(1, -1), true);
    paint(painter, ticksV->ticks(), height(), width());
  }
}

void AreaGrid::paint(QPainter &painter, const TickList &ticks, double width, double height)
{
  painter.setPen(QPen(Qt::black, 1.5));
  for(int i=0;i<ticks.size();i++)
  {
//    qDebug()<<"Grid"<<i<<ticks.x(i)<<ticks.y(i)<<painter.transform().map(QPointF(ticks.y(i), 0))<<painter.transform().map(QPointF(ticks.y(i), height));
    painter.drawLine(QPointF(ticks.y(i), 0), QPointF(ticks.y(i),height));
  }
//  auto main=ticks.mainTicks();
//  for(auto it=main.begin();it!=main.end();it++)
//    painter.drawLine(QPointF(it.key(), 0), QPointF(it.key(),height));
}
