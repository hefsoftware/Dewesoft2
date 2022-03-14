#include "manageritem.h"

using namespace Plot;

bool Item::markUpdate()
{
  bool ret=(m_dirty==DirtyState::Clean);
  if(ret)
    m_dirty=DirtyState::Update;
  return ret;
}

//bool Item::markDirty()
//{
//  bool ret=(m_dirty!=DirtyState::DataDirty);
//  if(ret)
//    m_dirty=DirtyState::DataDirty;
//  return ret;
//}


bool Item::markDirty()
{
  bool ret=(m_dirty!=DirtyState::DataDirty);
  if(ret)
  {
    m_dirty=DirtyState::DataDirty;
    for(auto it=m_children.begin();it!=m_children.end();it++)
      (*it)->markDirty();
  }
  return ret;
}

bool Item::markClean()
{
  bool ret=(m_dirty==DirtyState::Update);
  if(ret)
    m_dirty=DirtyState::Clean;
  return ret;
}

//bool Band::markDirty()
//{
//  bool ret=Item::markDirty();
//  if(ret)
//    for(auto it=m_children.begin();it!=m_children.end();it++)
//      (*it)->markDirty();
//  return ret;
//}

bool Item::ensureNotDirty()
{
  bool ret=(m_dirty==DirtyState::DataDirty);
  auto plist=parents();
  for(auto p: plist)
    if(p)
      ret|=p->ensureNotDirty();
  if(ret)
  {
    recalculate();
    m_dirty=DirtyState::Update;
  }
  return ret;
}

bool Item::hasAncestor(Item *ancestor)
{
  bool ret=false;
  if(ancestor==this)
    ret=true;
  else
  {
    auto p=parents();
    for(int i=0; !ret && i<p.size();i++)
      if(p[i])
        ret|=p[i]->hasAncestor(ancestor);
  }
  return ret;
}

bool Item::setParentPointer(Item *&parentPointer, Item *newParent)
{
  bool ret=false;
  if(parentPointer==newParent)
    ret=true; // Parent was not changed
  else if(!newParent)
  {
    parentPointer->m_children.remove(this);
    parentPointer=nullptr;
    ret=true;
  }
  else if(manager() && newParent->manager()==manager())
  {
    ret=!newParent->hasAncestor(this);
    if(ret)
    {
      if(parentPointer)
        parentPointer->m_children.remove(this);
      parentPointer=newParent;
      newParent->m_children.insert(this);
      markDirty();
    }
  }
  return ret;
}
Item *Item::findAncestorPostDFS(Item *item, const std::function<bool (Item *)> &test, const std::function<QList<Item *> (Item *)> &parents, bool childrenOnly)
{
  Item *ret=nullptr;
  if(item)
  {
    QList<Item *> visitList({item});
    QSet<Item *> visited({item});
    while(!visitList.isEmpty())
    {
      auto cur=visitList.takeFirst();
      auto p=parents(cur); // Children
      for(int i=p.size()-1;i>=0;i--)
        if(p[i] && !visited.contains(p[i]))
        {
          visitList.insert(0, p[i]);
          visited.insert(p[i]);
        }
      if(test(cur) && (visitList.size()==1 || !childrenOnly)) // Post visit
      {
        ret=cur;
        break;
      }
    }
  }
  return ret;
}
Item *Item::findAncestorPreDFS(Item *item, const std::function<bool (Item *)> &test, const std::function<QList<Item *> (Item *)> &parents, bool childrenOnly)
{
  Item *ret=nullptr;
  if(item)
  {
    QList<Item *> visitList({item});
    QSet<Item *> visited({item});
    while(!visitList.isEmpty())
    {
      Item *cur=visitList.takeFirst();
      if(test(cur) && (visited.size()==1 || !childrenOnly)) // Pre visit
      {
        ret=cur;
        break;
      }
      auto p=parents(cur);  // Children
      for(int i=p.size()-1;i>=0;i--)
        if(p[i] && !visited.contains(p[i]))
        {
          visitList.insert(0, p[i]);
          visited.insert(p[i]);
        }
    }
  }
  return ret;
}

Item *Item::findAncestorPreBFS(Item *item, const std::function<bool (Item *item)> &test, const std::function<QList<Item *> (Item *item)> &parents, bool childrenOnly)
{
  Item *ret=nullptr;
  if(item)
  {
    QList<Item *> visitList({item}), nextList;
    QSet<Item *> visited({item});
    while(!visitList.isEmpty() || !nextList.isEmpty())
    {
      if(visitList.isEmpty()) {
        visitList=nextList;
        nextList.clear();
      }
      auto cur=visitList.takeFirst();
      if(test(cur) && (visited.size()==1 || !childrenOnly)) // Pre visit
      {
        ret=cur;
        break;
      }
      auto p=parents(cur);  // Children
      for(int i=p.size()-1;i>=0;i--)
        if(p[i] && !visited.contains(p[i]))
        {
          nextList.insert(0, p[i]);
          visited.insert(p[i]);
        }
    }
  }
  return ret;
}

Item *Item::findAncestorPostBFS(Item *item, const std::function<bool (Item *item)> &test, const std::function<QList<Item *> (Item *item)> &parents, bool childrenOnly)
{
  Item *ret=nullptr;
  if(item)
  {
    QList<Item *> visitList({item}), nextList, postVisit;
    QSet<Item *> visited({item});
    while(!visitList.isEmpty() || !nextList.isEmpty())
    {
      if(visitList.isEmpty()) {
        visitList=nextList;
        nextList.clear();
      }
      auto cur=visitList.takeFirst();
      postVisit.append(cur);

      auto p=parents(cur);  // Children
      for(int i=p.size()-1;i>=0;i--)
        if(p[i] && !visited.contains(p[i]))
        {
          nextList.insert(0, p[i]);
          visited.insert(p[i]);
        }
    }
    for(int i=postVisit.size()-1;i>=0;i--)
    {
      auto cur=postVisit[i];
      if(test(cur) && (i>0 || !childrenOnly)) // Pre visit
      {
        ret=cur;
        break;
      }
    }
  }
  return ret;
}
