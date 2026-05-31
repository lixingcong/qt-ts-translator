#pragma once

#include <QString>
#include <QMap>
#include <QScopedPointer>

class Translator; // qt-linguist
class ConversionData; // qt-linguist

class TSManager
{
public:
	TSManager(const QString& filepath);
	~TSManager();

	bool load();
	bool save() const;

	inline QString filepath() const { return m_filepath; }

	enum ItemType { Unfinished, Finished, Vanished, Obsolete };

	struct Item
	{
		ItemType   type;
		QString    src;
		QString    translation;
		QList<int> messageIndices; // 对应ts文件的message(idx)，一个翻译可能对应多个idx
	};

	inline QMap<QString, Item> items() const { return m_srcTextToItem; }

	void setTranslations(const QMap<QString, QString>& dict);

protected:
	void resetData();

protected:
	const QString                  m_filepath;
	QScopedPointer<Translator>     m_translater;
	QScopedPointer<ConversionData> m_conversionData;
	QMap<QString, Item>            m_srcTextToItem;
};

