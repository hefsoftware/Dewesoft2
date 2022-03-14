#pragma once

#include <QObject>
#include <QSet>
class QPainter;

namespace Plot
{
  class Manager;
  class ItemPlacer;
  class Band;
  class Item: public QObject
  {
    friend class Manager;
    friend class ItemPlacer;
    Q_OBJECT
  public:
    explicit Item();
    bool visible() const;
    Manager *manager();

    bool isDirty()  { return (m_dirty==DirtyState::DataDirty); }
    bool needsUpdate() { return (m_dirty!=DirtyState::Clean); }

    virtual QTransform transform()=0;
    virtual void paint(QPainter &painter, bool screen) { Q_UNUSED(painter); Q_UNUSED(screen); }
    template <class T> static inline T *findAncestorPostDFS(Item *item, bool childrenOnly) { return findAncestorPostDFS<T>(item, [](const Item *item){ return item->parents();}, childrenOnly); }
    template <class T> static inline T *findAncestorPreDFS(Item *item, bool childrenOnly) { return findAncestorPreDFS<T>(item, [](const Item *item){ return item->parents();}, childrenOnly); }
    template <class T> static inline T *findAncestorPostBFS(Item *item, bool childrenOnly) { return findAncestorPostBFS<T>(item, [](const Item *item){ return item->parents();}, childrenOnly); }
    template <class T> static inline T *findAncestorPreBFS(Item *item, bool childrenOnly) { return findAncestorPreBFS<T>(item, [](const Item *item){ return item->parents();}, childrenOnly); }

    template <class T> static inline T *findAncestorPostDFS(Item *item, const std::function<QList<Item *> (Item *item)> &parents, bool childrenOnly) { return dynamic_cast<T *>(findAncestorPostDFS(item, [] (Item *item) -> bool {return dynamic_cast<T*>(item);}, parents, childrenOnly)); }
    template <class T> static inline T *findAncestorPreDFS(Item *item, const std::function<QList<Item *> (Item *item)> &parents, bool childrenOnly) { return dynamic_cast<T *>(findAncestorPreDFS(item, [] (Item *item) -> bool {return dynamic_cast<T*>(item);}, parents, childrenOnly)); }
    template <class T> static inline T *findAncestorPostBFS(Item *item, const std::function<QList<Item *> (Item *item)> &parents, bool childrenOnly) { return dynamic_cast<T *>(findAncestorPostBFS(item, [] (Item *item) -> bool {return dynamic_cast<T*>(item);} , parents, childrenOnly)); }
    template <class T> static inline T *findAncestorPreBFS(Item *item, const std::function<QList<Item *> (Item *item)> &parents, bool childrenOnly) { return dynamic_cast<T *>(findAncestorPreBFS(item, [] (Item *item) -> bool {return dynamic_cast<T*>(item);}, parents, childrenOnly)); }


    static Item *findAncestorPostDFS(Item *item, const std::function<bool (Item *item)> &test, const std::function<QList<Item *> (Item *item)> &parents, bool childrenOnly);
    static Item *findAncestorPreDFS(Item *item, const std::function<bool (Item *item)> &test, const std::function<QList<Item *> (Item *item)> &parents, bool childrenOnly);
    static Item *findAncestorPostBFS(Item *item, const std::function<bool (Item *)> &test, const std::function<QList<Item *> (Item *)> &parents, bool childrenOnly);
    static Item *findAncestorPreBFS(Item *item, const std::function<bool (Item *item)> &test, const std::function<QList<Item *> (Item *item)> &parents, bool childrenOnly);

  protected:
    bool markUpdate(); // Returns true if band dirty state was changed
    bool markDirty(); // Returns true if band dirty state was changed
    bool markClean(); // Returns true if band dirty state was changed
    bool ensureNotDirty(); // Returns true if band was dirty
    bool setParentPointer(Item *&parentPointer, Item *newParent);

    virtual QList<Item *> parents() const=0;
    virtual void recalculate() { }

    bool hasAncestor(Item *ancestor);

  private:
    QSet<Item *> m_children;
    Manager *m_manager;
    int m_id; // Unique id for the band
    bool m_visible; // Should item be shown
    enum class DirtyState
    {
      DataDirty,
      Update,
      Clean
    };
    DirtyState m_dirty;
  protected:
    virtual void layoutChanged() { }
  signals:
    // Layout was changed
    void layoutDirty();
    void dataDirty();
    void visualizationDirty();
  };
}
