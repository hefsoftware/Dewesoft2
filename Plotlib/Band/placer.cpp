#include "placer.h"
#include "../Manager/manager.h"
using namespace Plot;
BandPlacer &BandPlacer::assign(Manager *widget, int id, bool horizontal)
{
  if(band() && widget)
  {
    setManager(widget);
    setId(id);
//    band()->m_id=id;
    band()->m_isBandH=horizontal?Band::BandPlacedSide::Horizontal:Band::BandPlacedSide::Vertical;
    QObject::connect(band(), &Band::dataDirty, widget, &Manager::onDataDirty);
    QObject::connect(band(), &Band::visualizationDirty, widget, &Manager::onVisualizationDirty);
  }
  return *this;
}

ItemPlacer &ItemPlacer::assign(Manager *widget, int id)
{
  if(widget)
  {
    setManager(widget);
    setId(id);
  }
  return *this;
}
