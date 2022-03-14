#pragma once

#include "band.h"

namespace Plot
{
  class BandLabel : public Band
  {
    Q_OBJECT
  public:
    explicit BandLabel(const QString &label, double maxHeight): m_label(label), m_maxHeight(maxHeight)  { setPriority(3); setExpand(25/m_maxHeight); }
    double minHeight() override { return 12; }
    double maxHeight() override { return m_maxHeight; }

  protected:
    QString m_label;
    double m_maxHeight;
  };
}
