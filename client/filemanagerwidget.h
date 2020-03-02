﻿#ifndef LTP_CLIENT_FILEMANAGERWIDGET_H_
#define LTP_CLIENT_FILEMANAGERWIDGET_H_

#include <QtGui/QWidget>
#include "ui_filemanagerwidget.h"

namespace ltp
{
	namespace client
	{

		class FileManagerWidget : public QWidget
		{
			Q_OBJECT
		 
		public:
			FileManagerWidget(QWidget *parent = 0);
			~FileManagerWidget();
			void onStartPage();

		private:
			Ui::FileManagerWidgetClass ui;

		private slots:
			void onFileManagerModule(int);

		signals:
			void signalTips(QString);					// 提示栏信息
			void onHome();								// 返回主页
			void onProcessFile(QString filename);
			void onOpenFile(QString filename);
		};
	}
}
#endif // LTP_CLIENT_FILEMANAGERWIDGET_H_