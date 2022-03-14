#include "plotwidgetbasic.h"
#include <QPainter>
#include <QDebug>
#include "../Mathlib/basicfunctions.h"
#include "tickalgorithmbasic.h"
#include "plotareagrid.h"
#include "plotarea2d.h"
PlotWidgetBasic::PlotWidgetBasic(QWidget *parent): PlotWidgetBase(parent), m_policyX(KeepRange), m_policyY(KeepScale)
{
  m_scaleX=HEF::LinearFunction::newFromTwoPoints(-20,0, 200,1);
  m_scaleY=HEF::LinearFunction::newFromTwoPoints(-16,0, -10,100);
  m_scaleX2=HEF::LogarithmicFunction::newFromTwoPoints(1,0, 100,1);
  m_scaleY2=HEF::LogarithmicFunction::newFromTwoPoints(1,0, 100,1000);
  m_dirtyX=m_dirtyY=m_dirtyX2=m_dirtyY2=true;
  auto grid=new PlotAreaGrid();
  grid->setTicksX(&m_ticksX);
  grid->setTicksY(&m_ticksY);
  addPlotArea(new PlotArea2D);
  addPlotArea(grid);
}


bool PlotWidgetBasic::dragStart(const QMouseEvent &event)
{
  Q_UNUSED(event);
//  plotArea(event.localPos());
  return false;
}

void PlotWidgetBasic::dragEnd(const QMouseEvent &event)
{
  Q_UNUSED(event);
}

void PlotWidgetBasic::mouseClick(const QMouseEvent &event)
{
  Q_UNUSED(event);
}

void PlotWidgetBasic::mouseMove(const QMouseEvent &event)
{
  Q_UNUSED(event);
}

bool PlotWidgetBasic::hasLabel()
{
  return (height()-m_bands.top()-m_bands.bottom()>=plotMinHeight()+mainLabelMinHeight());
}

int PlotWidgetBasic::numVerticalScales()
{
  int nSca=((bool)(m_showedAreas&Area::TopScale))+((bool)(m_showedAreas&Area::BottomScale));
  return (height()-m_bands.top()-m_bands.bottom()>=plotMinHeight()+mainLabelMinHeight()+scaleMinHeight()*nSca)?nSca:0;
}

int PlotWidgetBasic::numVerticalLabels()
{
  int nLab=((bool)(m_showedAreas&Area::TopLabel))+((bool)(m_showedAreas&Area::BottomLabel));
  int nSca=((bool)(m_showedAreas&Area::TopScale))+((bool)(m_showedAreas&Area::BottomScale));
  return (height()-m_bands.top()-m_bands.bottom()>=plotMinHeight()+mainLabelMinHeight()+scaleMinHeight()*nSca+labelMinHeight()*nLab)?nLab:0;
}


int PlotWidgetBasic::numHorizontalLabels()
{
  int nLab=((bool)(m_showedAreas&Area::LeftLabel))+((bool)(m_showedAreas&Area::RightLabel));
  int nSca=((bool)(m_showedAreas&Area::LeftScale))+((bool)(m_showedAreas&Area::RightScale));
  return (width()-m_bands.left()-m_bands.right()>=plotMinWidth()+scaleMinHeight()*nSca+labelMinHeight()*nLab)?nLab:0;
}

int PlotWidgetBasic::numHorizontalScales()
{
  int nSca=((bool)(m_showedAreas&Area::LeftScale))+((bool)(m_showedAreas&Area::RightScale));
  return (width()-m_bands.left()-m_bands.right()>=plotMinWidth()+scaleMinHeight()*nSca)?nSca:0;
}


double PlotWidgetBasic::minRequiredWidth()
{
  return plotMinWidth()+numHorizontalScales()*scaleMinHeight()+numHorizontalLabels()*labelMinHeight();

}

double PlotWidgetBasic::minRequiredHeight()
{
  return plotMinHeight()+hasLabel()*mainLabelMinHeight()+numVerticalScales()*scaleMinHeight()+numVerticalLabels()*labelMinHeight();
}

double PlotWidgetBasic::labelHeight()
{
  return hasLabel()?qMin((height()-m_bands.top()-m_bands.bottom())*mainLabelMinHeight()/minRequiredHeight(), mainLabelMaxHeight()):0;
}

double PlotWidgetBasic::topScaleHeight()
{
  return numVerticalScales()?qMin((height()-m_bands.top()-m_bands.bottom())*scaleMinHeight()/minRequiredHeight(), scaleMaxHeight()):0;
}

double PlotWidgetBasic::bottomScaleHeight()
{
  return numVerticalScales()?qMin((height()-m_bands.top()-m_bands.bottom())*scaleMinHeight()/minRequiredHeight(), scaleMaxHeight()):0;
}

double PlotWidgetBasic::leftLabelHeight()
{
  return numHorizontalLabels()?qMin((width()-m_bands.left()-m_bands.right())*labelMinHeight()/minRequiredWidth(), labelMaxHeight()):0;
}

double PlotWidgetBasic::rightLabelHeight()
{
  return numHorizontalLabels()?qMin((width()-m_bands.left()-m_bands.right())*labelMinHeight()/minRequiredWidth(), labelMaxHeight()):0;
}

double PlotWidgetBasic::topLabelHeight()
{
  return numVerticalLabels()?qMin((height()-m_bands.top()-m_bands.bottom())*labelMinHeight()/minRequiredHeight(), labelMaxHeight()):0;
}

double PlotWidgetBasic::bottomLabelHeight()
{
  return numVerticalLabels()?qMin((height()-m_bands.top()-m_bands.bottom())*labelMinHeight()/minRequiredHeight(), labelMaxHeight()):0;
}

double PlotWidgetBasic::leftScaleHeight()
{
  return numHorizontalScales()?qMin((width()-m_bands.left()-m_bands.right())*scaleMinHeight()/minRequiredWidth(), scaleMaxHeight()):0;
}

double PlotWidgetBasic::rightScaleHeight()
{
  return numHorizontalScales()?qMin((width()-m_bands.left()-m_bands.right())*scaleMinHeight()/minRequiredWidth(), scaleMaxHeight()):0;
}

void PlotWidgetBasic::paintMainLabel(QPainter &p, const QRectF &r, double w)
{
  paintLabel(p, r, w, "Main");
}

void PlotWidgetBasic::paintTopLabel(QPainter &p, const QRectF &r, double w)
{
  paintLabel(p, r, w, "Top");
}

void PlotWidgetBasic::paintBottomLabel(QPainter &p, const QRectF &r, double w)
{
  paintLabel(p, r, w, "Bottom");
}

void PlotWidgetBasic::paintLeftLabel(QPainter &p, const QRectF &r, double w)
{
  paintLabel(p, r, w, "Left");

}

void PlotWidgetBasic::paintRightLabel(QPainter &p, const QRectF &r, double w)
{
  paintLabel(p, r, w, "Right");
}

void PlotWidgetBasic::paintLabel(QPainter &p, const QRectF &r, double w, const QString &text)
{
  if(r.height()>0)
  {
    QFont f;
    f.setPixelSize(r.height()*0.9);
    p.setFont(f);
    p.drawText(r, Qt::AlignCenter, text);
  }
}

void PlotWidgetBasic::paintScale(QPainter &p, const QRectF &r, double w, const HEF::FunctionPtr &f, HEF::FunctionPtr &realFunction, TickList &ticks, bool &dirty, ScaleResizePolicy policy, bool mirrorX, bool ticksOnTop)
{
  if(dirty)
  {
    dirty=false;
    auto function=f;
    auto pixelPerMM=p.device()->physicalDpiX()/25.4;
    auto tickDistance=qMin(qMax(w/3.5,45.), pixelPerMM*20);
    auto subTickDistance=qMin(qMax(w/20., 15.), pixelPerMM*5.);
    if(policy==KeepRange)
      function=HEF::LinearFunction::newFromTwoPoints(0,0,1,w)->fOf(f);
    realFunction=function;
    TickAlgorithmBasic scale(function, 0., w, &ticks);
    scale.setMinTickDistance(tickDistance,subTickDistance);
    scale.recalculate();
  }
  if(r.height()>0)
    drawTicks(p, ticks, r, ticksOnTop);
}


void PlotWidgetBasic::resizeEvent(QResizeEvent *event)
{
  if(event->size().width()!=event->oldSize().width())
    m_dirtyX=m_dirtyX2=true;
  if(event->size().height()!=event->oldSize().height())
    m_dirtyY=m_dirtyY2=true;
}


void PlotWidgetBasic::paintBottomScale(QPainter &p, const QRectF &r, double w)
{
  paintScale(p, r, w, m_scaleX, m_realScaleX, m_ticksX, m_dirtyX, m_policyX, false, true);
}

void PlotWidgetBasic::paintTopScale(QPainter &p, const QRectF &r, double w)
{
  paintScale(p, r, w, m_scaleX2, m_realScaleX2, m_ticksX2, m_dirtyX2, m_policyX, false, false);
}

void PlotWidgetBasic::paintLeftScale(QPainter &p, const QRectF &r, double w)
{
  paintScale(p, r, w, m_scaleY, m_realScaleY, m_ticksY, m_dirtyY, m_policyY, true, true);
}

void PlotWidgetBasic::paintRightScale(QPainter &p, const QRectF &r, double w)
{
  paintScale(p, r, w, m_scaleY2, m_realScaleY2, m_ticksY2, m_dirtyY2, m_policyY, true, false);
}


HEF::FunctionPtr PlotWidgetBasic::scaleX(int i)
{
  HEF::FunctionPtr ret;
  switch(i)
  {
    case 0:
      ret=m_realScaleX; break;
    case 1:
      ret=m_realScaleX2; break;
  }
  return ret;
}

HEF::FunctionPtr PlotWidgetBasic::scaleY(int i)
{
  HEF::FunctionPtr ret;
  switch(i)
  {
    case 0:
      ret=m_realScaleY; break;
    case 1:
      ret=m_realScaleY2; break;
  }
  return ret;
}
