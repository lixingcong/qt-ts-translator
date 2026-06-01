#include "TSManager.h"
#include "qt-linguist-shared/translator.h"
//#include <QDebug>

TSManager::TSManager(const QString& filepath)
    : m_filepath(filepath)
{

}

TSManager::~TSManager() {}

bool TSManager::load()
{
	resetData();

	if (m_translater->registeredFileFormats().isEmpty() || !m_translater->load(m_filepath, *m_conversionData, "ts"))
		return false;

	//qDebug().noquote() << m_filepath << "is loaded, languageCode=" << m_translater->languageCode() << "messageCount=" << m_translater->messageCount();

	const int messageCount = m_translater->messageCount();
	for (int i = 0; i < messageCount; ++i) {
		const TranslatorMessage&      msg     = m_translater->message(i);
		const TranslatorMessage::Type msgType = msg.type();

		Item item;
		item.src         = msg.sourceText();
		item.translation = msg.translation();

		bool skip = false;

		switch (msgType) {
#define CASE(xxx) \
	case TranslatorMessage::xxx: \
		item.type = xxx; \
		break;

			CASE(Unfinished)
			CASE(Finished)
			CASE(Vanished)
			CASE(Obsolete)
#undef CASE
		default:
			skip = true;
			break;
		}

		if (skip)
			continue;


		if (!m_srcTextToItem.contains(item.src)) {
			item.messageIndices << i;
			m_srcTextToItem[item.src] = item;
		} else {
			Item& existedItem = m_srcTextToItem[item.src];
			if (existedItem.type != item.type) {
				// 这里做了简化处理，忽略了一种情况：src相同，但是i不同，其msgType也可能不相同
				// 这里统一视为Unfinished
				existedItem.type = Unfinished;
			}

			existedItem.messageIndices << i;
		}
	}

	return true;
}

bool TSManager::save() const
{
	if (!m_translater || !m_conversionData)
		return false;

	return m_translater->save(m_filepath, *m_conversionData, "ts");
}

void TSManager::setTranslations(const QMap<QString, QString>& dict)
{
	for (auto it = dict.constBegin(), itEnd = dict.constEnd(); it != itEnd; ++it) {
		const QString src = it.key();
		const QString translation = it.value();

		if (m_srcTextToItem.contains(src) && !translation.isEmpty()) {
			Item& item   = m_srcTextToItem[src];
			item.translation = translation;
			item.type        = Finished;

			for(int idx: qAsConst(item.messageIndices)){
				TranslatorMessage& msg = m_translater->message(idx);
				msg.setTranslation(item.translation);
				msg.setType(TranslatorMessage::Finished);
			}
		}
	}
}

void TSManager::resetData()
{
	m_translater.reset(new Translator);
	m_conversionData.reset(new ConversionData);
	m_srcTextToItem.clear();
}
