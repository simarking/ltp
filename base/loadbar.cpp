﻿#include "loadbar.h"
#include <QDebug>

using ltp::base::LoadBar;

LoadBar::LoadBar(QWidget *parent)
	: QProgressBar(parent)
{
	this->setStyleSheet("QProgressBar{text-align:right; margin-right:40px; margin-top:3px; margin-bottom:3px; font:14px Arial;}");
	this->setFixedSize(146, 14);

	connect(this, SIGNAL(valueChanged(int)), this, SLOT(colorChanged(int)));
}

LoadBar::~LoadBar()
{
}

void LoadBar::colorChanged(int value)
{
	if (value < 40)			// 绿色范围
	{
		this->setStyleSheet("QProgressBar{text-align:right; margin-right:40px; margin-top:3px; margin-bottom:3px; font:14px Arial;} \
							 QProgressBar::chunk { background-color: #00AA3C }");
	} 
	else if (value >= 40 && value < 70)			// 黄色范围
	{
		this->setStyleSheet("QProgressBar{text-align:right; margin-right:40px; margin-top:3px; margin-bottom:3px; font:14px Arial;} \
							 QProgressBar::chunk { background-color: #FFD524 }");
	}
	else				// 红色警示范围
	{
		this->setStyleSheet("QProgressBar{text-align:right; margin-right:40px; margin-top:3px; margin-bottom:3px; font:14px Arial;} \
							 QProgressBar::chunk { background-color: #FA3200 }");
	}
}
