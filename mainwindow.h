#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class QMdiSubWindow;
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void openAcquisition(const QString &filename);
private slots:
  void on_actionScale_test_toggled(bool arg1);

private:
  Ui::MainWindow *ui;
  QMdiSubWindow *m_testScaleSub;
};
#endif // MAINWINDOW_H
