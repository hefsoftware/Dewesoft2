#pragma once
#include "item.h"
namespace Plot
{
  class ItemPlacer
  {
  public:
    constexpr ItemPlacer(Item *item): m_item(item) { }
    constexpr ItemPlacer &setManager(Manager *manager) { if(m_item)m_item->m_manager=manager; return *this;}
    constexpr ItemPlacer &setId(int id) { if(m_item)m_item->m_id=id; return *this;}
    ItemPlacer &assign(Manager *widget, int id);
    ItemPlacer &layoutChanged() { if(m_item)m_item->layoutChanged(); return *this;}
  protected:
    Item *m_item;
  };
}
