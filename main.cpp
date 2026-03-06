/*
 * %{Cpp:License:FileName}
 *
 * Brief: 功能简要概述
 *
 * Created on: 2026年 3月 6日
 *
 * Author: lixingcong
 */

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QMap>
#include <QSet>
#include <QTextStream>

#include <QCommandLineParser>
#include <QCommandLineOption>

#include "qt-linguist-shared/translator.h"

using StrMap = QMap<QString, QString>; // source -> translated

int main(int argc, char* argv[])
{
	QCoreApplication   app(argc, argv);
	QCommandLineParser parser;
	parser.addHelpOption();

	QCommandLineOption tsFileOption("ts", "the ts file", "TS_FILE");
	QCommandLineOption generateTxtOption("generate", "generate the list", "TXT_FILE");

	parser.addOption(tsFileOption);
	parser.addOption(generateTxtOption);

	parser.process(app);
	if (!parser.isSet(tsFileOption)) {
		qCritical("You need to specify the ts file");
		return 1;
	}

	const QString tsFilepath = parser.value(tsFileOption);

	Translator     translator;
	ConversionData cd;

	if (translator.registeredFileFormats().isEmpty()) {
		qCritical("No registered file formats");
		return 1;
	}

	if (translator.load(tsFilepath, cd, "ts")) {
		qDebug() << "loaded languageCode=" << translator.languageCode() << "messageCount=" << translator.messageCount();
	} else {
		qCritical("Can not load %s", qPrintable(tsFilepath));
		return 1;
	}

	if (parser.isSet(generateTxtOption)) {
		const QString generateFilepath = parser.value(generateTxtOption);
		QSet<QString> sourceTextSet;

		QFile file(generateFilepath);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
			qCritical("Can not open %s", qPrintable(generateFilepath));
			return 1;
		}

		QTextStream textStream(&file);

		const int messageCount = translator.messageCount();
		for (int i = 0; i < messageCount; ++i) {
			const TranslatorMessage& msg = translator.message(i);
			const QString            src = msg.sourceText();
			if (!sourceTextSet.contains(src)) {
				sourceTextSet.insert(src);
				textStream << src << Qt::endl;
			}
		}

		// translator.message(0).setTranslation("FUCK");
		// translator.save("/tmp/app_ja-11111.ts", cd, "ts");
		qDebug("Saved to %s", qPrintable(generateFilepath));
		return 0;
	}

	return 0;
}
