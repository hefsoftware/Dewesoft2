#pragma once
#include "manager.h"
#include "../../Mathlib/function.h"

namespace Plot
{
  class SingleArea : public Manager
  {
  public:
    SingleArea(QObject *parent = nullptr);
    Band *bandAtPosition(const QPointF &pt);
    Band *band(int id) override;
    Area *area(int id) override;
    int addBand(bool horizontal, Plot::Band *band) override { int ret=0; if(band){ if(horizontal)ret=addBandH(band); else ret=addBandV(band); } return ret;}
    int addArea(Plot::Area *area) override;
    QList<Plot::Area *> areas() override  {return m_placedAreas;}
    QSet<Plot::Band *> bands() override {return horizontalBands()+verticalBands(); }
    QSet<Plot::Band *> horizontalBands() { auto b=m_bandsH; return QSet<Plot::Band *>(b.begin(), b.end()); }
    QSet<Plot::Band *> verticalBands() { auto b=m_bandsV; return QSet<Plot::Band *>(b.begin(), b.end());  }
    QSizeF minimumSize() override;
    QSizeF bestSize() override;
  protected:
    void zoom(const QPointF &pt, double angle, QSet<Plot::Band *> bands);
    int addBandH(Plot::Band *band);
    int addBandV(Plot::Band *band);
  private:
    int insertIn(QMap<int, Plot::Band *> &map, Plot::Band *band, int id);
    int insertIn(QMap<int, Plot::Band *> &map, Plot::Band *band);
    int insertIn(QMap<int, Plot::Area *> &map, Plot::Area *area, int id);
    int insertIn(QMap<int, Plot::Area *> &map, Plot::Area *area);
    QMap<int, Plot::Band *> m_bandsH;
    QMap<int, Plot::Band *> m_bandsV;
    QMap<int, Plot::Area *> m_areas;
    QList<Plot::Band *> m_placedH, m_placedV;
    QList<Plot::Area *> m_placedAreas;
  protected:
    int nextItemId();
    Plot::Band *m_bandPlotH, *m_bandPlotV;
    void relayout() override;
    void calculateBands(const QMap<int, Plot::Band *> &bands, double size, QList<Plot::Band *> &output);
    void calculateCorners(int startX, int startY, bool left, bool top);
  };
}
