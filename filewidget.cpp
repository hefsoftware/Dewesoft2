#include "filewidget.h"
#include "ui_filewidget.h"
#include "Datafile/escape.h"
#include <QDebug>
FileWidget::FileWidget(DataFile::FilePtr file, QWidget *parent) :
  QWidget(parent), m_file(file),
  ui(new Ui::FileWidget)
{
  ui->setupUi(this);
  setWindowTitle(file->filename());
  addEntry(file, nullptr);
}

void FileWidget::addEntry(DataFile::EntryPtr ptr, QTreeWidgetItem *parent, const QString &path)
{
  auto c=ptr->children();
//  auto keys=c.keys();
//  std::sort(keys.begin(), keys.end(), [&c](const DataFile::Entry::INode &k1, const DataFile::Entry::INode &k2){return c[k1]<c[k2]; } );
  for(auto it=c.begin(); it!=c.end(); it++)
  {
    auto key=it.key();
    auto inode=it.value();
    auto newPath=DataFile::Entry::iNodeJoin(path, inode);
    QTreeWidgetItem *item=new QTreeWidgetItem({key});
    item->setData(0, Qt::UserRole, newPath);
    item->setData(0, Qt::ToolTipRole, tr("INode path: %1\nKey: %2").arg(newPath, DataFile::escape(key, DataFile::escapeUnicode)));
    if(parent)
      parent->addChild(item);
    else
      ui->tree->addTopLevelItem(item);
    auto c=ptr->openINode(inode);
    if(c)
    {
      addEntry(c, item, newPath);
    }
  }
  if(parent)
    parent->setExpanded(true);
}

FileWidget::~FileWidget()
{
  delete ui;
}

void FileWidget::on_tree_itemClicked(QTreeWidgetItem *item, int column)
{

}
#include <cxxabi.h>
static inline QString demangle(const char* mangled)
{
      int status;
      std::unique_ptr<char[], void (*)(void*)> result(
        abi::__cxa_demangle(mangled, 0, 0, &status), std::free);
      return result.get() ? QString::fromUtf8(result.get()) : QString("Demangle error");
}
void FileWidget::on_tree_itemSelectionChanged()
{
  auto it=ui->tree->currentItem();
  DataFile::EntryPtr ptr;
  QString path;
  if(it)
  {
    path=it->data(0, Qt::UserRole).toString();
    ptr=m_file->openINode(DataFile::Entry::iNodeSplit(path));
  }
  ui->data->setVisible(ptr);
  ui->metaData->setVisible(ptr);
  while(ui->metaData->rowCount())ui->metaData->removeRow(0);
  if(!ptr)
  {
    if(path.isEmpty())
      ui->dataDescription->setText(tr("---"));
    else
      ui->dataDescription->setText(tr("Could not open %1").arg(path));
  }
  else
  {
    DataFile::Entry *entry=ptr.data();
    ui->dataDescription->setText(tr("%1\t%2").arg(path, demangle(typeid(*entry).name())));
    auto meta=entry->metadata();
    for(auto i=meta.begin();i!=meta.end();i++)
    {
      int pos=ui->metaData->rowCount();
      ui->metaData->insertRow(pos);
      ui->metaData->setItem(pos, 0, new QTableWidgetItem(i.key()));
      ui->metaData->setItem(pos, 1, new QTableWidgetItem(i.value().toString()));
    }
  }
}
