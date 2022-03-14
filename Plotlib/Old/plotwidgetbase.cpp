#include "plotwidgetbase.h"
#include <QPainter>
#include <QDebug>
#include <QResizeEvent>
#include "plotarea.h"
PlotWidgetBase::PlotWidgetBase(QWidget *parent) : BaseWidget(parent), m_bands(3,3,3,3)
{
  m_showedAreas=Area::All;
}

void PlotWidgetBase::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);
  QPainter p(this);
//  int top=0;
//  int left=0;
//  int bottom=height()-1;
//  int right=width()-1;
  int lab=m_showedAreas&Area::GraphLabel?labelHeight():0;
  int llab=m_showedAreas&Area::LeftLabel?leftLabelHeight():0;
  int rlab=m_showedAreas&Area::RightLabel?rightLabelHeight():0;
  int tlab=m_showedAreas&Area::TopLabel?topLabelHeight():0;
  int blab=m_showedAreas&Area::BottomLabel?bottomLabelHeight():0;
  int lsca=m_showedAreas&Area::LeftScale?leftScaleHeight():0;
  int rsca=m_showedAreas&Area::RightScale?rightScaleHeight():0;
  int tsca=m_showedAreas&Area::TopScale?topScaleHeight():0;
  int bsca=m_showedAreas&Area::BottomScale?bottomScaleHeight():0;
  QRectF plotArea(0, 0, width(), height());
  plotArea-=m_bands;
  plotArea-=QMargins(llab+lsca,lab+tlab+tsca, rlab+rsca, blab+bsca);
  p.drawRect(plotArea);
//  p.fillRect(plotArea, QColor(255,192,192));
  if(m_showedAreas&Area::TopScale)
  {
    p.save();
    p.translate(plotArea.topLeft());
    QRectF r(QPoint(-lsca, -tsca), QPoint(plotArea.width()+rsca, 0));
    paintTopScale(p, r, plotArea.width());
    p.restore();
  }
  if(m_showedAreas&Area::BottomScale)
  {
    QRectF r(QPoint(-lsca, 0), QPoint(plotArea.width()+rsca, bsca));
    p.save();
    p.translate(plotArea.bottomLeft());
    paintBottomScale(p, r, plotArea.width());
    p.restore();
  }
  if(m_showedAreas&Area::LeftScale)
  {
    QRectF r(QPoint(-tsca, 0), QPoint(plotArea.height()+bsca, lsca));
    p.save();
    p.translate(plotArea.topLeft());
    p.rotate(90);
    paintLeftScale(p, r, plotArea.height());
    p.restore();
  }
  if(m_showedAreas&Area::RightScale)
  {
    QRectF r(QPoint(-tsca, -rsca), QPoint(plotArea.height()+bsca, 0));
    p.save();
    p.translate(plotArea.topRight());
    p.rotate(90);
    paintRightScale(p, r, plotArea.height());
    p.restore();
  }
  if(m_showedAreas&Area::TopLabel)
  {
    p.save();
    p.translate(plotArea.topLeft()-QPointF(0,tsca));
    QRectF r(QPoint(-llab-lsca, -tlab), QPoint(plotArea.width()+rlab+rsca, 0));
    paintTopLabel(p, r, plotArea.width());
    p.restore();
  }
  if(m_showedAreas&Area::BottomLabel)
  {
    QRectF r(QPoint(-llab-lsca, 0), QPoint(plotArea.width()+rlab+rsca, blab));
    p.save();
    p.translate(plotArea.bottomLeft()+QPointF(0,bsca));
    paintBottomLabel(p, r, plotArea.width());
    p.restore();
  }
  if(m_showedAreas&Area::LeftLabel)
  {
    QRectF r(QPoint(-tlab-tsca, 0), QPoint(plotArea.height()+blab+bsca, llab));
    p.save();
    p.translate(plotArea.topLeft()-QPointF(lsca, 0));
    p.rotate(90);
    paintLeftLabel(p, r, plotArea.width());
    p.restore();
  }
  if(m_showedAreas&Area::RightLabel)
  {
    QRectF r(QPoint(-tlab-tsca, -rlab), QPoint(plotArea.height()+blab+bsca, 0));
    p.save();
    p.translate(plotArea.topRight()+QPointF(rsca,0));
    p.rotate(90);
    paintRightLabel(p, r, plotArea.width());
    p.restore();
  }
  if(m_showedAreas&Area::GraphLabel)
  {
    p.save();
    p.translate(plotArea.topLeft()-QPointF(0,tsca+tlab));
    QRectF r(QPoint(-llab-lsca, -lab), QPoint(plotArea.width()+rlab+rsca, 0));
    paintMainLabel(p, r, plotArea.width());
    p.restore();
  }
  for(int i=0;i<m_plots.size();i++)
  {
    p.save();
    p.translate(plotArea.topLeft());
    m_plots[i]->setScaleX(scaleX(0));
    m_plots[i]->setScaleY(scaleY(0));
    m_plots[i]->plot(p, plotArea.size());
    p.restore();
  }
  //  QRectF plotArea(wholeArea.marginsRemoved(m_bands));
//  if(m_dirtyX)
//  {
//    auto f=m_scaleX;
//    auto pixelPerMM=p.device()->physicalDpiX()/25.4;
//    auto tickDistance=qMin(qMax(plotArea.width()/3.5,45.), pixelPerMM*20);
//    auto subTickDistance=qMin(qMax(plotArea.width()/20., 15.), pixelPerMM*5.);
//    if(m_policyX==KeepRange)
//      f=HEF::LinearFunction::newFromTwoPoints(0,0,1,plotArea.width())->fOf(f);
//    TickAlgorithmBasic scale(f, 0.,plotArea.width(), &m_ticksX);
//    scale.setMinTickDistance(tickDistance,subTickDistance);
//    scale.recalculate();
//    m_dirtyX=false;
//  }
//  if(m_dirtyY)
//  {
//    auto f=m_scaleY;
//    auto pixelPerMM=p.device()->physicalDpiY()/25.4;
//    auto tickDistance=qMin(qMax(plotArea.height()/3.5,45.), pixelPerMM*20);
//    auto subTickDistance=qMin(qMax(plotArea.height()/20., 15.), pixelPerMM*5.);
//    if(m_policyY==KeepRange)
//      f=HEF::LinearFunction::newFromTwoPoints(0,0,1,plotArea.height())->fOf(f);
//    qDebug()<<plotArea.height()<<tickDistance;
//    TickAlgorithmBasic scale(f, 0.,plotArea.height(), &m_ticksY);
//    scale.setMinTickDistance(tickDistance,subTickDistance);
//    scale.recalculate();
//    m_dirtyY=false;
//  }
//  p.drawRect(plotArea);
//  auto t=p.transform();
//  p.translate(plotArea.bottomLeft());
//  drawTicks(p, m_ticksX);
//  p.setTransform(t);
//  p.translate(plotArea.topLeft());
//  p.rotate(90);
//  drawTicks(p, m_ticksY, plotArea.height(), true);
//  p.setTransform(t);
////  qDebug()<<m_ticksY.mainTicks();

}

void PlotWidgetBase::drawTicks(QPainter &p, const TickList &tickList, const QRectF &r, bool ticksOnTop)
{
  p.setPen(QPen(Qt::black, 1.5));
  QFont f;
  f.setPixelSize(qMin(r.height()*0.6,11.));
  p.setFont(f);
  double tickH=r.height()*0.2;
  for(int i=0;i<tickList.size();i++)
  {
    double x=tickList.y(i);
    p.drawLine(x,ticksOnTop?r.top():r.bottom()-tickH, x, ticksOnTop?r.top()+tickH:r.bottom());
    p.drawText(x-200,r.top()+r.height()*(ticksOnTop?0.3:0.),400, r.height()*0.7, Qt::AlignCenter, tickList.label(i));
  }
  p.setPen(QPen(Qt::black, 1));
  for(int j=0;j<tickList.numSubTicks();j++)
  {
    double x=tickList.subTickY(j);
    p.drawLine(x,ticksOnTop?r.top():r.bottom()-tickH, x, ticksOnTop?r.top()+tickH:r.bottom());
  }
}

void PlotWidgetBase::drawTicks(QPainter &p, const TickList &tickList, int w, bool inverted)
{
  for(int i=0;i<tickList.size();i++)
  {
    double x=tickList.y(i);
    if(inverted)
      x=w-x;
    p.drawLine(x,0, x,10);
    p.drawText(x-100,10, 200, 10, Qt::AlignCenter, tickList.label(i));
  }
  for(int j=0;j<tickList.numSubTicks();j++)
  {
    double x=tickList.subTickY(j);
    if(inverted)
      x=w-x;
    p.drawLine(x,0,x,5);
  }
}

QSize PlotWidgetBase::minimumSizeHint() const
{
  return QSize(0,0);
//  return QSize(m_bands.left()+m_bands.right()+50, m_bands.top()+m_bands.bottom()+50);
}

//PlotWidgetBase::Area PlotWidgetBase::plotArea(QPointF pt)
//{

//}

void PlotWidgetBase::addPlotArea(PlotArea *plot)
{
  if(plot)
  {
    m_plots.append(plot);
  }
}

void PlotWidgetBase::paintRect(QPainter &p, QRectF r, int baseW, const QString &text)
{
  auto pen=p.pen();
  p.setPen(Qt::black);
  p.drawRect(r);
  p.drawLine(0,-5, 0, 5);
  p.setPen(Qt::gray);
  p.drawLine(baseW,-5, baseW, 5);
  p.drawText(r, Qt::AlignCenter, text);
  p.drawRect(QRectF(r.topLeft()+QPoint(2,2),r.topLeft()+QPoint(4,4)));
  p.setPen(pen);
}

void PlotWidgetBase::hideArea(const Areas &area)
{
  m_showedAreas&=~area;
  updateGeometry();
}

void PlotWidgetBase::showArea(const Areas &area)
{
  m_showedAreas|=area;
  updateGeometry();
}

QSize PlotWidgetBase::sizeHint() const
{
  return QSize(800,600);
}
