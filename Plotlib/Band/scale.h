#pragma once
#include "band.h"
#include "../../Mathlib/function.h"
#include "../ticklist.h"
#include "../scrollmanager.h"
namespace Plot
{
  class BandScale
  {
  public:
    explicit BandScale() { }
    virtual HEF::FunctionPtr function()=0;
  };

  class BandTicks
  {
  public:
    explicit BandTicks() { }
    virtual TickList ticks()=0;
  };

  class BandScaleDraw: public Band
  {
    Q_OBJECT
  public:
    BandScaleDraw(Band *parent=nullptr): Band(parent) { m_drawLabel=false; m_drawTicks=true; }
    double minHeight() override { return m_drawTicks*3+m_drawLabel*9; }
    double maxHeight() override { return m_drawTicks*6+m_drawLabel*18; }
    void paint(QPainter &p, bool screen) override;
    bool drawTicks() const;
    void setDrawTicks(bool drawTicks);

    bool drawLabel() const;
    void setDrawLabel(bool drawLabel);

  protected:
    bool m_drawTicks;
    bool m_drawLabel;
  };

  class BandScaleDefault : public Band, public BandScale, public BandTicks
  {
    Q_OBJECT
  public:
    explicit BandScaleDefault(HEF::FunctionPtr function): m_function(function), m_dirty(true)
    {
      m_scroll.setNotifyFunc([this](){ m_dirty=true; emit dataDirty(); });
    }
    double minHeight() override { return 12; }
    double maxHeight() override { return 25; }
    // PlotBandScale interface
    HEF::FunctionPtr function() override { checkNotDirty(); return m_scrolledFunction; }
    TickList ticks() override { checkNotDirty(); return m_ticks; }
    // PlotBand interface
    void paint(QPainter &p, bool screen) override;
  protected:
    void checkNotDirty();
    void layoutChanged() override { m_dirty=true; }
    HEF::FunctionPtr m_function;
    HEF::FunctionPtr m_scrolledFunction;
    bool m_dirty;
    TickList m_ticks;
    ScrollManager m_scroll;
    // PlotBand interface
  protected:
    bool startDragMove(const QPointF &pt) override;
    bool updateDragMove(const QPointF &pt) override;
    void endDragMove() override;

    // Band interface
  public:
    bool zoom(const QPointF &pt, double angle) override;
  };
}
