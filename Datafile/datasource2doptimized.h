#ifndef DATASOURCE2DOPTIMIZED_H
#define DATASOURCE2DOPTIMIZED_H

#include "datasource2d.h"
namespace HEF
{
  class StatAccumulator;
}
namespace DataFile
{
  class Source2DOptimized : public DataSource2D
  {
  public:
    Source2DOptimized();
    Error getStatFast(qint64 start, qint64 size, double *min, double *max, double *integral, double *squaredIntegral, double *deltaX) override;
    struct CacheData
    {
      double min;
      double max;
      double integral;
      double squaredIntegral;
      void update(double *min, double *max, double *integral, double *squaredIntegral);
      void set(const HEF::StatAccumulator &data);
    };
    QVector<qint64> suggestedCacheSize(qint64 size);
  protected:
    virtual Error getCachedStat(int cacheLevel, qint64 start, qint64 size, double *min, double *max, double *integral, double *squaredIntegral) = 0;
    QVector<qint64> m_cacheSize;
  };
}
#endif // DATASOURCE2DOPTIMIZED_H
