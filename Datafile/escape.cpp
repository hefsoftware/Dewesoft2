#include "escape.h"
#include <functional>
#include <QVector>
#include <QRegularExpression>
QString DataFile::escape(const QString &str, const std::function<bool (char32_t)> &escape, bool cOnly)
{
  QString ret;
  for(int i=0;i<str.size();i++)
  {
    auto ch=str[i];
    if(!escape(ch.unicode()))
      ret+=ch;
    else
    {
      switch(ch.unicode())
      {
        case '\0':
          ret+="\\0"; break;
        case '\a':
          ret+="\\a"; break;
        case '\b':
        ret+="\\b"; break;
        case '\e':
          ret+="\\e"; break;
        case '\f':
          ret+="\\f"; break;
        case '\n':
          ret+="\\n"; break;
        case '\r':
          ret+="\\r"; break;
        case '\t':
          ret+="\\t"; break;
        case '\v':
          ret+="\\v"; break;
        case '\\':
          ret+="\\\\"; break;
        case '\"':
          ret+="\\\""; break;
        case '\'':
          ret+="\\\'"; break;
        default:
          if(ch.unicode()<0x80 && ch.isPrint() && !cOnly)
            ret+=QString("\\")+ch;
          else if(ch.unicode()<0xA0)
            ret+=QString("\\x")+QString::number(ch.unicode(), 16).rightJustified(2,QChar('0'));
          else
            ret+=QString("\\u")+QString::number(ch.unicode(), 16).rightJustified(4,QChar('0'));
      }
    }
  }
  return ret;
}

QString DataFile::escape32(const QString &str, const std::function<bool (char32_t)> &escape, bool cOnly)
{
  auto nameChars=str.toUcs4();
  QString ret;
  for(int i=0;i<nameChars.size();i++)
  {
    auto ch=nameChars[i];
    if(!escape(ch))
      ret+=QString::fromUcs4(&ch, 1);
    else
    {
      switch(ch)
      {
        case '\0':
          ret+="\\0"; break;
        case '\a':
          ret+="\\a"; break;
        case '\b':
        ret+="\\b"; break;
        case '\e':
          ret+="\\e"; break;
        case '\f':
          ret+="\\f"; break;
        case '\n':
          ret+="\\n"; break;
        case '\r':
          ret+="\\r"; break;
        case '\t':
          ret+="\\t"; break;
        case '\v':
          ret+="\\v"; break;
        case '\\':
          ret+="\\\\"; break;
        case '\"':
          ret+="\\\""; break;
        case '\'':
          ret+="\\\'"; break;
        default:
          if(ch<0x80 && QChar::isPrint(ch) && !cOnly)
            ret+=QString("\\")+ch;
          else if(ch<0xA0)
            ret+=QString("\\x")+QString::number(ch, 16).rightJustified(2,QChar('0'));
          else if(ch<0x10000)
            ret+=QString("\\u")+QString::number(ch, 16).rightJustified(4,QChar('0'));
          else
            ret+=QString("\\U")+QString::number(ch, 16).rightJustified(8,QChar('0'));
      }
    }
  }
  return ret;
}

QString DataFile::escapeNoShortcut(const QString &str, const std::function<bool (const QChar &)> &escape)
{
  QString ret;
  for(int i=0;i<str.size();i++)
  {
    auto ch=str[i];
    if(!escape(ch))
      ret+=ch;
    else if(ch<0xA0)
      ret+=QString("\\x")+QString::number(ch.unicode(), 16).rightJustified(2,QChar('0'));
    else
      ret+=QString("\\u")+QString::number(ch.unicode(), 16).rightJustified(4,QChar('0'));
  }
  return ret;
}

QString DataFile::escapeNoShortcut32(const QString &str, const std::function<bool (char32_t)> &escape)
{
  auto nameChars=str.toUcs4();
  QString ret;
  for(int i=0;i<nameChars.size();i++)
  {
    auto ch=nameChars[i];
    if(!escape(ch))
      ret+=QString::fromUcs4(&ch, 1);
    else if(ch<0xA0)
      ret+=QString("\\x")+QString::number(ch, 16).rightJustified(2,QChar('0'));
    else if(ch<0x10000)
      ret+=QString("\\u")+QString::number(ch, 16).rightJustified(4,QChar('0'));
    else
      ret+=QString("\\U")+QString::number(ch, 16).rightJustified(8,QChar('0'));
  }
  return ret;
}

bool DataFile::escapeUnicode(char32_t ch)
{
  switch(ch)
  {
    case '\0': case '\a': case '\b': case '\e': case '\f': case '\n': case '\r': case '\t': case '\v': case '\\': case '\'': case '\"':
      return true;
    default:
      return !QChar::isPrint(ch);
  }
}

bool DataFile::escapePathFunction(char32_t ch)
{
  switch(ch)
  {
    case '\0': case '\a': case '\b': case '\e': case '\f': case '\n': case '\r': case '\t': case '\v': case '\\': case '\'': case '\"': case '/':
      return true;
    default:
      return !QChar::isPrint(ch);
  }
}
bool DataFile::escapeAscii(char32_t ch)
{
  switch(ch)
  {
    case '\0': case '\a': case '\b': case '\e': case '\f': case '\n': case '\r': case '\t': case '\v': case '\\': case '\'': case '\"':
      return true;
    default:
      return (ch>=0x80) || (!QChar::isPrint(ch));
  }
}

QString DataFile::unescape(const QString &name)
{
  QRegularExpression re("\\\\(([xX][0-9A-Fa-f]{1,2})|([uU][0-9A-Fa-f]{1,8})|([0-7]{1,3})|.)");
  QRegularExpressionMatchIterator i = re.globalMatch(name);

  QString ret;
  int pos=0;
  while (i.hasNext()) {
      QRegularExpressionMatch match = i.next();
      if(match.capturedStart()>pos)
        ret.append(name.midRef(pos,match.capturedStart()-pos));
      pos=match.capturedEnd();
      QString escaped=match.captured(1), unescaped;
      switch(escaped[0].unicode())
      {
        case 'x': case 'X': // Hex character
          unescaped=QChar(escaped.midRef(1).toInt(nullptr, 16)); break;
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': // Octal character
          unescaped=QChar(escaped.toInt(nullptr, 8)); break;
        case 'u': case 'U': // Hex character
        {
          char32_t ch=escaped.midRef(1).toUInt(nullptr, 16);
          unescaped=QString::fromUcs4(&ch, 1);
          break;
        }
        case 'a': case 'A':
          unescaped+=QChar('\a'); break;
        case 'b': case 'B':
          unescaped+=QChar('\b'); break;
        case 'e': case 'E':
          unescaped+=QChar('\e'); break;
        case 'f': case 'F':
          unescaped+=QChar('\f'); break;
        case 'n': case 'N':
          unescaped+=QChar('\n'); break;
        case 'r': case 'R':
          unescaped+=QChar('\r'); break;
        case 't': case 'T':
          unescaped+=QChar('\t'); break;
        case 'v': case 'V':
          unescaped+=QChar('\v'); break;
        default:
          unescaped+=escaped[0];
      }
      ret+=unescaped;
  }
  ret.append(name.midRef(pos));
  return ret;
}

QStringList DataFile::splitPath(const QString &path, bool &absolute)
{
  absolute=false;
  static QRegularExpression re;
  static bool initializedRe=false;
  if(!initializedRe)
  {
    QString escape("\\\\");
    re.setPattern(QString::fromLatin1("(?:[^)")+escape+QString::fromLatin1(")](?:")+escape+escape+QString::fromLatin1(")*)(/)"));
    initializedRe=true;
  }
  QStringList ret;

  QRegularExpressionMatchIterator i = re.globalMatch(path);

  QStringList seps;
  int pos=0;
  while (i.hasNext()) {
    QRegularExpressionMatch match = i.next();

    int start=match.capturedStart(1);
    if(start==0)
      absolute=true;
    else
      ret.append(unescape(path.mid(pos, start-pos)));
    pos=match.capturedEnd(1);
  }
  if(pos!=path.size())
    ret.append(unescape(path.mid(pos)));
  return ret;
}

QString DataFile::joinPath(const QString &base, const QString &extra)
{
  return (base.isNull()?QString(""):base+"/")+escapePath(extra);
}

QString DataFile::escapePath(const QString &path)
{
  return escape32(path, escapePathFunction, false);
}

QStringList DataFile::splitPath(const QString &path)
{
  bool abs;
  return splitPath(path, abs);
}

QStringList DataFile::splitUnescapePath(const QString &path)
{
  bool abs;
  auto ret=splitPath(path, abs);
//  for(int i=0;i<ret.size();i++)
//    ret=ret
  return ret;
}

QString DataFile::joinPath(const QStringList &list)
{
  QString ret;
  for(int i=0;i<list.size();i++)
  {
    if(i)
      ret+=QString("/");
    ret+=escapePath(list[i]);
  }
  return ret;
}
