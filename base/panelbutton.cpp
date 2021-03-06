﻿#include "panelbutton.h"

#include <QVBoxLayout>

using ltp::base::PanelButton;

PanelButton::PanelButton(QWidget *parent)
	: ClickDelayCheckButton(parent)
{
	this->setCheckable(true);

}

PanelButton::~PanelButton()
{

}

void PanelButton::setButtonPicture(const QString& defaultPix, const QString& hoverPix, const QString& checkedPix, const QString& pressedPix)
{
    // 设置按钮图片
	this->setStyleSheet(QString("QPushButton{background:#F0F0F0; font:12px SIMHEI; border:1px solid #D2D2D2; border-radius:2px; padding-bottom:3px; color:black;} \
						QPushButton{background-image:url(%1);background-repeat:no-repeat; background-position:top; text-align:bottom;}\
						QPushButton:checked{background:#0064C8; background-image:url(%3);background-repeat:no-repeat; background-position:top; text-align:bottom; color:white;} \
						QPushButton:pressed{background:#0080FF; background-image:url(%4);background-repeat:no-repeat; background-position:top; text-align:bottom; color:white;}")
						.arg(defaultPix).arg(checkedPix).arg(pressedPix));
	Q_UNUSED(hoverPix);
}

