#pragma once
#include "band.h"

namespace Plot
{
  class PlotBandPlotArea : public Band
  {
    Q_OBJECT
  public:
    explicit PlotBandPlotArea(): m_minHeight(100), m_maxHeight(qInf()) { setPlaceOrderZero(); setPriority(MaxPriority); }
    double minHeight() override { return m_minHeight; }
    double maxHeight() override { return m_maxHeight; }
    bool isPlotArea() override { return true; }
  protected:
    double m_minHeight;
    double m_maxHeight;
  };

}
