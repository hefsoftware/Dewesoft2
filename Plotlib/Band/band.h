#pragma once

#include <QObject>
#include <QSet>
#include "../Manager/item.h"
class QPainter;

namespace Plot
{
  class Manager;
  class ItemPlacer;
  class BandPlacer;
  class Band;


  class Band : public Item
  {
    friend class Manager;
    Q_OBJECT
  public:
    explicit Band();
    explicit Band(Band *parent);
    virtual double minHeight()=0;
    virtual double maxHeight()=0;

    virtual double setExpand(double expand);
    double expand() const { return m_expand; }

    int priority() const { return m_priority; }
    virtual bool setPriority(int priority);
    static const int MaxPriority;

    int sidePriority() const { return m_sidePriority; }
    virtual bool setSidePriority(int priority);

    double placeOrder() const { return m_placeOrder; }
    virtual bool setPlaceOrder(double placeOrder);


    virtual bool isPlotArea() { return false; }
    constexpr double height() const { return m_height; }
    constexpr double top() const { return m_top; }
    constexpr double left() const { return m_left; }
    constexpr double width() const { return m_width; }
    constexpr double borderLeft() const { return m_borderLeft; }
    constexpr double borderRight() const { return m_borderRight; }
    constexpr bool isShown() const { return m_shown; }
    QRectF fullDrawRect();

    bool contains(const QPointF &pt);

    Band *rootBand() { return findAncestorPostDFS<Band>(this, false); }
    Band *parentBand() { return dynamic_cast<Band *>(m_parent); }
    virtual bool setParentBand(Band *parent);
    bool setParentBand(int id);

//    Manager *manager();
    constexpr bool horizontal() { return m_isBandH==BandPlacedSide::Horizontal; }
    constexpr bool vertical() { return m_isBandH==BandPlacedSide::Vertical; }
    QTransform transform() final;

    virtual bool startDragMove(const QPointF &pt){ Q_UNUSED(pt); return false; }
    virtual bool updateDragMove(const QPointF &pt) { Q_UNUSED(pt); return true; }
    virtual void endDragMove() { }

    virtual bool zoom(const QPointF &pt, double angle){ Q_UNUSED(pt); Q_UNUSED(angle); return false; }
  protected:
    void setPlaceOrderZero() { m_placeOrder=0; }

    QList<Item *> parents() const override { return {m_parent}; }

//    void markClean();
//  protected:
  private:
    Item *m_parent; // Parent band (example: the scale to be used for a tick/grid band)
    // Properties assigned by the layout object
    enum class BandPlacedSide
    {
      Unassigned,
      Horizontal,
      Vertical
    };

    BandPlacedSide m_isBandH; // Is horizontal scale/band
    // Placement properties
    int m_priority; // Priority in showing/hiding the item. The higher the latest the item will be hidden when shrinking the widget. A value of MaxPriority means it can't be hidden.
    double m_placeOrder; // Sorting order (increasing numbers means left to right, top to bottom)
    int m_sidePriority; // Priority in utiling the space on the left/right. Corners will be managed throught this property.
    double m_expand; // Expand factor

    // Calculated ("cached") values after doing layout
    friend class BandPlacer;
    bool m_shown;
    double m_height;
    double m_top;
    double m_left;
    double m_width;
    double m_borderLeft;
    double m_borderRight;
  };
}
