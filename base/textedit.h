﻿#ifndef LTP_BASE_TEXTEDIT_H_
#define LTP_BASE_TEXTEDIT_H_

#include <QtGui/QWidget>
#include "ui_textedit.h"

namespace ltp
{
	namespace base
	{
		class TextEdit : public QWidget
		{
			Q_OBJECT
		 
		public:
			TextEdit(QWidget *parent = 0);
			~TextEdit();  
			QString getCurrentFileName();			// 获取当前打开文件名
			
			//设置不可编辑
			void setReadOnly(bool readOnly)
			{
				ui.insideTextEdit_->setReadOnly(readOnly);
			}

		public slots:
			void gotoLineBegin();					// 定位到行首
			void insertPlainText(const QString& text);	// 插入文字

		private:
			enum SaveFile
			{
				Yes,
				No,
				Cancel
			};
			bool loadFile(const QString& filePath);	// 加载文件
			SaveFile checkModified();				// 检查文件是否修改
			Ui::TextEditClass ui;
			QString fileName_;						// 当前文件名
			QString filePath_;						// 当前文件名(含路径)
			int lineTotalNum_;						// 打开文件的总行数

		private slots:
			void onCurrentLineHighLight();			// 当前行高亮
			void cursorChanged();					// 行号信息修改
			void focusLine(int lineNum);

		public slots:
			bool save();
			void saveAs(const QString& filePath);
			void removeLine();						// 删除行
			void copyText();						// 拷贝
			void pasteText();						// 复制
			void cutText();							// 剪切 
			void recoverText();						// 撤销
			void repealText();						// 恢复
			bool openFile(const QString& filePath);	// 打开文件
			bool closeFile();						// 关闭文件

		signals:
			void signalTips(QString);
			void signalSaved(QString filePath);
			void signalClosed(QString filePath);
		};
	}
}


#endif // LTP_BASE_TEXTEDIT_H_