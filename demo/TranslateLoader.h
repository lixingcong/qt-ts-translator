/*
 * TranslateLoader.h
 *
 * Brief: 功能简要概述
 *
 * Created on: 2026年 3月 5日
 *
 * Author: lixingcong
 */

#pragma once

#include <QStringList>

namespace TranslateLoader
{

QStringList supportedLanguages(); // 用于界面显示选项切换语言

void loadAtRun(); // 程序启动时加载语言

int  currentLanguage(); // 返回值为supportedLanguages的下标索引

/*! \brief 尝试加载语言
 *  \param idx 为supportedLanguages返回的数组下标索引
 *             若传入不合法的下标，则根据系统语言尝试加载
 *  \return 实际加载的语言索引
 */
int setLanguage(int idx);

void saveLanguage(); // 将当前语言保存到ini配置中

}
