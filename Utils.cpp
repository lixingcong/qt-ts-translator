#include "Utils.h"
#include <QRegularExpression>

namespace Utils
{

static const QString EMOJI_NEWLINE = QStringLiteral("🏮");

void removeNewLine(QString &s)
{
	static const QRegularExpression re("\r?\n");
	s.replace(re, EMOJI_NEWLINE);
}

void restoreNewLine(QString &s)
{
	s.replace(EMOJI_NEWLINE, "\n");
}


}
