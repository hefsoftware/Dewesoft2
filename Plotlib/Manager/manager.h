#pragma once

#include <QObject>
#include <QMap>
#include <QSet>
#include <QSizeF>
#include "event.h"
class QPainter;
class QWheelEvent;

namespace Plot
{
  class Band;
  class Area;
  class Item;
  class PlotVisualization;
  class MouseEvent;
  class BandPlacer;
  class Manager: public QObject
  {
    friend class PlotVisualization;
    friend class BandPlacer;
    Q_OBJECT
  public:
    Manager(QObject *parent);
    QSizeF size() const;
    virtual Band *band(int id)=0;
    virtual Area *area(int id)=0;
    virtual QSet<Band *> bands()=0;
    virtual QList<Area *> areas()=0;
    QList<Item *> items();

    virtual void render(QPainter &painter, bool screen=true);
    virtual int addBand(bool horizontal, Plot::Band *band) { Q_UNUSED(horizontal); Q_UNUSED(band); return 0; }
    virtual int addArea(Plot::Area *area) { Q_UNUSED(area); return 0; }
    //public slots:
    void invalidateLayout(); // Invalidate all layout (need to recalculate placing of bands)
    void invalidateData(Plot::Band *band); // Invalidate data for band (and thus does the same to all of its children)
    void update(Plot::Band *band); // Invalidate view for a specific band
    void update(); // Invalidate the view for all bands

    bool isLayoutDirty() { return m_layoutDirty; }
    bool ensureLayoutNotDirty(); // Returns true if a relayout was performed
  protected:
    void setSize(const QSizeF &size);
    void render(QPainter &painter, const QList<Band *> &bands, const QList<Plot::Area *> &areas, bool screen=true);

    //  void invalidateView(); //
  public:
    virtual QSizeF minimumSize()=0;
    virtual QSizeF bestSize()=0;
  protected:
    // Plot visualization interface
    virtual bool dragStart(const MouseEvent &event) { Q_UNUSED(event); return false; }
    virtual void dragEnd(const MouseEvent &event) { Q_UNUSED(event); }
    virtual void mouseClick(const MouseEvent &event) { Q_UNUSED(event); }
    virtual void mouseMove(const MouseEvent &event) { Q_UNUSED(event); }
    virtual void wheelEvent(const QWheelEvent &event) { Q_UNUSED(event); }
    virtual void relayout()=0;
  private:
    static constexpr int m_dragDistance=3;
    void widgetPressEvent(const MouseEvent &event);
    void widgetMoveEvent(const MouseEvent &event);
    void widgetReleaseEvent(const MouseEvent &event);
    void widgetWheelEvent(const QWheelEvent &event) { wheelEvent(event); }

    QMap<Qt::MouseButton, MouseEvent> m_dragStart;
    QMap<Qt::MouseButton, MouseEvent> m_dragActive;
    QSizeF m_size;
    bool m_layoutDirty;

  public:
    // Drag move implementation
    bool startDragMove(const QPointF &point);
    void updateDragMove(const QPointF &point);
    void endDragMove();
    bool isDragMoving();
    QSet<Plot::Band *> dragMovedBands();
    bool addDragMoveBands(const QSet<Plot::Band *> &bands);
    bool addDragMoveBand(Plot::Band *band);
    void removeDragMoveBand(Plot::Band *band);

  public:
    //  QSize sizeHint() const override;
    bool debugView() const;
    void setDebugView(bool debugView);
  private:
    bool m_debugView;
  private slots:
    void onDataDirty();
    void onVisualizationDirty();
  private:
    QSet<Plot::Band *> m_dragMoveBands;
    QPointF m_dragMovePoint;
    bool m_dragMoveActive;
    PlotVisualization *m_widget;
  };
}
