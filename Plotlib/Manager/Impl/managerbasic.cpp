#include "managerbasic.h"
#include <QWheelEvent>
#include "../../Band/scale.h"
#include "../../Band/label.h"
#include "../../Area/grid.h"
#include "../../Area/Impl/plotarea2d.h"

using namespace Plot;

ManagerBasic::ManagerBasic(QObject *parent): SingleArea(parent)
{
  auto labelH=new BandLabel("Label horizontal",20);
  labelH->setPlaceOrder(-1);
  addBandH(labelH);
  auto scaleH=new BandScaleDefault(HEF::linearTwoPoints(0, 0, 1, 1));
  scaleH->setPlaceOrder(2);
  addBandH(scaleH);

  auto labelV=new BandLabel("Label vertical",20);
  labelV->setPlaceOrder(-2);
  addBandV(labelV);
  auto scaleV=new BandScaleDefault(HEF::linearTwoPoints(0, 0, 1, 1));
  scaleV->setPlaceOrder(-1);
  addBandV(scaleV);

  auto grid=new AreaGrid;
  addArea(grid);
  grid->setParentH(scaleH);
  grid->setParentV(scaleV);

  auto plot=new Area2D;
  addArea(plot);
  plot->setParentH(scaleH);
  plot->setParentV(scaleV);
}

bool ManagerBasic::dragStart(const MouseEvent &event)
{
  bool ret=false;
  if(event.button()==Qt::MiddleButton)
  {
    if((ret=startDragMove(event.pos())))
    {
      auto band=bandAtPosition(event.pos());
      if(band->isPlotArea())
        addDragMoveBands(bands());
      else
        addDragMoveBand(band->rootBand());
    }
  }
  return ret;
}

void ManagerBasic::dragEnd(const MouseEvent &event)
{
  if(event.button()==Qt::MiddleButton)
    endDragMove();
}

void ManagerBasic::mouseClick(const MouseEvent &event)
{
//  if(event.button()==Qt::RightButton)
//  {
//    auto band=bandAtPosition(event.pos());
//    if(band)
//    {
//      QMenu menu;
//      if(band->isPlotArea())
//      {
//        prepareMenu(menu, m_bandPlotV, tr(" horizontal"));
//        prepareMenu(menu, m_bandPlotH, tr(" vertical"));
//      }
//      else
//        prepareMenu(menu, band);
//      auto a=menu.addAction(tr("Debug view"), [this]() { setDebugView(!debugView()); });
//      a->setCheckable(true);
//      a->setChecked(debugView());
//      menu.exec(event.screenPos().toPoint());
//    }
//  }
}

void ManagerBasic::wheelEvent(const QWheelEvent &event)
{
  if(isDragMoving())
  {
    zoom(event.pos(), event.angleDelta().y(), dragMovedBands());
  }
  else
  {
    auto band=bandAtPosition(event.pos());
    if(band->isPlotArea())
    {
      zoom(event.pos(), event.angleDelta().y(), bands());
    }
    else
    {
      zoom(event.pos(), event.angleDelta().y(), {band->rootBand()});
    }
  }
}
