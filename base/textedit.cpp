﻿#include "textedit.h"
#include <QTextCodec>
#include <QTextBlock>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QAbstractButton>
#include <QPushButton>
#include <QDebug>
#include <QScrollBar>
#include <QAbstractTextDocumentLayout>
#include "base/messagebox.h"

using ltp::base::TextEdit;

TextEdit::TextEdit(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	lineTotalNum_ = 1;

	focusLine(1);
	// 光标位置行高亮显示
	connect(ui.insideTextEdit_, SIGNAL(cursorPositionChanged()), this, SLOT(onCurrentLineHighLight()));
	connect(ui.insideTextEdit_, SIGNAL(cursorPositionChanged()), this, SLOT(cursorChanged()));
}

TextEdit::~TextEdit()
{
}

void TextEdit::cursorChanged()
{
	// 总行号更新
	lineTotalNum_ = ui.insideTextEdit_->document()->blockCount();
	// 当前行/总行数 label修改
	int currentLine = ui.insideTextEdit_->textCursor().blockNumber() + 1;
	double percent  = currentLine * 1.0 / lineTotalNum_ * 100;
	ui.cursorPosition_->setText(QString("%1/%2(%3%)").arg(currentLine).arg(lineTotalNum_).arg(QString::number(percent, 'f', 2)));
}

void TextEdit::onCurrentLineHighLight()
{
	QList<QTextEdit::ExtraSelection> extraSelection;
	QTextEdit::ExtraSelection selection;

	// 选中行颜色属性设置
	QColor lineColor = QColor(205, 225, 245);
	selection.format.setBackground(lineColor);
	selection.format.setProperty(QTextFormat::FullWidthSelection, true);
	selection.cursor = ui.insideTextEdit_->textCursor();		// 当前光标
	selection.cursor.clearSelection();

	// 设置的selection作为当前行选中
	extraSelection.append(selection);
	ui.insideTextEdit_->setExtraSelections(extraSelection);
}

void TextEdit::focusLine(int lineNum)
{
	QTextBlock block = ui.insideTextEdit_->document()->findBlockByNumber(lineNum);
	QList<QTextEdit::ExtraSelection> extraSelection;
	QTextEdit::ExtraSelection selection;

	// 选中行颜色属性设置
	QColor lineColor = QColor(205, 225, 245);
	selection.format.setBackground(lineColor);
	selection.format.setProperty(QTextFormat::FullWidthSelection, true);
	selection.cursor = QTextCursor(block);
	ui.insideTextEdit_->setTextCursor(selection.cursor);
	// 设定当前选中位置位于文本框中央
	auto position = ui.insideTextEdit_->document()->documentLayout()->blockBoundingRect(block).toRect();
	ui.insideTextEdit_->verticalScrollBar()->setValue(position.center().y() - ui.insideTextEdit_->height() / 2);

	// 设置的selection作为当前行选中
	extraSelection.append(selection);
	ui.insideTextEdit_->setExtraSelections(extraSelection);
}

bool TextEdit::save()
{
	if (filePath_.isNull() || filePath_.isEmpty())		// 当前文件名是空
	{
		// error报警 
		emit signalTips(QString(tr("无法保存未命名文件")));
		return false;
	}

	// 保存当前打开文件
	QFile file(filePath_);
	if(!file.open(QFile::WriteOnly | QFile::Text))			// 打开权限
	{
		// error报警：无法读取文件
		emit signalTips(QString(tr("错误：无法写入文件")));
		return false;
	}

	QTextStream out(&file);
	out.setCodec(QTextCodec::codecForName("gbk"));
    out << ui.insideTextEdit_->toPlainText();
	out.flush();
	file.close();
    ui.insideTextEdit_->document()->setModified(false);
	// 提示保存成功
	emit signalTips(QString(tr("文件保存成功！")));
	emit signalSaved(filePath_);
	return true;
}

void TextEdit::saveAs(const QString& filePath)
{
	Q_UNUSED(filePath);
}

void TextEdit::removeLine()
{
	QTextCursor lineCursor = ui.insideTextEdit_->textCursor();		// 光标所在位置
	lineCursor.select(QTextCursor::BlockUnderCursor);				// 设置为整行选中
	ui.insideTextEdit_->setTextCursor(lineCursor);
	ui.insideTextEdit_->textCursor().removeSelectedText();			// 选中进行删除操作
}

void TextEdit::copyText()
{
	ui.insideTextEdit_->copy();		// 复制功能
}

void TextEdit::pasteText()
{
	ui.insideTextEdit_->paste();	// 粘贴功能
}

void TextEdit::cutText()
{
	ui.insideTextEdit_->cut();		// 剪切功能
}

void TextEdit::recoverText()
{
	ui.insideTextEdit_->redo();		// 撤销上一编辑
}

void TextEdit::repealText()
{
	ui.insideTextEdit_->undo();		// 恢复撤销
}

void ltp::base::TextEdit::insertPlainText(const QString& text)
{
	ui.insideTextEdit_->insertPlainText(text);
}

//读取文件
bool TextEdit::loadFile(const QString& filePath)
{
	QFile file(filePath);
	if(!file.open(QFile::ReadOnly | QFile::Text))
	{
		// error报警：无法读取文件
		emit signalTips(QString(tr("错误：无法读取文件")));
		return false;
	}

	QFileInfo fileinfo(filePath);
	if (fileinfo.size() > 1024 * 1024)
	{	
		// error报警：文件大小超过1MB, 无法读取文件!
		emit signalTips(tr("错误：文件大小超过1MB, 无法读取文件!"));
		return false;
	}
	QTextStream in(&file);
	//控制器上NC文件默认使用gbk
	in.setCodec(QTextCodec::codecForName("gbk"));
	ui.insideTextEdit_->setPlainText(in.readAll());

	fileName_ = fileinfo.filePath();
	filePath_ = filePath;

	lineTotalNum_ = ui.insideTextEdit_->document()->blockCount();

	file.close();

	// 更新行号
	cursorChanged();
	return true;
}

bool TextEdit::openFile(const QString& openFileName)
{
	// 检查是否修改
	auto result = checkModified();
	if (result == Yes)
	{
		save();
	}
	else if (result == Cancel)
	{
		return false;
	}

	if (openFileName.isNull() || openFileName.isEmpty())		// 文件名是空
	{
		//error报警:未能打开文件
		emit signalTips(QString(tr("错误：未能打开文件")));
		return false;
	}

	loadFile(openFileName);
	return true;
}

QString TextEdit::getCurrentFileName()
{
	return fileName_;
}

bool TextEdit::closeFile()
{
	// 检查是否修改
	auto result = checkModified();
	if (result == Yes)
	{
		save();
	}
	else if (result == Cancel)
	{
		return false;
	}
	//文件关闭信号
	emit signalClosed(filePath_);
	// 清除当前文件名
	fileName_.clear();
	filePath_.clear();
	// 清空文件显示
	ui.insideTextEdit_->clear();
	return true;
}

TextEdit::SaveFile TextEdit::checkModified()
{
	if(ui.insideTextEdit_->document()->isModified())		// 当前文件修改了
	{
		// 提示是否保存
		MessageBox box;
		box.setTitleName(QObject::tr("提示"));
		QPushButton* confirmButton = box.addButton(tr("确定"), QMessageBox::YesRole);
		QPushButton* noButton = box.addButton(tr("否"), QMessageBox::NoRole);
		QPushButton* cancelButton = box.addButton(tr("取消"), QMessageBox::RejectRole);
		box.setContentWidget(std::shared_ptr<QWidget>(new QLabel(tr("当前文件尚未保存，是否保存？"))));
		box.exec();
		if (box.clickedButton() == confirmButton)
		{
			return Yes;
		}
		else if (box.clickedButton() == noButton)
		{
			return No;
		}
		else if (box.clickedButton() == cancelButton || box.clickedButton() == nullptr)
		{
			return Cancel;
		}
	}
	return No;
}

void ltp::base::TextEdit::gotoLineBegin()
{
	ui.insideTextEdit_->moveCursor(QTextCursor::StartOfLine);
}
