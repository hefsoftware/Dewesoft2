#include "file.h"
using namespace DataFile;

File::File(): Entry(nullptr)
{

}

QString File::filename() const
{
  return m_filename;
}
