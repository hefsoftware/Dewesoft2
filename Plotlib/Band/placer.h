#pragma once
#include "../Manager/placer.h"
#include "band.h"
namespace Plot
{
  class BandPlacer: public ItemPlacer
  {
  public:
    constexpr BandPlacer(Band *band): ItemPlacer(band) { }
    constexpr BandPlacer &setShown(bool shown) { if(band())band()->m_shown=shown; return *this;}
    //  constexpr PlotBandPlacer &setBandH() { if(band())band()->m_isBandH=true; return *this;}
    //  constexpr PlotBandPlacer &setBandV() { if(band())band()->m_isBandH=false; return *this;}
    constexpr BandPlacer &setHeight(double height) { if(band())band()->m_height=height; return *this;}
    constexpr BandPlacer &setTop(double top) { if(band())band()->m_top=top; return *this;}
    constexpr BandPlacer &setLeft(double left) { if(band())band()->m_left=left; return *this;}
    constexpr BandPlacer &setWidth(double width) { if(band())band()->m_width=width; return *this;}
    constexpr BandPlacer &setBorderLeft(double borderLeft) { if(band())band()->m_borderLeft=borderLeft; return *this;}
    constexpr BandPlacer &setBorderRight(double borderRight) { if(band())band()->m_borderRight=borderRight; return *this;}
    BandPlacer &assign(Manager *widget, int id, bool horizontal);
  protected:
    constexpr Band *band() {return static_cast<Band *>(m_item); }
  };
}
