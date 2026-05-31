/*
 * MainWindow.cpp
 *
 * Brief: 功能简要概述
 *
 * Created on: 2026年 5月 29日
 *
 * Author: lixingcong
 */
#include "MainWindow.h"
#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QCheckBox>
#include <QSettings>
#include <QTextStream>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QAction>
#include <QMenu>
#include <QCursor>
#include <QContextMenuEvent>
#include <QComboBox>
#include <QInputDialog>

#include "TSManager.h"
#include "FixedDictManager.h"
#include "Utils.h"

namespace
{
constexpr const char* CONFIG_FILE     = "config.ini";
constexpr const char* KEY_SRC_TS      = "source_ts";
constexpr const char* KEY_ONLINE_DICT = "dict_online";
constexpr const char* KEY_FIXED_DICT  = "dict_fixed";
}

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
    , m_tableWidgetDict(new QTableWidget)
    , m_tableWidgetFixedDict(new QTableWidget)
    , m_lineEditSourceTS(new QLineEdit)
    , m_lineEditFixedDict(new QLineEdit)
    , m_lineEditOnlineDict(new QLineEdit)
    , m_checkBoxIgnoreFinished(new QCheckBox("Ignore Finished"))
    , m_checkBoxIgnoreVanishedObsolete(new QCheckBox("Ignore Obsolete"))
    , m_comboBoxFixedDictLanguage(new QComboBox)
    , m_fixedDictManager(new FixedDictManager)
{
	auto mainLayout = new QVBoxLayout(this);

	{
		auto gridLayout = new QGridLayout;
		mainLayout->addLayout(gridLayout);

		int row = 0;

		{
			int  col       = 0;
			auto btnBrowse = new QPushButton("Browse");
			auto btnLoad   = new QPushButton("Load");
			auto btnSave = new QPushButton("Save");

			gridLayout->addWidget(new QLabel("TS File"), row, col++);
			gridLayout->addWidget(m_lineEditSourceTS, row, col++);
			gridLayout->addWidget(btnBrowse, row, col++);
			gridLayout->addWidget(btnLoad, row, col++);
			gridLayout->addWidget(btnSave, row, col++);

			m_lineEditSourceTS->setPlaceholderText("Qt *.ts file");

			connect(btnBrowse, &QPushButton::clicked, this, &MainWindow::onClickedBrowseTs);
			connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onClickedLoadTs);
			connect(btnSave, &QPushButton::clicked, this, &MainWindow::onClickedSaveTs);

			++row;
		}

		{
			int col = 0;

			auto btnBrowse = new QPushButton("Browse");
			auto btnLoad   = new QPushButton("Load");
			auto btnSave   = new QPushButton("Save");
			gridLayout->addWidget(new QLabel("Custom Fixed Dictionary"), row, col++);
			gridLayout->addWidget(m_lineEditFixedDict, row, col++);

			m_lineEditFixedDict->setPlaceholderText("Json file");

			gridLayout->addWidget(btnBrowse, row, col++);
			gridLayout->addWidget(btnLoad, row, col++);
			gridLayout->addWidget(btnSave, row, col++);

			connect(btnBrowse, &QPushButton::clicked, this, &MainWindow::onClickedBrowseFixedDict);
			connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onClickedLoadFixedDict);
			connect(btnSave, &QPushButton::clicked, this, &MainWindow::onClickedSaveFixedDit);

			++row;
		}

		{
			int col = 0;

			auto btnBrowse = new QPushButton("Browse");
			auto btnLoad   = new QPushButton("Load");
			auto btnSave   = new QPushButton("Export Template");
			gridLayout->addWidget(new QLabel("Online Dictionary"), row, col++);
			gridLayout->addWidget(m_lineEditOnlineDict, row, col++);

			m_lineEditOnlineDict->setPlaceholderText("Text file to auto translate");

			gridLayout->addWidget(btnBrowse, row, col++);
			gridLayout->addWidget(btnLoad, row, col++);
			gridLayout->addWidget(btnSave, row, col++);

			connect(btnBrowse, &QPushButton::clicked, this, &MainWindow::onClickedBrowseOnlineDict);
			connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onClickedLoadOnlineDict);
			connect(btnSave, &QPushButton::clicked, this, &MainWindow::onClickedExportOnlineDictTemplate);

			++row;
		}
	}

	{
		auto groupBox = new QGroupBox("TS File(Edit is not allow)");
		auto l        = new QVBoxLayout(groupBox);

		{
			auto btnLayout = new QHBoxLayout;
			btnLayout->setMargin(0);
			btnLayout->addWidget(m_checkBoxIgnoreFinished);
			btnLayout->addWidget(m_checkBoxIgnoreVanishedObsolete);
			btnLayout->addStretch();

			m_checkBoxIgnoreVanishedObsolete->setChecked(true);
			connect(m_checkBoxIgnoreFinished, &QCheckBox::toggled, this, &MainWindow::onToggledIgnore);
			connect(m_checkBoxIgnoreVanishedObsolete, &QCheckBox::toggled, this, &MainWindow::onToggledIgnore);
			l->addLayout(btnLayout);
		}

		l->addWidget(m_tableWidgetDict);

		m_tableWidgetDict->setAlternatingRowColors(true);
		m_tableWidgetDict->verticalHeader()->hide();
		m_tableWidgetDict->setColumnCount(3);
		m_tableWidgetDict->setHorizontalHeaderLabels({"Status", "Source", "Translation"});
		m_tableWidgetDict->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_tableWidgetDict->setSelectionMode(QAbstractItemView::ContiguousSelection);
		m_tableWidgetDict->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_tableWidgetDict->installEventFilter(this);

		QHeaderView* horizontalHeader= m_tableWidgetDict->horizontalHeader();
		horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
		horizontalHeader->setSectionResizeMode(1, QHeaderView::Stretch);
		horizontalHeader->setSectionResizeMode(2, QHeaderView::Stretch);

		mainLayout->addWidget(groupBox);
	}

	{
		auto groupBox = new QGroupBox("Custom Fixed Dictionary(Editable)");
		auto l        = new QVBoxLayout(groupBox);

		{
			auto btnLayout = new QHBoxLayout;
			btnLayout->setMargin(0);
			l->addLayout(btnLayout);

			auto btnAdd    = new QPushButton("Add Lang");
			auto btnRemove = new QPushButton("Remove Lang");

			btnLayout->addWidget(new QLabel("Language"));
			btnLayout->addWidget(m_comboBoxFixedDictLanguage);
			btnLayout->addWidget(btnAdd);
			btnLayout->addWidget(btnRemove);
			btnLayout->addStretch();

			m_comboBoxFixedDictLanguage->addItem(m_fixedDictManager->defaultLanguage());
			onSwitchToLanguage(m_fixedDictManager->defaultLanguage());

			connect(m_comboBoxFixedDictLanguage, &QComboBox::currentTextChanged, this, &MainWindow::onSwitchToLanguage);
			connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onClickedAddLanguage);
			connect(btnRemove, &QPushButton::clicked, this, &MainWindow::onClickedRemoveLanguage);
		}

		l->addWidget(m_tableWidgetFixedDict);

		m_tableWidgetFixedDict->setAlternatingRowColors(true);
		m_tableWidgetFixedDict->verticalHeader()->hide();
		m_tableWidgetFixedDict->setColumnCount(3);
		m_tableWidgetFixedDict->setHorizontalHeaderLabels({"Valid", "Source", "Translation"});
		m_tableWidgetFixedDict->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_tableWidgetFixedDict->setSelectionMode(QAbstractItemView::ContiguousSelection);
		m_tableWidgetFixedDict->installEventFilter(this);

		connect(m_tableWidgetFixedDict, &QTableWidget::cellChanged, this, &MainWindow::onFixedDictCellChanged);

		QHeaderView* horizontalHeader= m_tableWidgetFixedDict->horizontalHeader();
		horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
		horizontalHeader->setSectionResizeMode(1, QHeaderView::Stretch);
		horizontalHeader->setSectionResizeMode(2, QHeaderView::Stretch);

		mainLayout->addWidget(groupBox);
	}

	{
		// load settings
		QSettings settings(CONFIG_FILE, QSettings::IniFormat);
		settings.setIniCodec("UTF-8");
		m_lineEditSourceTS->setText(settings.value(KEY_SRC_TS).toString());
		m_lineEditOnlineDict->setText(settings.value(KEY_ONLINE_DICT).toString());
		m_lineEditFixedDict->setText(settings.value(KEY_FIXED_DICT).toString());
	}
}

MainWindow::~MainWindow()
{
	// save settings
	QSettings settings(CONFIG_FILE, QSettings::IniFormat);
	settings.setIniCodec("UTF-8");
	settings.setValue(KEY_SRC_TS, m_lineEditSourceTS->text());
	settings.setValue(KEY_ONLINE_DICT, m_lineEditOnlineDict->text());
	settings.setValue(KEY_FIXED_DICT, m_lineEditFixedDict->text());

	delete m_fixedDictManager;
}

void MainWindow::onClickedBrowseTs()
{
	const QString t1 = m_lineEditSourceTS->text();
	const QString t2 = QFileDialog::getOpenFileName(this, QString(), t1, "TS Files (*.ts);;");
	if (!t2.isEmpty()) {
		m_lineEditSourceTS->setText(t2);
	}
}

void MainWindow::onClickedLoadTs()
{
	const QString filepath = m_lineEditSourceTS->text();
	if (filepath.isEmpty())
		return;

	m_tsManager.reset(new TSManager(filepath));
	if (m_tsManager->load()) {
		reloadTsTable();
		refreshFixedDictValid();
	} else {
		QMessageBox::warning(this, QString(), "Failed to load TS");
	}
}

void MainWindow::onClickedSaveTs()
{
	if (!m_tsManager || !m_tsManager->save()) {
		QMessageBox::warning(this, QString(), "Failed to save TS: file is not opened yet");
		return;
	}

	if (QMessageBox::No
	    == QMessageBox::question(this, QString(), QString("Are you sure to overwrite %1?").arg(m_tsManager->filepath()), QMessageBox::Yes | QMessageBox::No))
		return;

	QMap<QString, QString> dict;

	const int TableRowCount = m_tableWidgetDict->rowCount();
	for (int row = 0; row < TableRowCount; ++row) {
		const QString src         = m_tableWidgetDict->item(row, 1)->text();
		const QString translation = m_tableWidgetDict->item(row, 2)->text();
		dict[src]                 = translation;
	}

	for (auto it = m_fixedDict.constBegin(), itEnd = m_fixedDict.constEnd(); it != itEnd; ++it) {
		dict[it.key()] = it.value();
	}

	m_tsManager->setTranslations(dict);
	if (m_tsManager->save()) {
		QMessageBox::information(this, QString(), "Saved");
	}else{
		QMessageBox::critical(this, QString(), QString("Failed to save"));
	}
}

void MainWindow::onClickedBrowseOnlineDict()
{
	const QString t1 = m_lineEditOnlineDict->text();
	const QString t2 = QFileDialog::getOpenFileName(this, QString(), t1, "Text Files (*.txt);;");
	if (!t2.isEmpty()) {
		m_lineEditOnlineDict->setText(t2);
	}
}

void MainWindow::onClickedLoadOnlineDict()
{
	const QString filepath = m_lineEditOnlineDict->text();
	if (filepath.isEmpty())
		return;

	QFile file(filepath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(this, QString(), QString("Failed to open %1").arg(filepath));
		return;
	}

	QTextStream textStream(&file);
	textStream.setCodec("UTF-8");

	QStringList lines;
	while (!textStream.atEnd())
		lines << textStream.readLine();

	const int TableRowCount = m_tableWidgetDict->rowCount();
	if (lines.count() != TableRowCount) {
		QMessageBox::critical(this, QString(), QString("Row count mismatch!\nTxt file: %1\nDict table: %2").arg(lines.count()).arg(TableRowCount));
		return;
	}

	for (int row = 0; row < TableRowCount; ++row) {
		QTableWidgetItem* col2 = m_tableWidgetDict->item(row, 2);
		QString           line(lines.at(row));
		Utils::restoreNewLine(line);
		col2->setText(line);
	}
}

void MainWindow::onClickedExportOnlineDictTemplate()
{
	const QString filepath = QFileDialog::getSaveFileName(this, QString(), "template.txt");
	if (filepath.isEmpty())
		return;

	// 生成txt
	QFile file(filepath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
		QMessageBox::critical(this, QString(), QString("Can not open %1").arg(filepath));
		return;
	}

	QTextStream textStream(&file);
	textStream.setCodec("UTF-8");

	const int TableRowCount = m_tableWidgetDict->rowCount();
	for (int row = 0; row < TableRowCount; ++row) {
		QString src = m_tableWidgetDict->item(row, 1)->text();
		Utils::removeNewLine(src);
		textStream << src << Qt::endl;
	}
}

void MainWindow::onClickedBrowseFixedDict()
{
	const QString t1 = m_lineEditFixedDict->text();
	const QString t2 = QFileDialog::getOpenFileName(this, QString(), t1, "JSON Files (*.json);;");
	if (!t2.isEmpty()) {
		m_lineEditFixedDict->setText(t2);
	}
}

void MainWindow::onClickedLoadFixedDict()
{
	m_tableWidgetFixedDict->clearContents();
	m_tableWidgetFixedDict->setRowCount(0);

	m_fixedDictManager->reset();
	m_fixedDict = m_fixedDictManager->dict(currentLanguage());

	QString filepath = m_lineEditFixedDict->text();
	if (filepath.isEmpty())
		return;

	QFile file(filepath);
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(this, QString(), QString("Failed to open %1").arg(filepath));
		return;
	}

	QJsonParseError     jError;
	const QJsonDocument jDoc = QJsonDocument::fromJson(file.readAll(), &jError);
	if (QJsonParseError::NoError != jError.error) {
		QMessageBox::critical(this, QString(), QString("Failed to parse json file %1, info=%2").arg(filepath, jError.errorString()));
		return;
	}

	const QJsonObject j = jDoc.object();

	m_fixedDictManager->load(j.value(KEY_FIXED_DICT).toObject());
	m_fixedDict = m_fixedDictManager->dict(currentLanguage());
	m_comboBoxFixedDictLanguage->clear();
	m_comboBoxFixedDictLanguage->addItems(m_fixedDictManager->languages());

	reloadTsTable();
}

void MainWindow::onClickedSaveFixedDit()
{
	QString filepath = m_lineEditFixedDict->text();
	if (filepath.isEmpty()) {
		filepath = QFileDialog::getSaveFileName(this, QString(), "fixed_dict.json");
		if (filepath.isEmpty()) // still invalid
			return;
		m_lineEditFixedDict->setText(filepath);
	} else {
		if (QMessageBox::No
		    == QMessageBox::question(this, QString(), QString("Are you sure to overwrite %1?").arg(filepath), QMessageBox::Yes | QMessageBox::No))
			return;
	}

	QFile file(filepath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
		QMessageBox::critical(this, QString(), QString("Can not open %1").arg(filepath));
		return;
	}

	QJsonObject jFixedDict;
	m_fixedDictManager->save(&jFixedDict);

	const QJsonObject j{{KEY_FIXED_DICT, jFixedDict}};

	if (file.write(QJsonDocument(j).toJson())) {
		QMessageBox::information(this, QString(), "Saved");
	} else {
		QMessageBox::critical(this, QString(), QString("Failed to save to %1").arg(filepath));
	}
}

void MainWindow::onClickedAddLanguage()
{
	const QString newLang = QInputDialog::getText(this, QString(), "Input the new language");
	if (newLang.isEmpty())
		return;

	if (m_fixedDictManager->addLanguage(newLang)) {
		m_fixedDict = m_fixedDictManager->dict(newLang);

		const int RowCount = m_comboBoxFixedDictLanguage->count();
		m_comboBoxFixedDictLanguage->addItem(newLang);
		m_comboBoxFixedDictLanguage->setCurrentIndex(RowCount);
	} else {
		QMessageBox::warning(this, QString(), "Failed to add language");
	}
}

void MainWindow::onClickedRemoveLanguage()
{
	const QString lang = m_comboBoxFixedDictLanguage->currentText();

	if (QMessageBox::No
	    == QMessageBox::question(this,
	                             QString(),
	                             QString("Are you sure to remove language: %1?\nThis operation cannot be undone.").arg(lang),
	                             QMessageBox::Yes | QMessageBox::No))
		return;

	if (m_fixedDictManager->removeLanguage(lang)) {
		m_fixedDict = m_fixedDictManager->dict(currentLanguage());

		const int idx = m_comboBoxFixedDictLanguage->currentIndex();
		m_comboBoxFixedDictLanguage->removeItem(idx);
	}
}

void MainWindow::onToggledIgnore()
{
	reloadTsTable();
	refreshFixedDictValid();
}

void MainWindow::reloadTsTable()
{
	//qDebug("reloadTsTable");
	m_tableWidgetDict->clearContents();
	m_tableWidgetDict->setRowCount(0);
	if (!m_tsManager)
		return;

	const bool ignoreFinished = m_checkBoxIgnoreFinished->isChecked();
	const bool ignoreObsolete = m_checkBoxIgnoreVanishedObsolete->isChecked();

	const QMap<QString, TSManager::Item> tsItems = m_tsManager->items();
	int                                  row     = 0;
	for (auto it = tsItems.constBegin(), itEnd = tsItems.constEnd(); it != itEnd; ++it) {
		const TSManager::Item& tsItem = it.value();

		if (ignoreFinished && tsItem.type == TSManager::Finished)
			continue;

		if (ignoreObsolete && (tsItem.type == TSManager::Vanished || tsItem.type == TSManager::Obsolete))
			continue;

		if (m_fixedDict.contains(tsItem.src))
			continue;

		m_tableWidgetDict->setRowCount(row + 1);

		setDictRow(row, tsItem.type, tsItem.src, tsItem.translation);

		++row;
	}
}

void MainWindow::refreshFixedDictValid()
{
	//qDebug("refreshFixedDictValid");
	const QMap<QString, TSManager::Item> tsItems = m_tsManager ? m_tsManager->items() : QMap<QString, TSManager::Item>();

	int RowCount = m_tableWidgetFixedDict->rowCount();
	for (int row = 0; row < RowCount; ++row) {
		QTableWidgetItem* col0     = m_tableWidgetFixedDict->item(row, 0);
		QTableWidgetItem* col1     = m_tableWidgetFixedDict->item(row, 1);

		const bool srcValid = tsItems.contains(col1->text());
		col0->setText(srcValid ? "Yes": QString());
	}
}

void MainWindow::moveDictToFixed()
{
	const auto srs = m_tableWidgetDict->selectedRanges();
	if (srs.isEmpty()) {
		QMessageBox::information(this, QString(), "Select more items and retry again");
		return;
	}

	int top    = srs.first().topRow();
	int bottom = srs.first().bottomRow();

	for (const QTableWidgetSelectionRange& sr : srs) {
		if (top > sr.topRow())
			top = sr.topRow();
		if (bottom < sr.bottomRow())
			bottom = sr.bottomRow();
	}

	//qDebug("Selected Top=%d, Bottom=%d", top, bottom);
	for (int i = top; i <= bottom; ++i) {
		const QString src = m_tableWidgetDict->item(top, 1)->text();
		const QString translation = m_tableWidgetDict->item(top, 2)->text();

		const int TableRowCount = m_tableWidgetFixedDict->rowCount();
		m_tableWidgetFixedDict->setRowCount(TableRowCount + 1);
		setFixedDictRow(TableRowCount, src, translation);

		m_fixedDictManager->setTransaltion(currentLanguage(), src, translation);
		m_fixedDict[src] = translation;

		m_tableWidgetDict->removeRow(top);
	}

	refreshFixedDictValid();
}

void MainWindow::moveFixedToDict()
{
	const auto srs = m_tableWidgetFixedDict->selectedRanges();
	if (srs.isEmpty()) {
		QMessageBox::information(this, QString(), "Select more items and retry again");
		return;
	}

	int top    = srs.first().topRow();
	int bottom = srs.first().bottomRow();

	for (const QTableWidgetSelectionRange& sr : srs) {
		if (top > sr.topRow())
			top = sr.topRow();
		if (bottom < sr.bottomRow())
			bottom = sr.bottomRow();
	}

	//qDebug("Selected Top=%d, Bottom=%d", top, bottom);
	if (!m_tsManager)
		return;

	const auto tsItems = m_tsManager->items();

	for (int i = top; i <= bottom; ++i) {
		const QString src = m_tableWidgetFixedDict->item(top, 1)->text();

		if (!tsItems.contains(src)) {
			++top; // skip
			continue;
		}

		const TSManager::Item& tsItem        = tsItems.value(src);
		const int              TableRowCount = m_tableWidgetDict->rowCount();
		m_tableWidgetDict->setRowCount(TableRowCount + 1);
		setDictRow(TableRowCount, tsItem.type, src, tsItem.translation);

		m_fixedDictManager->removeSrc(src);
		m_fixedDict.remove(src);

		m_tableWidgetFixedDict->removeRow(top);
	}
}

void MainWindow::clearFixedDict()
{
	m_tableWidgetFixedDict->clearContents();
	m_tableWidgetFixedDict->setRowCount(0);

	m_fixedDictManager->reset();
	m_fixedDict = m_fixedDictManager->dict(currentLanguage());

	reloadTsTable();
}

void MainWindow::onFixedDictCellChanged(int row, int col)
{
	if (2 == col) {
		const QString src = m_tableWidgetFixedDict->item(row, 1)->text();
		if (src.isEmpty())
			return;

		const QString transaltion = m_tableWidgetFixedDict->item(row, 2)->text();
		m_fixedDictManager->setTransaltion(currentLanguage(), src, transaltion);
		m_fixedDict[src]          = transaltion;

		//qDebug("change fixed: (%s) => (%s)", qPrintable(src), qPrintable(transaltion));
	}
}

void MainWindow::onSwitchToLanguage(const QString& lang)
{
	qDebug("onSwitchToLanguage=%s", qPrintable(lang));
	if (lang.isEmpty())
		return;

	m_fixedDict = m_fixedDictManager->dict(lang);

	int row = 0;
	for (auto it = m_fixedDict.constBegin(), itEnd = m_fixedDict.constEnd(); it != itEnd; ++it) {
		const QString src         = it.key();
		const QString translation = it.value();

		m_tableWidgetFixedDict->setRowCount(row + 1);
		setFixedDictRow(row, src, translation);

		++row;
	}

	refreshFixedDictValid();
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::ContextMenu) {
		QContextMenuEvent* cmEvent = static_cast<QContextMenuEvent*>(event);

		if (watched == m_tableWidgetDict) {
			QMenu menu;
			menu.addAction("Move rows to fixed dict", this, &MainWindow::moveDictToFixed);
			menu.exec(cmEvent->globalPos());
		} else if (watched == m_tableWidgetFixedDict) {
			QMenu menu;
			menu.addAction("Move rows to dict", this, &MainWindow::moveFixedToDict);
			menu.addAction("Clear all", this, &MainWindow::clearFixedDict);
			menu.exec(cmEvent->globalPos());
		}
	}

	return QWidget::eventFilter(watched, event);
}

void MainWindow::setDictRow(int row, int itemType, const QString& src, const QString& translation) {
	QTableWidgetItem* col[3] = {new QTableWidgetItem, new QTableWidgetItem, new QTableWidgetItem};
	switch (itemType) {
#define CASE(x) \
	case TSManager::x: \
		col[0]->setText(#x); \
		break;

		CASE(Finished)
		CASE(Vanished)
		CASE(Obsolete)
		CASE(Unfinished)
#undef CASE
	default:
		Q_ASSERT_X(false, "MainWindow::onClickedLoadTs()", "Invalid type of TSManger::Item");
		break;
	}

	col[1]->setText(src);
	col[2]->setText(translation);

	for (int i = 0; i < 3; ++i)
		m_tableWidgetDict->setItem(row, i, col[i]);
}

void MainWindow::setFixedDictRow(int row, const QString& src, const QString& translation)
{
	QTableWidgetItem* col[3] = {new QTableWidgetItem, new QTableWidgetItem, new QTableWidgetItem};
	for (int i = 0; i < 3; ++i)
		m_tableWidgetFixedDict->setItem(row, i, col[i]);

	auto setReadOnly = [](QTableWidgetItem* item) { item->setFlags(item->flags() & (~Qt::ItemIsEditable)); };

	setReadOnly(col[0]);
	setReadOnly(col[1]);

	col[1]->setText(src);
	col[2]->setText(translation);
}

QString MainWindow::currentLanguage() const
{
	return m_comboBoxFixedDictLanguage->currentText();
}
