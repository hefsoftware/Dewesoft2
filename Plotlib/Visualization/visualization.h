#pragma once
#include <QSet>
#include <QSizeF>
class QWheelEvent;
namespace Plot
{
  class Band;
  class Manager;
  class MouseEvent;
  class PlotVisualization
  {
    friend class Manager;
  public:
    PlotVisualization(Manager *manager=nullptr);

  protected:
    virtual void layoutChanged() { }; // Called by manager when layout has changed
    virtual void notifyUpdate(QSet<Plot::Band *> band) { Q_UNUSED(band); } // Called by manager when one or more bands needs updating
    void setManagerSize(const QSizeF &size);
    Manager *manager() const;
    void setManager(Manager *manager);

    bool ensureLayoutNotDirty() const;
    bool isLayoutDirty() const;
    void recalculateLayout() const;
  protected:
    void emitPressEvent(const MouseEvent &event);
    void emitMoveEvent(const MouseEvent &event);
    void emitReleaseEvent(const MouseEvent &event);
    void emitWheelEvent(const QWheelEvent &event);
  private:
    Manager *m_manager;
  };
  class PlotVisualizationPublic: public PlotVisualization
  {
  public:
    PlotVisualizationPublic(Manager *manager=nullptr);
    inline Manager *manager() const { return PlotVisualization::manager(); }
    inline void setManager(Manager *manager) { PlotVisualization::setManager(manager); }
  };

}
