#ifndef DATAFILE_STRING_H
#define DATAFILE_STRING_H
#include <QString>
#include <functional>
namespace DataFile
{
  // Escape the characters for which filter returns true. If available "one character" will be used (e.g. \n \r \t)
  QString escape(const QString &str, const std::function<bool(char32_t)> &escape, bool cOnly=true);
  // Escape the characters for which filter returns true. If cOnly is true only standard "one character" sequences will be used (e.g. \n \r \t), otherwise all escaped character <0x80 will be encoded as \? (Pay attention not to escape e.g. n, r, t and so in this case)
  QString escape32(const QString &str, const std::function<bool(char32_t)> &escape, bool cOnly=true);
  // Escape the characters for which filter returns true. For escaping no "one character" escapes will be used but \x?? will be used instead. E.g. \n will be encoded as \x0A
  QString escapeNoShortcut(const QString &str, const std::function<bool(const QChar &ch)> &escape);
  // Escape the characters for which filter returns true. For escaping no "one character" escapes will be used but \x?? will be used instead. E.g. \n will be encoded as \x0A
  QString escapeNoShortcut32(const QString &str, const std::function<bool(char32_t)> &escape);
  bool escapeMinimal(char32_t ch);
  // =escapeUnicode plus '/'
  bool escapePathFunction(char32_t ch);
  bool escapeUnicode(char32_t ch);
  bool escapeAscii(char32_t ch);
  QString unescape(const QString &name);
  QString escapePath(const QString &path);
  QStringList splitPath(const QString &path, bool &absolute);
  QStringList splitPath(const QString &path);
  QStringList splitUnescapePath(const QString &path);
  QString joinPath(const QStringList &list);
  QString joinPath(const QString &base, const QString &extra);
}
#endif // STRING_H
