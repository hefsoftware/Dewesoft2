#include "source2dtestfile.h"
#include "../../Mathlib/sum.h"
#include <QFile>
#include <QDebug>
using namespace DataFile;

Source2DTestFile::Source2DTestFile(const QString &filename)
{
  m_x=m_y=nullptr;
  m_size=0;
  QFile f(filename);
  if(f.open(QIODevice::ReadOnly))
  {
    m_size=f.size()/(2*sizeof(double));
    m_x=new double[m_size];
    m_y=new double[m_size];
    m_cacheSize=suggestedCacheSize(m_size);
    for(int j=0;j<m_cacheSize.size();j++)
      m_statCache.append(QList<CacheData>());
    HEF::StatAccumulator stats[m_cacheSize.size()];
    double lastX=qQNaN();
    double firstX=qQNaN();
    int nX=0;
    for(qint64 j=0;j<m_size;j++)
    {
      f.read((char *)(&m_x[j]), sizeof(double));
      f.read((char *)(&m_y[j]), sizeof(double));
      for(int i=0;i<m_cacheSize.size();i++)
      {
        stats[i].feed(m_x[j], m_y[j]);
        if(!((j+1) % m_cacheSize[i]))
        {
          CacheData cd;
          cd.set(stats[i]);
          m_statCache[i].append(cd);
          stats[i].clear();
        }
      }
      double curX=m_x[j];
      if(nX>0)
      {
        if(curX-lastX>(lastX-firstX)*2/nX)
        {
          m_domain=m_domain|HEF::Range(firstX, lastX);
          firstX=curX;
          lastX=qQNaN();
          nX=0;
        }
        else
        {
          lastX=curX;
          nX++;
        }
      }
      else if(!qIsNaN(firstX))
      {
        lastX=curX;
        nX=1;
      }
      else
        firstX=curX;
    }
    qDebug()<<"Finished reading"<<m_size;
  }
  else
    qDebug()<<"Failed to read"<<filename;
}

Error Source2DTestFile::size(qint64 &result)
{
  result=m_size;
  return Error::Success;
}

Error Source2DTestFile::dataUncached(qint64 startSample, qint64 numSamples, double *x, double *y)
{
  memcpy(x, &m_x[startSample], sizeof(double)*numSamples);
  memcpy(y, &m_y[startSample], sizeof(double)*numSamples);
  return Error::Success;
}


Error Source2DTestFile::getCachedStat(int cacheLevel, qint64 start, qint64 size, double *min, double *max, double *integral, double *squaredIntegral)
{
  for(qint64 i=0;i<size;i++)
    m_statCache[cacheLevel][start+i].update(min, max, integral, squaredIntegral);
  return Error::Success;
}
