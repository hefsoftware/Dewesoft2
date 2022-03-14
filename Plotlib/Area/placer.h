#pragma once
#include "../Manager/placer.h"
#include "area.h"
namespace Plot
{
  class AreaPlacer: public ItemPlacer
  {
  public:
    constexpr AreaPlacer(Area *area): ItemPlacer(area) { }
    constexpr AreaPlacer &setRect(const QRectF &rect) { if(area())area()->m_rect=rect; return *this;}

  protected:
    constexpr Area *area() {return static_cast<Area *>(m_item); }
  };
}
