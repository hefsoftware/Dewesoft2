#ifndef FILEWIDGET_H
#define FILEWIDGET_H

#include <QWidget>
#include "Datafile/file.h"

namespace Ui {
  class FileWidget;
}
class QTreeWidgetItem;
class FileWidget : public QWidget
{
  Q_OBJECT

public:
  explicit FileWidget(DataFile::FilePtr file, QWidget *parent = nullptr);
  ~FileWidget();
protected:
  DataFile::FilePtr m_file;
  void addEntry(DataFile::EntryPtr ptr, QTreeWidgetItem *parent, const QString &path=QString());
private slots:
  void on_tree_itemClicked(QTreeWidgetItem *item, int column);

  void on_tree_itemSelectionChanged();

private:
  Ui::FileWidget *ui;
};

#endif // FILEWIDGET_H
