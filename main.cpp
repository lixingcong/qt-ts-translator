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
#include "qt-linguist-shared/translator.h"

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	Translator                    t;
	QList<Translator::FileFormat> formats = t.registeredFileFormats();

	qDebug() << formats.size();
	if (!formats.isEmpty()) {
		ConversionData cd;
		if (t.load("/tmp/app_ja.ts", cd, "ts")) {
			qDebug() << "loaded languageCode" << t.languageCode() << "messageCount=" << t.messageCount();
			const int messageCount = t.messageCount();
			for (int i = 0; i < messageCount; ++i) {
				const TranslatorMessage& msg = t.message(i);
				qDebug() << "i=" << i << msg.sourceText();
			}

			t.message(0).setTranslation("FUCK");
			t.save("/tmp/app_ja-11111.ts", cd, "ts");
		}
	}

	return 0;
}
