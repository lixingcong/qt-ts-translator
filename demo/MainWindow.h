/*
 * MainWindow.h
 *
 * Brief: 功能简要概述
 *
 * Created on: 2026年 4月 17日
 *
 * Author: lixingcong
 */

#pragma once

#include <QWidget>

class QLabel;
class QPushButton;

class MainWindow : public QWidget
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget* parent = nullptr);

protected Q_SLOTS:
	void onClicked();

protected:
	void retranslateText();

protected:
	QPushButton* const m_btn;
	QLabel* const      m_label;
};
