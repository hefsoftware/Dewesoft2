#ifndef TESTSCALEWIDGET_H
#define TESTSCALEWIDGET_H

#include <QWidget>
#include <QMap>
#include "Mathlib/function.h"

namespace Ui {
  class TestScaleWidget;
}

class TestScaleWidget : public QWidget
{
  Q_OBJECT

public:
  explicit TestScaleWidget(QWidget *parent = nullptr);
  ~TestScaleWidget();

  void saveResults();
  void loadResults();
protected:
  int setupIndex(const QStringList &list, const QString &result, int start=0);
  void setupRemoveAll(QStringList &list, const QString &result);
  void setupScale();

  QList<double> splitDouble(const QStringList &l);
  QList<double> splitDouble(const QString &str, const QString &sepRegex="[,\\s]+");
  QString currentSetup();
  QString doubleToHex(double val);
  QString toEncodedString(const QList<double> &values, QString sep);
  void setScaleLimits(int left, int right);
  void loadSetup(const QString &setup);
  QList<double> splitEncodedDoubles(const QStringList &l);
  QString toString(const QList<double> &values, QString sep);
private slots:
  void on_borderLeft_valueChanged(int arg1);

  void on_borderRight_valueChanged(int arg1);

  void on_mainTicks_valueChanged(int arg1);

  void on_subTicks_valueChanged(int arg1);

  void on_manualButton_clicked(bool checked);

  void on_buttonOk_clicked();

  void on_buttonFail_clicked();

  void onScaleLayoutChange();
  void on_buttonDelete_clicked();

  void on_okLoad_clicked();

  void on_okUp_clicked();

  void on_okDown_clicked();

  void on_failLoad_clicked();

  void on_failUp_clicked();

  void on_failDown_clicked();

  void on_scaleType_currentIndexChanged(int index);

private:
  QMap<QString, HEF::FunctionPtr> m_functions;
  QStringList m_okResults, m_wrongResults;
  QString m_lastManual;
  QString m_curSetup;
  int m_okIndex, m_wrongIndex;
  Ui::TestScaleWidget *ui;
};

#endif // TESTSCALEWIDGET_H
