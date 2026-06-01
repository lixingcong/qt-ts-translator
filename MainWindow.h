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
class QPushButton;
class QGroupBox;

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
	void setUseFixedDict(bool b);

	void reloadTsTable();
	void refreshFixedDictValid(); // 只刷新第一列

	void moveDictToFixed();
	void moveFixedToDict();
	void forceMoveFixedToDict();
	void copyDictSource();
	void copyDictOldTranslation();
	void copyFixedDictSource();

	void onDictCellChanged(int row, int col);
	void onFixedDictCellChanged(int row, int col);
	void onSwitchToLanguage(const QString& lang);

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

	void setDictRow(int row, int itemType, const QString& src, const QString& oldTranslation, const QString& newTranslation);
	void setFixedDictRow(int row, const QString& src, const QString& translation);

	void removeSelectedFixedDict(bool forceDelete);
	void copyTableWidgetCellText(QTableWidget* tableWidget, int srcCol);

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
	bool                    m_useFixedDict;

	QPushButton* const m_btnFixedDictBrowse;
	QPushButton* const m_btnFixedDictLoad;
	QPushButton* const m_btnFixedDictSave;
	QGroupBox* const   m_groupBoxFixedDict;

	QScopedPointer<TSManager> m_tsManager;

	QMap<QString, QString> m_tableDict; // 这个仅保存用户在界面上手工修改过的翻译（包括从template.txt中加载的），用于刷新表格缓存，不用于真正翻译
};
