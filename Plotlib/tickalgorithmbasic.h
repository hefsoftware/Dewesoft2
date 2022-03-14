#ifndef TICKALGORITHMBASIC_H
#define TICKALGORITHMBASIC_H

#include "../Mathlib/function.h"
#include "ticklist.h"
class TickList;

class TickListInserter
{
public:
  TickListInserter(TickList &list, const HEF::Range &image, const HEF::Range &preimage, HEF::FunctionPtr function): m_list(list), m_image(image), m_preimage(preimage), m_function(function) { }
  bool addMain(double x, int precision);
  bool addMain(double x);
  bool addSub(double x);
  HEF::Range image() const;
  HEF::Range preimage() const;
  HEF::FunctionPtr function() const;

  void clear() { m_list.clear(); }
  TickList &ticks();

protected:
  TickList &m_list;
  HEF::Range m_image;
  HEF::Range m_preimage;
  HEF::FunctionPtr m_function;
};

class TickAlgorithmBasic
{
public:
  TickAlgorithmBasic(const HEF::FunctionPtr &function, double min, double max, TickList *result=nullptr);
  double minMainDistance() const  { return m_minMainDistance;  }
  double minSubDistance() const { return m_minSubDistance; }
  void setMinMainDistance(double minLabelDistance) { m_minMainDistance = minLabelDistance; }
  void setMinSubDistance(double minSubDistance) {  m_minSubDistance = minSubDistance; }
  void setMinTickDistance(double main, double secondary=qQNaN()) { setMinMainDistance(main); setMinSubDistance(secondary); }

  bool recalculate();

  const TickList &ticks() { return m_ticks; }
  const QMap<double, double> &mainTicks() { return m_ticks.mainTicks(); }
  const QMap<double, double> &subTicks() {return m_ticks.subTicks(); }

protected:
  struct LogDivision {
    // Minimum ratio between two consecutive setpoints (that is setPoint[i+1]/setPoint[i])
    double ratioMin;
    // Maximum ratio between two consecutive spans (that is setPoint[i+2]/setPoint[i])
    double ratioMax;
    // Setpoints (Note: first setpoint is always 1, last setpoint is always 10 or one of its powers (100, 1000, etc)
    QList<double> setPoints;
    // The size of this array is 1 less than size of setPoints. allowedDivisions[i] indicates number of division allowed in range setPoint[i] and setPoint[i+1]
    QList<QList<int> > allowedDivisions;
    // Precision that setPoint[i] should have
    QList<int> precision;
  };

  static QList<LogDivision> m_logOptimizedDiv;
  static QList<LogDivision> m_logBackupDiv;

  TickList m_ticks;
  HEF::FunctionPtr m_function;
  TickList *m_result;
  double m_min, m_max, m_delta;
  double m_minMainDistance;
  double m_minSubDistance;

  constexpr double epsilon() { return .2e-5; }
  HEF::Range fullRange();

  // ***** Helper functions
  // Last resort that calculates a single point within the scale
  bool recalculateSinglePoint(TickListInserter &insert);

  // ***** Functions for linear scales ******
  // Check is the function can be considered a linear scale
  double linearConstant(const HEF::Range &preimage);
  double linearConstant(const HEF::Range &preimage, const QList<double> &values);
  bool recalculateLinear(TickListInserter &insert, double k);
  void createTicksLinear(const HEF::Range &preimage, double x0, double y0, double x1);
  void createTicksLinear(const HEF::Range &preimage, double x0, double y0, double x1, int numTicks);
  bool recalculateLinearSubTicks(double preimageRange, int fullTickOrder, int mainDiv, double k, double &ticks, int &numSubTicks, double &subTickDist, int &precision);
  void recalculateLinearSubTicks(int fullTickOrder, int mainDiv, double k, double &ticks, int &numSubTicks, double &subTickDist, int &precision);
  void createLinearTicks(TickListInserter &insert, double ticks, int numSubTick, int precision);

  // ***** Functions for logarithmic scales ******
  double logarithmicConstant(const HEF::Range &preimage);
  double logarithmicConstant(const HEF::Range &preimage, const QList<double> &values);
  bool recalculateLogarithmic(TickListInserter &insert, double k);
  // Divide et impera logarithm ticks (when all integer divisions from 1 to 9 are available
  void recalculateLogarithmicSubDiv(TickListInserter &insert, double k, double min, double max, int numDiv, int ord, int prevDiv=1);
  void addLogTicksByTable(TickListInserter &insert, const LogDivision *row, double k, int order);
  void addLogSubTicks(TickListInserter &insert, double minRatioPerSubTick, double start, double end, const QList<int> &allowedSubs);

  // **** Functions for log ticks lookup tables
  static void initializeLogTables();
  /* Add a row to a log tick lookup table.
     Note: setPoints must start with a value > 1.0 (that will be automatically added to the new row) and terminate with 10 or one of its power (e.g. 100, 1000,...)
     allowedDivisions is the number of divisions allowed from setPoints[i-1] and setPoint[i]
  */
  static void addLogTableRow(QList<LogDivision> &dest, const QList<double> &setPoints, const QList<QList<int> > &allowedDivisions);
  static const LogDivision *findLogTableIndex(const QList<TickAlgorithmBasic::LogDivision> &source, double ratioMin, double ratioMax);
};

#endif // TICKALGORITHMBASIC_H
