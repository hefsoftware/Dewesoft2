#include "plotwidgetextestimplementation.h"
//#include <Plotlib/plotband.h>
#include <Plotlib/Band/scale.h>
#include "Band/label.h"
#include <QMenu>
#include <QDebug>
#include <QWheelEvent>
PlotWidgetExTestImplementation::PlotWidgetExTestImplementation(QWidget *parent): SingleArea(parent)
{
  auto scale=new Plot::BandScaleDefault(HEF::linearTwoPoints(0, 0, 1, 1));
  addBandH(scale);
  addBandH(new Plot::BandLabel("Graph label", 127));
  addBandH(new Plot::BandLabel("Graph label", 127));
  addBandH(new Plot::BandScaleDefault(HEF::linearTwoPoints(0, 0, 1, 1)));
  addBandV(new Plot::BandLabel("Graph label V", 127));
//  addScaleV(HEF::linearTwoPoints(0, 0, 1, 1));
  addBandV(new Plot::BandScaleDefault(HEF::linearTwoPoints(0, 0, 1, 1)));
  addBandV(new Plot::BandLabel("Graph label V2", 127));
  auto draw=new Plot::BandScaleDraw();
  addBandH(draw);
  draw->setPlaceOrder(-1);
  draw->setParentBand(scale);
}

void PlotWidgetExTestImplementation::prepareMenu(QMenu &menu, Plot::Band *band, const QString &suffix)
{
  // Place order
  auto mPlace=menu.addMenu(tr("Place")+suffix);
  QSet<int> placeSet({-9, -8, -7, -6, -5, -4, -3, -2, -1 , 1, 2, 3, 4, 5, 6, 7, 8, 9, band->placeOrder()});
  auto placeL=placeSet.values();
  std::sort(placeL.begin(), placeL.end());
  for(auto i: placeL)
  {
    auto a=mPlace->addAction(tr("%1").arg(i), [band,i]() { band->setPlaceOrder(i); });
    a->setCheckable(true);
    a->setChecked((i==band->placeOrder()));
  }
  if(!band->isPlotArea())
  {
    // Priority menu
    auto mPriority=menu.addMenu(tr("Priority")+suffix);
    QSet<int> prioSet({0,1,2,3,4,5,6,7,8,9,Plot::Band::MaxPriority,band->priority()});
    auto l=prioSet.values();
    std::sort(l.begin(), l.end());
    for(auto i: l)
    {
      auto a=mPriority->addAction(i==Plot::Band::MaxPriority?tr("Max"):tr("%1").arg(i), [band,i]() { band->setPriority(i); });
      a->setCheckable(true);
      a->setChecked((i==band->priority()));
    }
  }
  // Priority menu
  auto mExpand=menu.addMenu(tr("Expand")+suffix);
  QSet<double> expandSet({0.5, 1, 1.5, 2, 2.5, 3, band->expand()});
  auto expandL=expandSet.values();
  std::sort(expandL.begin(), expandL.end());
  for(auto i: expandL)
  {
    auto a=mExpand->addAction(tr("%1").arg(i), [band,i]() { band->setExpand(i); });
    a->setCheckable(true);
    a->setChecked((i==band->expand()));
  }
  // Horizontal priority menu
  auto mhPriority=menu.addMenu(tr("Side priority")+suffix);
  QSet<int> mhPrioritySet({-9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, band->sidePriority()});
  auto mhPriorityL=mhPrioritySet.values();
  std::sort(mhPriorityL.begin(), mhPriorityL.end());
  for(auto i: mhPriorityL)
  {
    auto a=mhPriority->addAction(tr("%1").arg(i), [band,i]() { band->setSidePriority(i); });
    a->setCheckable(true);
    a->setChecked((i==band->sidePriority()));
  }

}
void PlotWidgetExTestImplementation::mouseClick(const Plot::MouseEvent &event)
{
  if(event.button()==Qt::RightButton)
  {
    auto band=bandAtPosition(event.pos());
    if(band)
    {
      QMenu menu;
      if(band->isPlotArea())
      {
        prepareMenu(menu, m_bandPlotV, tr(" horizontal"));
        prepareMenu(menu, m_bandPlotH, tr(" vertical"));
      }
      else
        prepareMenu(menu, band);
      auto a=menu.addAction(tr("Debug view"), [this]() { setDebugView(!debugView()); });
      a->setCheckable(true);
      a->setChecked(debugView());
      menu.exec(event.screenPos().toPoint());
    }
  }
}

bool PlotWidgetExTestImplementation::dragStart(const Plot::MouseEvent &event)
{
  bool ret=false;
  if(event.button()==Qt::RightButton)
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

void PlotWidgetExTestImplementation::dragEnd(const Plot::MouseEvent &event)
{
  if(event.button()==Qt::RightButton)
  {
    endDragMove();
  }
}

void PlotWidgetExTestImplementation::wheelEvent(const QWheelEvent &event)
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
