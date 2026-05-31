/*
 * MainWindow.h
 *
 * Brief: 功能简要概述
 *
 * Created on: 2026年 5月 29日
 *
 * Author: lixingcong
 */
#pragma once

#include <QWidget>
#include <QMap>

class QLineEdit;
class QTableWidget;
class QCheckBox;
class TSManager;
class FixedDictManager;
class QComboBox;

class MainWindow : public QWidget
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();

protected Q_SLOTS:
	void onClickedBrowseTs();
	void onClickedLoadTs();
	void onClickedSaveTs();

	void onClickedBrowseOnlineDict();
	void onClickedLoadOnlineDict();
	void onClickedExportOnlineDictTemplate();

	void onClickedBrowseFixedDict();
	void onClickedLoadFixedDict();
	void onClickedSaveFixedDit();

	void onClickedAddLanguage();
	void onClickedRemoveLanguage();

	void onToggledIgnore();

	void reloadTsTable();
	void refreshFixedDictValid(); // 只刷新第一列

	void moveDictToFixed();
	void moveFixedToDict();
	void clearFixedDict();

	void onFixedDictCellChanged(int row, int col);
	void onSwitchToLanguage(const QString& lang);

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

	void setDictRow(int row, int itemType, const QString& src, const QString& translation);
	void setFixedDictRow(int row, const QString& src, const QString& translation);

	QString currentLanguage() const;

protected:
	QTableWidget* const m_tableWidgetDict;
	QTableWidget* const m_tableWidgetFixedDict; // 固定翻译

	QLineEdit* const m_lineEditSourceTS; // 源ts
	QLineEdit* const m_lineEditFixedDict; // 固定翻译
	QLineEdit* const m_lineEditOnlineDict; // 机器翻译

	QCheckBox* const m_checkBoxIgnoreFinished;
	QCheckBox* const m_checkBoxIgnoreVanishedObsolete;

	QComboBox* const m_comboBoxFixedDictLanguage;

	FixedDictManager* const m_fixedDictManager; // 固定翻译
	QMap<QString, QString>  m_fixedDict;

	QScopedPointer<TSManager> m_tsManager;
};
