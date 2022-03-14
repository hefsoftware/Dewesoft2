#pragma once
#include "../datasource2doptimized.h"
namespace DataFile
{
  class Source2DTestFile: public Source2DOptimized
  {
  public:
    Source2DTestFile(const QString &filename);
    Error size(qint64 &result) override;
    Error dataUncached(qint64 startSample, qint64 numSamples, double *x, double *y) override;
    HEF::Range domain() override { return m_domain; }
//    Error getCachedStat(int cacheLevel, qint64 start, qint64 size, double *min, double *max, double *integral, double *squaredIntegral) override;
  protected:
    QList<QList<CacheData> > m_statCache;
    double *m_x, *m_y;
    qint64 m_size;
    HEF::Range m_domain;
  protected:
    Error getCachedStat(int cacheLevel, qint64 start, qint64 size, double *min, double *max, double *integral, double *squaredIntegral) override;
  };
}
