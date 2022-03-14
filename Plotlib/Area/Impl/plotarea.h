#pragma once
#include "../../Manager/item.h"
#include <QRectF>
namespace Plot
{
  class Manager;
  class AreaPlacer;
  class Area : public Item
  {
    Q_OBJECT
    friend class AreaPlacer;
  public:
    explicit Area();
    virtual QSizeF minSize() { return QSizeF(0,0); }
    double zOrder() const;
    virtual bool setZOrder(double zOrder);
    Band *parentH() const;
    Band *parentV() const;
    Area *parentArea() const;
    virtual bool setParentArea(Area *parent);
    virtual bool setParentH(Band *parentH);
    virtual bool setParentV(Band *parentV);
    QTransform transform() override;
    QRectF rect() { return m_rect; }
    double width() { return m_rect.width(); }
    double height() { return m_rect.height(); }
    QSizeF size() { return m_rect.size(); }
  protected:
    QList<Item *> parents() const override { return {m_parentArea, m_parentH, m_parentV}; }
  private:
    double m_zOrder;
    Item *m_parentH; // Parent band horizontal
    Item *m_parentV; // Parent band vertical
    Item *m_parentArea; // Parent area
    QRectF m_rect;
  };
}
