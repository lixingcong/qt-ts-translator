/*
 * FixedDictManager.h
 *
 * Brief: 功能简要概述
 *
 * Created on: 2026年 5月 31日
 *
 * Author: lixingcong
 */
#pragma once

#include <QStringList>
#include <QMap>

class QJsonObject;

class FixedDictManager
{
public:
	FixedDictManager();
	~FixedDictManager();

	void load(const QJsonObject& j);
	void save(QJsonObject* out);

	using Dict = QMap<QString, QString>;
	Dict dict(const QString& lang) const; // 返回src -> translation

	inline QStringList languages() const { return m_languages; }
	bool addLanguage(const QString& lang);
	bool removeLanguage(const QString& lang);

	bool setTransaltion(const QString& lang, const QString& src, const QString& translation);
	bool removeSrc(const QString& src);

	QString defaultLanguage() const;

public:
	void reset();

protected:
	QMap<QString, Dict> m_srcToDict; // Dict的含义：language -> translation，如"zh_CN"->"你好"
	QStringList         m_languages;
};
