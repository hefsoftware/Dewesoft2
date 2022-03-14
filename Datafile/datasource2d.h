#ifndef DATASOURCE2D_H
#define DATASOURCE2D_H

#include <QObject>
#include "../Mathlib/range.h"
#include <QEnableSharedFromThis>
#include "error.h"
namespace HEF
{
  class Range;
}
namespace DataFile
{
  class DataSource2D : public QObject, public QEnableSharedFromThis<DataSource2D>
  {
    Q_OBJECT
  public:
    enum class Interpolate
    {
      Step,
      Middle,
      Linear,
      Hermite
    }; // Interpolation method
    explicit DataSource2D(QObject *parent = nullptr);

    inline QSharedPointer<DataSource2D> sharedPtr() { QSharedPointer<DataSource2D> ret; ret=sharedFromThis().staticCast<DataSource2D>(); if(!ret)ret=QSharedPointer<DataSource2D>(this); return ret; }

    virtual Error size(qint64 &result)=0;
    inline qint64 size() { qint64 result; return size(result)==Error::Success?result:-1; }

    Error data(qint64 startSample, qint64 numSamples, double *x, double *y);
    Error dataX(qint64 startSample, qint64 numSamples, double *x);
    Error dataY(qint64 startSample, qint64 numSamples, double *y);
    inline Error data(qint64 index, double &x, double &y) { return data(index, 1, &x, &y); }
    inline Error dataX(qint64 index, double &x) { Error ret=dataX(index, 1, &x); if(ret!=Error::Success) x=qQNaN(); return ret; }
    inline Error dataY(qint64 index, double &y) { Error ret=dataY(index, 1, &y); if(ret!=Error::Success) y=qQNaN(); return ret; }

    // Read data from cache. These functions are guaranteed to be called with a valid range
    virtual Error dataCached(qint64 startSample, qint64 numSamples, double *x, double *y);
    virtual Error dataCachedX(qint64 startSample, qint64 numSamples, double *x);
    virtual Error dataCachedY(qint64 startSample, qint64 numSamples, double *y);

    inline Error dataCached(qint64 index, double &x, double &y)  { return dataCached(index, 1, &x, &y); }
    inline Error dataCachedX(qint64 index, double &x)  { return dataCachedX(index, 1, &x); }
    inline Error dataCachedY(qint64 index, double &y) { return dataCachedY(index, 1, &y); }

    inline Error x(qint64 index, double &x) { return dataX(index, x); }
    inline Error y(qint64 index, double &y) { return dataY(index, y); }

    inline double x(qint64 index) { double ret; x(index,ret); return ret; }
    inline double y(qint64 index) { double ret; y(index, ret); return ret; }

//    Error min(double xMin, double xMax, double &result);
//    Error max(double xMin, double xMax, double &result);
//    Error avg(double xMin, double xMax, double &result);

//    Error yStep(double xValue, double &yResult, double *integralBefore=nullptr, double *integralAfter=nullptr);
//    Error yStepMiddle(double xValue, double &yResult, double *integralBefore=nullptr, double *integralAfter=nullptr);
//    Error yLinear(double xValue, double &yResult, double *integralBefore=nullptr, double *integralAfter=nullptr);
//    Error yHermite(double xValue, double &yResult, double *integralBefore=nullptr, double *integralAfter=nullptr);

//    inline double yStep(double xValue) {double ret; yStep(xValue, ret); return ret;}
//    inline double yStepMiddle(double xValue) {double ret; yStepMiddle(xValue, ret); return ret;}
//    inline double yLinear(double xValue) {double ret; yLinear(xValue, ret); return ret;}
//    inline double yHermite(double xValue) {double ret; yHermite(xValue, ret); return ret;}

    Error yStepFast(qint64 index, double xValue, double &yResult);
    Error yStepMiddleFast(qint64 index, double xValue, double &yResult);
    Error yLinearFast(qint64 index, double xValue, double &yResult);
    Error yHermiteFast(qint64 index, double xValue, double &yResult);
    Error yInterpolateFast(qint64 index, double xValue, double &yResult);

    Error yStepFast(qint64 index, double xValue, double &yResult, double *integralBefore, double *integralAfter, double *squaredIntegralBefore=nullptr, double *squaredIntegralAfter=nullptr, double *xBefore=nullptr, double *xAfter=nullptr);
    Error yStepMiddleFast(qint64 index, double xValue, double &yResult, double *integralBefore, double *integralAfter, double *squaredIntegralBefore=nullptr, double *squaredIntegralAfter=nullptr, double *xBefore=nullptr, double *xAfter=nullptr);
    Error yLinearFast(qint64 index, double xValue, double &yResult, double *integralBefore, double *integralAfter, double *squaredIntegralBefore=nullptr, double *squaredIntegralAfter=nullptr, double *xBefore=nullptr, double *xAfter=nullptr);
    Error yHermiteFast(qint64 index, double xValue, double &yResult, double *integralBefore, double *integralAfter, double *squaredIntegralBefore=nullptr, double *squaredIntegralAfter=nullptr, double *xBefore=nullptr, double *xAfter=nullptr);
    Error yInterpolateFast(qint64 index, double xValue, double &yResult, double *integralBefore, double *integralAfter, double *squaredIntegralBefore=nullptr, double *squaredIntegralAfter=nullptr, double *xBefore=nullptr, double *xAfter=nullptr);

//    QList<double> avg;
    Error getStat(const QList<HEF::Range> &ranges, double *min=nullptr, double *max=nullptr, double *avg=nullptr, double *rms=nullptr, double *delta=nullptr, double *integral=nullptr, double *integralSquared=nullptr);

    // Returns an index i such that xValues[i]*percent+xValues[i+1]*(1-percent)=x
    // Note: if size==0 or if x<minimum x => index=0, percent=-1
    // if x==maximum x => index=size-1 percent=0
    // if x>maximum size => index=size-1 percent=2
    Error index(double x, qint64 &index, double &percent);

    Error indexLE(double x, qint64 &index);
    // Returns the index of element whose x value is >= x
    Error indexGE(double x, qint64 &index);
    inline qint64 indexLE(double x) {qint64 index; return (indexLE(x,index)==Error::Success)?index:-2; }
    inline qint64 indexGE(double x) {qint64 index; return (indexGE(x,index)==Error::Success)?index:-2; }
    // Returns a frequency (1/dx) for a range of samples or qNan if channels are asyncronous.
    // Sync channels are those for which x(index+k)=x(index)+syncronous(index, num)*k for k=0..num-1
    Error syncronous(qint64 index, qint64 count, double &frequency);

//    // Statistic functions. Can be reimplemented if a faster calulation is available. Fast functions (do not need to check for index limits)
//    // Calculate the minimum in a range of samples
//    virtual bool minFast(qint64 startSample, qint64 numSamples);
//    // Calculate the maximum in a range of samples
//    virtual bool maxFast(qint64 startSample, qint64 numSamples);
//    // Calculate the average of a range of samples
    //    virtual bool avgFast(qint64 startSample, qint64 numSamples);

    // Note: the function should update the values (e.g. add to existing integral and max/min with current max/min values)
    Error getStat(const QList<HEF::Range> &ranges, QVector<double> *min, QVector<double> *max=nullptr, QVector<double> *avg=nullptr, QVector<double> *rms=nullptr, QVector<double> *delta=nullptr, QVector<double> *integral=nullptr, QVector<double> *integralSquared=nullptr);
    virtual Error getStatFast(qint64 start, qint64 size, double *min, double *max=nullptr, double *integral=nullptr, double *squaredIntegral=nullptr, double *xDelta=nullptr);
    virtual HEF::Range domain();

    virtual Error indexLEFast(double xValue, qint64 &index, qint64 low, qint64 high);
    virtual Error indexGEFast(double xValue, qint64 &index, qint64 low, qint64 high);
    // Returns a frequency (1/dx) for a range of samples or qNan if channels are asyncronous.
    // Sync channels are those for which x(index+k)=x(index)+syncronous(index, num)*k for k=0..num-1
    // Interval passed is guaranteed to be valid
    virtual Error syncronousFast(qint64 index, qint64 count, double &freq) { Q_UNUSED(index); Q_UNUSED(count); freq=qQNaN(); return Error::Success; }
  public:
    Error allocateBuffer(qint64 size);
    double *m_buffer;
    int64_t m_bufferSize;
    // Linearization caching
    enum class InterpolationCacheType
    {
      NoCache,
      Linear,
      Hermite
    };
    InterpolationCacheType m_cacheType;
    qint64 m_cacheIndex;
    double m_x0, m_y0;
    double m_x1, m_y1;
    double m_c1, m_c2, m_c3; // Coefficents or extra parameters for cache
    Interpolate m_interpolate;
  protected:
    // Fast version. Does not perform any checks (x and y must be allocated and sample range must be valid)
    virtual Error dataUncached(qint64 startSample, qint64 numSamples, double *x, double *y)=0;
    virtual Error dataUncachedX(qint64 startSample, qint64 numSamples, double *x);
    virtual Error dataUncachedY(qint64 startSample, qint64 numSamples, double *y);
  private:
    // Note: following function must be called only after the right index for x has been successfully prepared
    double internalYMiddle(double x);
    double internalYStep(double x);
    double internalYLinear(double x);
    double internalYHermite(double x);

    double internalYMiddle(double x, double *integralBefore, double *integralAfter, double *squaredIntegralBefore=nullptr, double *squaredIntegralAfter=nullptr, double *xBefore=nullptr, double *xAfter=nullptr);
    double internalYStep(double x, double *integralBefore, double *integralAfter, double *squaredIntegralBefore=nullptr, double *squaredIntegralAfter=nullptr, double *xBefore=nullptr, double *xAfter=nullptr);
    double internalYLinear(double x, double *integralBefore, double *integralAfter, double *squaredIntegralBefore=nullptr, double *squaredIntegralAfter=nullptr, double *xBefore=nullptr, double *xAfter=nullptr);
    double internalYHermite(double x, double *integralBefore, double *integralAfter, double *squaredIntegralBefore=nullptr, double *squaredIntegralAfter=nullptr, double *xBefore=nullptr, double *xAfter=nullptr);

    Error interpolationPrepare(double xValue, double &yResult, qint64 &index);
    Error precalculateLinearInterpolation(qint64 index);
    Error precalculateHermiteInterpolation(qint64 index);
  };
  class DataSourceTest: public DataSource2D
  {
    // DataSource2D interface
  public:
    Error size(qint64 &result) override;
    Error dataUncached(qint64 startSample, qint64 numSamples, double *x, double *y) override;
  };
  class DataSourceTest2: public DataSource2D
  {
    // DataSource2D interface
  public:
    DataSourceTest2(const QList<double> &x, const QList<double> &y);
    Error size(qint64 &result) override;
    Error dataUncached(qint64 startSample, qint64 numSamples, double *x, double *y) override;
  protected:
    QList<double> m_x, m_y;
  };
}

#endif // DATASOURCE2D_H
