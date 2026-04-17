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
#include <QFileInfo>

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QRegularExpression>

#include "qt-linguist-shared/translator.h"

static const QString EMOJI_NEWLINE = "🏮";

static inline void removeNewLine(QString& s)
{
	static const QRegularExpression re("\r?\n");
	s.replace(re, EMOJI_NEWLINE);
}

static inline void restoreNewLine(QString& s)
{
	s.replace(EMOJI_NEWLINE, "\n");
}

int main(int argc, char* argv[])
{
	QCoreApplication   app(argc, argv);
	QCommandLineParser parser;
	parser.addHelpOption();

	QCommandLineOption inputTsFileOption("ts", "TS file for input", "TS_FILE");
	QCommandLineOption txtOption("txt", "Txt file for save/load as dict", "TXT_FILE");
	QCommandLineOption outputTsFileOption("ts-out", "TS file for output", "TS_FILE");

	parser.addOption(inputTsFileOption);
	parser.addOption(txtOption);
	parser.addOption(outputTsFileOption);

	parser.process(app);
	if (!parser.isSet(inputTsFileOption)) {
		qCritical("You need to specify the ts file");
		return 1;
	}

	const QString tsFilepath = parser.value(inputTsFileOption);

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

	if (!parser.isSet(txtOption)) {
		qCritical("You need to specify the txt file to save/load as dictionary");
		return 1;
	}

	const QString txtFilepath  = parser.value(txtOption);
	const int     messageCount = translator.messageCount();

	QMap<QString, int> srcToTxtFileLineNum;
	QStringList        srcParsedTexts;
	QStringList        srcTexts;

	{
		// 生成ts字典
		for (int i = 0; i < messageCount; ++i) {
			const TranslatorMessage&      msg     = translator.message(i);
			const TranslatorMessage::Type msgType = msg.type();

			if (TranslatorMessage::Vanished == msgType || TranslatorMessage::Obsolete == msgType)
				continue; // ignore

			const QString src = msg.sourceText();

			QString parsedSrc(src);
			removeNewLine(parsedSrc);

#if 0
			if (src.contains("\n")) {
				qDebug("src contains newline");
			}
#endif

			if (!srcToTxtFileLineNum.contains(parsedSrc)) {
				srcToTxtFileLineNum[parsedSrc] = srcTexts.count();
				srcParsedTexts << parsedSrc;
				srcTexts << src;
			}
		}
	}

	if (!parser.isSet(outputTsFileOption)) {
		// txt已存在
		if (QFileInfo(txtFilepath).isFile()) {
			qInfo("Are you sure to overwrite %s? (y/n)", qPrintable(txtFilepath));

			QTextStream in(stdin);
			if (in.readLine() != "y") {
				qInfo("You choose not to overwrite");
				return 0;
			}
		}

		// 生成txt
		QFile file(txtFilepath);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
			qCritical("Can not open %s", qPrintable(txtFilepath));
			return 1;
		}

		QTextStream textStream(&file);
		textStream.setCodec("UTF-8");

		for (const QString& s : qAsConst(srcParsedTexts))
			textStream << s << Qt::endl;

		qDebug("Line count=%d, saved to %s", srcParsedTexts.count(), qPrintable(txtFilepath));
	} else {
		// 输出新的ts文件
		QMap<QString, QString> dict;
		Q_ASSERT(srcParsedTexts.count() == srcTexts.count());

		QFile file(txtFilepath);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			qCritical("Can not open %s", qPrintable(txtFilepath));
			return 1;
		}

		QTextStream textStream(&file);
		textStream.setCodec("UTF-8");

		auto itSrc    = srcTexts.constBegin();
		auto itSrcEnd = srcTexts.constEnd();

		while (!textStream.atEnd() && itSrc != itSrcEnd) {
			const QString translated = textStream.readLine();
			QString       parsedTranslated(translated);
			restoreNewLine(parsedTranslated);

			//qDebug() << parsedTranslated;
			dict[*(itSrc++)] = parsedTranslated;
		}

		int updatedCount = 0;
		for (int i = 0; i < messageCount; ++i) {
			TranslatorMessage&            msg     = translator.message(i);
			const TranslatorMessage::Type msgType = msg.type();

			if (TranslatorMessage::Vanished == msgType || TranslatorMessage::Obsolete == msgType)
				continue; // ignore

			const QString src = msg.sourceText();
			if (dict.contains(src)) {
				const QString translatedTxt = dict.value(src);
				const QString translatedTs  = msg.translation();

				if (translatedTxt != src && translatedTxt != translatedTs) { // not translate yet
					msg.setTranslation(translatedTxt);
					msg.setType(TranslatorMessage::Finished);
					++updatedCount;
				}
			}
		}

		const QString tsOutputFilepath = parser.value(outputTsFileOption);
		const bool    saved            = translator.save(tsOutputFilepath, cd, "ts");

		qInfo("Updated %d items. Save to %s %s", updatedCount, qPrintable(tsOutputFilepath), (saved ? "OK" : "Failed"));
	}

	return 0;
}
