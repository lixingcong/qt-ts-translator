/*
 * FixedDictManager.cpp
 *
 * Brief: 功能简要概述
 *
 * Created on: 2026年 5月 31日
 *
 * Author: lixingcong
 */
#include "FixedDictManager.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>

namespace
{
constexpr const char* KEY_SOURCE       = "src";
constexpr const char* KEY_ITEMS        = "items";
constexpr const char* KEY_TRANSLATIONS = "translations";
constexpr const char* KEY_LANGUAGES        = "languages";
constexpr const char* LANGUAGE_DEFAULT      = "default";

}

FixedDictManager::FixedDictManager()
{
	reset();
}

FixedDictManager::~FixedDictManager() {}

void FixedDictManager::load(const QJsonObject &j)
{
	reset();

	// lang names
	const QJsonArray jLanguages = j.value(KEY_LANGUAGES).toArray();
	for (const QJsonValue& jv : jLanguages) {
		const QString lang = jv.toString();
		if (!lang.isEmpty() && lang != LANGUAGE_DEFAULT) {
			m_languages << lang;
		}
	}

	// build lang->idx map
	const QSet<QString> m_languageSet = m_languages.toSet();

	// translations
	const QJsonArray itemArray = j.value(KEY_ITEMS).toArray();
	for (const QJsonValue& jv : itemArray) {
		const QJsonObject item = jv.toObject();
		const QString     src  = item.value(KEY_SOURCE).toString();

		if (!src.isEmpty()) {
			const QJsonObject jTranslations = item.value(KEY_TRANSLATIONS).toObject();
			Dict              translations;

			const QStringList languages = jTranslations.keys();
			for (const QString& lang : languages) {
				if (!m_languageSet.contains(lang))
					continue;

				translations[lang] = jTranslations.value(lang).toString();
			}

			m_srcToDict[src] = translations;
		}
	}
}

void FixedDictManager::save(QJsonObject* out)
{
	out->operator[](KEY_LANGUAGES) = QJsonArray::fromStringList(m_languages);

	QJsonArray jItems;
	for (auto it = m_srcToDict.constBegin(), itEnd = m_srcToDict.constEnd(); it != itEnd; ++it) {
		const QString& src  = it.key();
		const Dict&    dict = it.value();

		QJsonObject jDict;
		for (auto dIt = dict.constBegin(), dItEnd = dict.constEnd(); dIt != dItEnd; ++dIt)
			jDict[dIt.key()] = dIt.value();

		jItems.append(QJsonObject({{KEY_SOURCE, src}, {KEY_TRANSLATIONS, jDict}}));
	}
	out->operator[](KEY_ITEMS) = jItems;
}

FixedDictManager::Dict FixedDictManager::dict(const QString& lang) const
{
	QString K = lang.isEmpty() ? LANGUAGE_DEFAULT : lang;
	Dict ret;
	for (auto it = m_srcToDict.constBegin(), itEnd = m_srcToDict.constEnd(); it != itEnd; ++it) {
		const Dict& langToTranslation = it.value();
		ret[it.key()]                 = langToTranslation.value(K);
	}
	return ret;
}

void FixedDictManager::reset()
{
	m_srcToDict.clear();
	m_languages.clear();
	m_languages.append(LANGUAGE_DEFAULT);
}

bool FixedDictManager::addLanguage(const QString& lang)
{
	if (lang.isEmpty() || m_languages.contains(lang))
		return false;

	m_languages << lang;
	return true;
}

bool FixedDictManager::removeLanguage(const QString& lang)
{
	if (lang == LANGUAGE_DEFAULT || lang.isEmpty()  || !m_languages.contains(lang))
		return false;

	for (auto it = m_srcToDict.begin(), itEnd = m_srcToDict.end(); it != itEnd; ++it) {
		Dict& dict = it.value();
		dict.remove(lang);
	}

	m_languages.removeOne(lang);

	return true;
}

bool FixedDictManager::setTransaltion(const QString& lang, const QString& src, const QString& translation)
{
	QString K = lang.isEmpty() ? LANGUAGE_DEFAULT : lang;

	if (!m_languages.contains(K))
		return false;

	if (m_srcToDict.contains(src)) {
		Dict& dict = m_srcToDict[src];
		dict[K]    = translation;
	} else {
		Dict dict;
		dict[K]          = translation;
		m_srcToDict[src] = dict;
	}

	return true;
}

bool FixedDictManager::removeSrc(const QString& src)
{
	return m_srcToDict.remove(src) > 0;
}

QString FixedDictManager::defaultLanguage() const
{
	return LANGUAGE_DEFAULT;
}
