/*
 * TranslateLoader.cpp
 *
 * Brief: 功能简要概述
 *
 * Created on: 2026年 3月 5日
 *
 * Author: lixingcong
 */

#include "TranslateLoader.h"
#include <QCoreApplication>
#include <QLocale>
#include <QPointer>
#include <QTranslator>
#include <QSettings>

static constexpr const char* TR_CONTEXT = "TranslateLoader";
static constexpr const char* KEY_LOCALE = "locale";

// https://stackoverflow.com/questions/4415530/equivalents-to-msvcs-countof-in-other-compilers
template<typename T>
constexpr size_t countof(const T&)
{
	return std::extent<T>::value;
}

namespace TranslateLoader
{

struct SupportedLanguage
{
	const char* displayName; // 用与界面显示下拉框切换当前语言
	const char* localeName; // 用于构造QLocale对象
};

static constexpr SupportedLanguage s_langList[] = {
     {"English", "C"}, // 第1项，默认英文
    {QT_TRANSLATE_NOOP_UTF8("TranslateLoader", "Chinese(Simplified)"), "zh_CN"}, // 简中
    {QT_TRANSLATE_NOOP_UTF8("TranslateLoader", "Japanese"), "ja_JP"}, // 日语
};

static constexpr size_t s_langCount          = countof(s_langList);
static int              s_currentLanguageIdx = 0;

static QTranslator* s_qtTranslater;
static QTranslator* s_appTranslater;

QStringList supportedLanguages()
{
	QStringList ret = {"English"}; // 第1项，默认英文
	for (size_t i = 1; i < s_langCount; ++i)
		ret << QCoreApplication::translate(TR_CONTEXT, s_langList[i].displayName);
	return ret;
}

void loadAtRun()
{
	const QString iniFilepath = QString("%1/language.ini").arg(QCoreApplication::applicationDirPath());
	QSettings     settings(iniFilepath, QSettings::IniFormat);
	settings.setIniCodec("UTF-8");

	if (settings.contains(KEY_LOCALE)) {
		const QString L = settings.value(KEY_LOCALE).toString();
		for (size_t i = 0; i < s_langCount; ++i) {
			if (L == s_langList[i].localeName) {
				setLanguage(i);
				qDebug("Loaded locale \"%s\" from language.ini", s_langList[i].localeName);
				return; // stop
			}
		}
	}

	// 自动加载
	setLanguage(-1);
	qDebug("Auto select language to %d", s_currentLanguageIdx);
	//return; // debug
	if (s_currentLanguageIdx > 0) {
		// 这里不调用saveLanguage()因为又都打开了一个QSettings。并共享相同的ini文件
		const char* newLocaleName = s_langList[s_currentLanguageIdx].localeName;
		settings.setValue(KEY_LOCALE, newLocaleName);
		qDebug("Saved locale \"%s\" to language.ini", newLocaleName);
	}
}

int currentLanguage()
{
	return s_currentLanguageIdx;
}

int setLanguage(int idx)
{
	QLocale locale;

	if (idx >= 0 && idx < s_langCount)
		locale = QLocale(s_langList[idx].localeName);

	QCoreApplication* app = QCoreApplication::instance();

	auto loadTranslate = [app, &locale](const QString& filename, const QString& prefix, const QString& dir) -> QTranslator* {
		QTranslator* translator = new QTranslator(app);

		const bool loaded = translator->load(locale, filename, prefix, dir);

		if (loaded) {
			app->installTranslator(translator);
		} else {
			delete translator;
			translator = nullptr;
		}

		//qDebug("locale: %s, load translate %s, result:%d", qPrintable(locale.name()), qPrintable(filename), loaded);
		return translator;
	};

	// QTBASE
	if (s_qtTranslater) {
		app->removeTranslator(s_qtTranslater);
		s_qtTranslater->deleteLater();
	}
	s_qtTranslater = loadTranslate("qtbase", "_", ":/i18n");

	// APP
	if (s_appTranslater) {
		app->removeTranslator(s_appTranslater);
		s_appTranslater->deleteLater();
	}
	s_appTranslater = loadTranslate("app", "_", ":/i18n");

	if (s_appTranslater) {
		// 找出正常加载的语言索引
		for (size_t i = 0; i < s_langCount; ++i) {
			if (locale.name() == s_langList[i].localeName) {
				s_currentLanguageIdx = i;
				return i;
			}
		}
	}

	s_currentLanguageIdx = 0;
	return 0; // 没有加载任何翻译，则为英文
}

void saveLanguage()
{
	const QString iniFilepath = QString("%1/language.ini").arg(QCoreApplication::applicationDirPath());
	QSettings     settings(iniFilepath, QSettings::IniFormat);
	settings.setIniCodec("UTF-8");


	const char* localeName = s_langList[s_currentLanguageIdx].localeName;
	settings.setValue(KEY_LOCALE, localeName);
	qDebug("Saved locale \"%s\" to language.ini", localeName);
}
}

