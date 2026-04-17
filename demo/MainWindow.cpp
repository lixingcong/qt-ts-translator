/*
 * MainWindow.cpp
 *
 * Brief: 功能简要概述
 *
 * Created on: 2026年 4月 17日
 *
 * Author: lixingcong
 */

#include "MainWindow.h"
#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>
#include <QMessageBox>

#include "TranslateLoader.h"

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
    , m_btn(new QPushButton)
    , m_label(new QLabel)
{
	auto mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(m_btn);
	mainLayout->addWidget(m_label);

	retranslateText();

	setMinimumSize(266, 99);

	connect(m_btn, &QPushButton::clicked, this, &MainWindow::onClicked);
}

void MainWindow::onClicked()
{
	const QStringList supported  = TranslateLoader::supportedLanguages();
	const int         currentIdx = TranslateLoader::currentLanguage();
	const int         nextIdx    = currentIdx < supported.count() - 1 ? currentIdx + 1 : 0;

	QMessageBox::information(this, QString(), tr("Current language: %1. Next language: %2").arg(supported.at(currentIdx), supported.at(nextIdx)));

	TranslateLoader::setLanguage(nextIdx);
	TranslateLoader::saveLanguage();

	retranslateText();
}

void MainWindow::retranslateText()
{
	m_btn->setText(tr("Next language"));
	m_label->setText(tr("This is a long text\nIt contains a newline"));
}
