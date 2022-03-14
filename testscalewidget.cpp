#include "testscalewidget.h"
#include "ui_testscalewidget.h"
#include <QStyle>
#include <QInputDialog>
#include <QDebug>
#include <QMdiSubWindow>
#include <QDir>
TestScaleWidget::TestScaleWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::TestScaleWidget)
{
  ui->setupUi(this);
  ui->buttonOk->setIcon(ui->buttonOk->style()->standardIcon(QStyle::SP_DialogApplyButton));
  ui->buttonFail->setIcon(ui->buttonOk->style()->standardIcon(QStyle::SP_DialogCancelButton));
  ui->buttonDelete->setIcon(ui->buttonDelete->style()->standardIcon(QStyle::SP_TrashIcon));
//  ui->buttonDelete->setVisible(false);
  ui->borderLeft->setValue(ui->scale->leftMargin());
  ui->borderLeft->setValue(ui->scale->rightMargin());
  m_functions.insert("Linear", HEF::linearTwoPoints(0, 0, 1000, 1000));
  m_functions.insert("Logarithmic", HEF::logarithmicTwoPoints(1, 0, 10, 100));
  m_functions.insert("Inverse linear", HEF::linearTwoPoints(0, 0, -1000, 1000));
  for(auto it=m_functions.begin();it!=m_functions.end();it++)
    ui->scaleType->addItem(it.key(),it.key());
  ui->scaleType->setCurrentText("Logarithmic");
  connect(ui->scale, &TestScaleWidget2::changedLayout, this, &TestScaleWidget::onScaleLayoutChange);
  m_okIndex=m_wrongIndex=0;
//  m_okResults.append("Linear\t-207.412/08d462f030ed69c0\t2.98598/bb0853944be30740\t32/0000000000004040\t651/0000000000588440\t45/0000000000804640\t12/0000000000002840\t100/0000000000005940;200/0000000000006940\t85/0000000000405540;90/0000000000805640;95/0000000000c05740;105/0000000000405a40;110/0000000000805b40;115/0000000000c05c40;120/0000000000005e40;125/0000000000405f40;130/0000000000406040;135/0000000000e06040;140/0000000000806140;145/0000000000206240;150/0000000000c06240;155/0000000000606340;160/0000000000006440;165/0000000000a06440;170/0000000000406540;175/0000000000e06540;180/0000000000806640;185/0000000000206740;190/0000000000c06740;195/0000000000606840;205/0000000000a06940;210/0000000000406a40;215/0000000000e06a40;220/0000000000806b40;225/0000000000206c40;230/0000000000c06c40;235/0000000000606d40;240/0000000000006e40;245/0000000000a06e40;250/0000000000406f40;255/0000000000e06f40;260/0000000000407040;265/0000000000907040;270/0000000000e07040;275/0000000000307140;280/0000000000807140;285/0000000000d07140");
  loadResults();
  setupScale();
}

TestScaleWidget::~TestScaleWidget()
{
  delete ui;
}

void TestScaleWidget::setupScale()
{
  ui->scale->setLeftMargin(ui->borderLeft->value());
  ui->scale->setRightMargin(ui->borderRight->value());
  ui->scale->setMainTickDist(ui->mainTicks->value());
  ui->scale->setSubTickDist(ui->subTicks->value());
  HEF::FunctionPtr f;
  f=m_functions.value(ui->scaleType->currentData().toString());
  ui->scale->setFunction(f);
}


void TestScaleWidget::loadResults()
{
  m_okResults.clear();
  m_wrongResults.clear();
  auto fn=QDir(QCoreApplication::applicationDirPath()).relativeFilePath("scaleResults.txt");
  QFile f(fn);
  bool good=true;
  if(f.open(QFile::Text|QFile::ReadOnly))
  {
    while(true)
    {
      auto d=f.readLine();
      if(d.isEmpty())
        break;
      auto s=QString::fromUtf8(d);
      if(s.right(1)=="\n")
        s=s.left(s.size()-1);
      if(s.startsWith("****"))
      {
        if(s.contains("GOOD"))
          good=true;
        else if(s.contains("FAIL"))
          good=false;
        continue;
      }
      if(good)
        m_okResults.append(s);
      else
        m_wrongResults.append(s);
    }
  }
}

int TestScaleWidget::setupIndex(const QStringList &list, const QString &result, int start)
{
  int ret=-1;
  if(start<0)
    start=0;
  QString resultKey=result;
  int i=resultKey.indexOf("#");
  if(i>=0)
  {
    resultKey=resultKey.left(i);
    for(;start<list.size();start++)
    {
      if(list[start].startsWith(resultKey))
      {
        ret=start;
        break;
      }
    }
  }
  return ret;
}

void TestScaleWidget::setupRemoveAll(QStringList &list, const QString &result)
{
  for(int i=setupIndex(list,result); i>=0; )
  {
    list.removeAt(i);
    i=setupIndex(list,result,i);
  }
}

void TestScaleWidget::saveResults()
{
  auto fn=QDir(QCoreApplication::applicationDirPath()).relativeFilePath("scaleResults.txt");
  QFile f(fn);
  if(f.open(QFile::Truncate|QFile::Text|QFile::WriteOnly))
  {
    f.write(QString("******** GOOD ********\n").toUtf8());
    for(int i=0;i<m_okResults.size();i++)
      f.write((m_okResults[i]+"\n").toUtf8());
    f.write(QString("******** FAILED ********\n").toUtf8());
    for(int i=0;i<m_wrongResults.size();i++)
      f.write((m_wrongResults[i]+"\n").toUtf8());
  }
}

void TestScaleWidget::on_borderLeft_valueChanged(int arg1)
{
  Q_UNUSED(arg1);
  setupScale();
}

void TestScaleWidget::on_borderRight_valueChanged(int arg1)
{
  Q_UNUSED(arg1);
  setupScale();
}

void TestScaleWidget::on_mainTicks_valueChanged(int arg1)
{
  Q_UNUSED(arg1);
  setupScale();
}

void TestScaleWidget::on_subTicks_valueChanged(int arg1)
{
  Q_UNUSED(arg1);
  setupScale();
}

QList<double> TestScaleWidget::splitDouble(const QStringList &l)
{
  QList<double> ret;
  for(int i=0;i<l.size();i++)
  {
    bool ok;
    double val=l[i].toDouble(&ok);
    if(ok)
      ret.append(val);
  }
  return ret;
}

QList<double> TestScaleWidget::splitEncodedDoubles(const QStringList &l)
{
  QList<double> ret;
  for(int i=0;i<l.size();i++)
  {
    bool ok;
    double val;
    QString v=l[i];
    if(v.contains("/"))
      v=v.mid(v.indexOf("/")+1);
    v=v.trimmed();
    if(v.size()==sizeof(double)*2 && QRegExp("[0-9A-Fa-f]+").exactMatch(v))
    {
      auto ba=QByteArray::fromHex(v.toUtf8());
      memcpy(&val, ba.data(), sizeof(double));
      ok=true;
    }
    else
      val=v.toDouble(&ok);
    if(ok)
      ret.append(val);
  }
  return ret;
}

QList<double> TestScaleWidget::splitDouble(const QString &str, const QString &sepRegex)
{
  return splitDouble(str.split(QRegExp(sepRegex), QString::SkipEmptyParts));
}

QString TestScaleWidget::doubleToHex(double val)
{
  uint8_t data[sizeof(val)];
  memcpy(data, &val, sizeof(val));
  return QString("%1/%2").arg(val).arg(QString::fromUtf8(QByteArray((char *)data, 8).toHex()));
}
QString TestScaleWidget::toString(const QList<double> &values, QString sep)
{
  QStringList ret;
  for(int i=0;i<values.size();i++)
    ret.append(QString::number(values[i]));
  return ret.join(sep);
}
QString TestScaleWidget::toEncodedString(const QList<double> &values, QString sep)
{
  QStringList ret;
  for(int i=0;i<values.size();i++)
    ret.append(doubleToHex(values[i]));
  return ret.join(sep);
}
QString TestScaleWidget::currentSetup()
{
  QString ret;
  ret=ui->scaleType->currentData().toString()+"\t";
  ret+=toEncodedString(
    QList<double>({
      ui->scale->offset(),
      ui->scale->scale(),
      (double)ui->scale->leftPosition(),
      (double)ui->scale->rightPosition(),
      ui->scale->mainTickDist(),
      ui->scale->subTickDist()
    }),"\t");
  ret+="#";
  ret+=toEncodedString(ui->scale->mainTicks().keys(), ";")+"\t"+toEncodedString(ui->scale->subTicks().keys(), ";");
  return ret;
}

void TestScaleWidget::loadSetup(const QString &setup)
{
  //  qDebug()<<"Loading setup"<<setup;
  auto l1=setup.split("#");
  if(l1.size()==2)
  {
    auto l=l1[0].split("\t");
    auto l2=l1[1].split("\t");
    if(l.size()==7 && l2.size()==2)
    {
      auto f=l[0];
      for(int i=0;i<ui->scaleType->count();i++)
      {
        if(ui->scaleType->itemData(i).toString()==f)
        {
          ui->scaleType->setCurrentIndex(i);
          break;
        }
      }
      auto mainTicks=l2[0];
      auto subTicks=l2[1];
      auto vals=splitEncodedDoubles(l.mid(1,6));
      if(vals.size()==6)
      {
        ui->scale->setScroll(vals[1],vals[0]); // Scale, offset
        setScaleLimits(vals[2], vals[3]);
        ui->mainTicks->setValue(vals[4]);
        ui->subTicks->setValue(vals[5]);
        ui->manualButton->setChecked(false);
        auto mainTicksVal=splitEncodedDoubles(mainTicks.split(";"));
        auto subTicksVal=splitEncodedDoubles(subTicks.split(";"));
        if(!subTicksVal.isEmpty())
          m_lastManual=QString("%1/%2").arg(toString(mainTicksVal," "),toString(subTicksVal," "));
        else
          m_lastManual=toString(mainTicksVal," ");
        setupScale();
        onScaleLayoutChange();
      }
    }
  }
}

void TestScaleWidget::on_manualButton_clicked(bool checked)
{
  if(!checked)
  {
    ui->scale->setManual();
  }
  else
  {
    bool ok;
    QString result=QInputDialog::getText(this, tr("Setpoints"), QString("Enter manual setpoints"),QLineEdit::Normal, m_lastManual, &ok);
    if(ok)
    {
      QString main, sub;
      if(result.contains("/"))
      {
         main=result.left(result.indexOf("/"));
         sub=result.mid(result.indexOf("/")+1);
      }
      else
        main=result;
      QSet<double> pointsMain=splitDouble(main).toSet();
      QSet<double> pointsSub=splitDouble(sub).toSet();
      if(pointsMain.isEmpty() && pointsSub.isEmpty())
        ui->manualButton->setChecked(false);
      else
      {
        m_lastManual=result;
        ui->scale->setManual(pointsMain, pointsSub);
      }
    }
    else
      ui->manualButton->setChecked(false);

  }
}

void TestScaleWidget::on_buttonOk_clicked()
{
  setupRemoveAll(m_wrongResults, m_curSetup);
  if(m_wrongIndex>=m_wrongResults.size() && !m_wrongResults.isEmpty())
    m_wrongIndex--;
  int i=setupIndex(m_okResults, m_curSetup);
  if(i<0)
  {
    m_okResults.append(m_curSetup);
    m_okIndex=m_okResults.size()-1;
  }
  saveResults();
  onScaleLayoutChange();
}

void TestScaleWidget::setScaleLimits(int left, int right)
{
  QWidget *w=nullptr;
  for(w=this; w; w=w->parentWidget())
  {
    if(dynamic_cast<QMdiSubWindow *>(w) || !w->parentWidget())
      break;
  }
  int minW=w->minimumSizeHint().width();
  int curWidth=w->width(), curHeight=w->height();
  int deltaW=curWidth-(ui->scale->rightPosition()+ui->scale->rightMargin());
  int newWidth=qMax(right+20+deltaW,minW);
  w->resize(newWidth, curHeight);
  auto lm=left;
  auto rm=newWidth-deltaW-right;
  ui->scale->setLeftMargin(lm);
  ui->scale->setRightMargin(rm);
  ui->borderLeft->setValue(lm);
  ui->borderRight->setValue(rm);
}

void TestScaleWidget::on_buttonFail_clicked()
{
  setupRemoveAll(m_okResults, m_curSetup);
  if(m_okIndex>=m_okResults.size() && !m_okResults.isEmpty())
    m_okIndex--;
  int i=setupIndex(m_wrongResults, m_curSetup);
  if(i<0)
  {
    m_wrongResults.append(m_curSetup);
    m_wrongIndex=m_wrongResults.size()-1;
  }
  saveResults();
  onScaleLayoutChange();
//  setScaleLimits(40,500);
}

void TestScaleWidget::onScaleLayoutChange()
{
  m_curSetup=currentSetup();
  bool isOk=false, isWrong=false;
  int i;
  if((i=setupIndex(m_okResults,m_curSetup))>=0)
  {
    m_okIndex=i;
    isOk=true;
  }
  if((i=setupIndex(m_wrongResults,m_curSetup))>=0)
  {
    m_wrongIndex=i;
    isWrong=true;
  }
  ui->buttonDelete->setEnabled(isOk||isWrong);
  ui->buttonOk->setEnabled(!isOk);
  ui->buttonFail->setEnabled(!isWrong);
//  ui->failUp
  ui->okDown->setEnabled( m_okIndex<m_okResults.size()-1);
  ui->failDown->setEnabled( m_wrongIndex<m_wrongResults.size()-1 );
  ui->okUp->setEnabled( m_okIndex>0 );
  ui->failUp->setEnabled( m_wrongIndex>0 );
  ui->okLoad->setEnabled(!m_okResults.isEmpty() && !isOk);
  ui->failLoad->setEnabled(!m_wrongResults.isEmpty() && !isWrong);
  ui->scale->setBackground(isOk?QColor(192,255,192): (isWrong?QColor(255,192,192):QColor(255,255,255)));
  ui->scale->update();
//  qDebug()<<"Current setup"<<m_curSetup;
}


void TestScaleWidget::on_buttonDelete_clicked()
{
  int i=setupIndex(m_okResults, m_curSetup);
  if(i>=0)
  {
    m_okResults.removeAt(i);
    m_okIndex=i;
    if(m_okIndex>0 && m_okIndex>=m_okResults.size())
      m_okIndex--;
  }
  i=setupIndex(m_wrongResults, m_curSetup);
  if(i>=0)
  {
    m_wrongResults.removeAt(i);
    m_wrongIndex=i;
    if(m_wrongIndex>0 && m_wrongIndex>=m_wrongResults.size())
      m_wrongIndex--;
  }
  saveResults();
  onScaleLayoutChange();
}

void TestScaleWidget::on_okLoad_clicked()
{
  Q_ASSERT(m_okIndex>=0 && m_okIndex<m_okResults.size());
  loadSetup(m_okResults[m_okIndex]);
}

void TestScaleWidget::on_okUp_clicked()
{
  Q_ASSERT(m_okIndex>0 && m_okIndex<=m_okResults.size());
  loadSetup(m_okResults[m_okIndex-1]);
}

void TestScaleWidget::on_okDown_clicked()
{
  Q_ASSERT(m_okIndex>=0 && m_okIndex+1<m_okResults.size());
  loadSetup(m_okResults[m_okIndex+1]);
}

void TestScaleWidget::on_failLoad_clicked()
{
  Q_ASSERT(m_wrongIndex>=0 && m_wrongIndex<m_wrongResults.size());
  loadSetup(m_wrongResults[m_wrongIndex]);
}

void TestScaleWidget::on_failUp_clicked()
{
  Q_ASSERT(m_wrongIndex>0 && m_wrongIndex<=m_wrongResults.size());
  loadSetup(m_wrongResults[m_wrongIndex-1]);
}

void TestScaleWidget::on_failDown_clicked()
{
  Q_ASSERT(m_wrongIndex>=0 && m_wrongIndex+1<m_wrongResults.size());
  loadSetup(m_wrongResults[m_wrongIndex+1]);
}

void TestScaleWidget::on_scaleType_currentIndexChanged(int index)
{
  if(index>=0)
    setupScale();
}
