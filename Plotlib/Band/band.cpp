#include "band.h"
#include <cmath>
#include "../Manager/manager.h"
#include <QTransform>
using namespace Plot;

const int Band::MaxPriority=std::numeric_limits<int>::max();






Band::Band() : m_parent(nullptr), m_priority(0), m_placeOrder(1), m_sidePriority(0), m_expand(1.)
{
  m_borderLeft=m_borderRight=m_top=m_height=m_width=false;
  m_shown=false;
  m_isBandH=BandPlacedSide::Unassigned;
}

//Band::Band(Band *parent): Band(parent?parent->manager():nullptr)
//{
//  setParentBand(parent);
//}

bool Band::setPriority(int priority)
{
  m_priority=priority;
  layoutDirty();
  return true;
}

bool Band::setSidePriority(int priority)
{
  m_sidePriority=priority;
  layoutDirty();
  return true;
}

Item::Item(): m_manager(nullptr), m_id(0), m_visible(true)
{
  m_dirty=DirtyState::DataDirty;

}

bool Item::visible() const
{
  return m_visible;
}

Manager *Item::manager()
{
  return m_manager;
}

QRectF Band::fullDrawRect()
{
  return QRectF(-m_borderLeft, 0, m_width+m_borderLeft+m_borderRight, m_height);
}

bool Band::contains(const QPointF &pt)
{
  bool ret=false;
  if(m_isBandH==BandPlacedSide::Horizontal)
    ret=QRectF(m_left-m_borderLeft, m_top, m_width+m_borderRight+m_borderLeft, m_height).contains(pt);
  else if(m_isBandH==BandPlacedSide::Vertical)
    ret=QRectF(m_top, m_left-m_borderLeft, m_height, m_width+m_borderRight+m_borderLeft).contains(pt);
  return ret;
}

bool Band::setParentBand(Band *parent)
{
  bool ret=false;
  if(!parent || (horizontal() && parent->horizontal()) || (vertical() && parent->vertical()))
    ret=setParentPointer(m_parent, parent);
  return ret;
//  bool ret=false;
//  if(!parent)
//  {
//    if(parent)
//      parent->m_children.remove(this);
//    m_parent=nullptr;
//    ret=true;
//  }
//  else if(manager() && parent->manager()==manager())
//  {
//    if(m_isBandH==BandPlacedSide::Unassigned && parent->m_isBandH!=BandPlacedSide::Unassigned)
//    {
//      manager()->addBand(parent->horizontal(), this);
//    }
//    if((horizontal() && parent->horizontal()) || (vertical() && parent->vertical()))
//    {
//      ret=true;
//      for(auto it=parent->parentBand(); it; it=it->parentBand())
//      {
//        if(it==this)
//        {
//          ret=false;
//          break;
//        }
//      }
//      if(ret)
//      {
//        if(parent)
//          parent->m_children.remove(this);
//        m_parent=parent;
//        m_parent->m_children.insert(this);
//      }
//    }
//    if(ret)
//      markDirty();
//  }
//  return ret;
}

bool Band::setParentBand(int id)
{
  bool ret=false;
  if(manager())
    ret=setParentBand(manager()->band(id));
  return ret;
}

QTransform Band::transform()
{
  QTransform ret;
  if(m_isBandH==BandPlacedSide::Horizontal)
    ret=QTransform::fromTranslate(left(), top());
  else if(m_isBandH==BandPlacedSide::Vertical)
    ret=QTransform::fromTranslate(top()+height(),left()).rotate(90);
  return ret;
}



bool Band::setPlaceOrder(double placeOrder)
{
  m_placeOrder = placeOrder;
  emit layoutDirty();
  return true;
}

double Band::setExpand(double expand)
{
  m_expand=fmax(expand,0.001);
  emit layoutDirty();
  return true;
}
