#include "ftptransmissionmanager.h"
#include <QUrl>

using ltp::base::FtpTransmissionManager;

void ltp::base::FtpTransmissionManager::connect()
{
	ftp_.reset(new QFtp());
	QObject::connect(ftp_.get(), SIGNAL(commandFinished(int, bool)), this, SLOT(ftpCommandFinished(int, bool)));
	//connect(ftp_.get(), SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(updateDataTransferProgress(qint64, qint64)));

	QUrl url(QString::fromStdString(ip_));
	ftp_->connectToHost(url.toString(), 21);
	ftp_->login();
}

bool FtpTransmissionManager::downloadFile(const QString& remoteFilePath, const QString& localFilePath)
{
	file_.reset(new QFile(localFilePath));
	//非只读删除文件
	if (!file_->open(QIODevice::WriteOnly))
	{
		return false;
	}
	//启动下载
	ftp_->get(remoteFilePath, file_.get());
	//弹出模态进度框，等待下载完成
	progressDialog_->exec();
	return true;
}

bool FtpTransmissionManager::uploadFile(const QString& localFilePath, const QString& remoteFilePath)
{
	file_.reset(new QFile(localFilePath));
	//上传只需要读文件
	if (!file_->open(QIODevice::ReadOnly))
	{
		return false;
	}
	//启动下载
	ftp_->put(file_.get(), remoteFilePath);
	//弹出模态进度框，等待下载完成
	progressDialog_->exec();
	return true;
}

FtpTransmissionManager::FtpTransmissionManager()
{
	progressDialog_ = new QDialog();
	progressDialog_->setModal(true);
}

FtpTransmissionManager::~FtpTransmissionManager()
{
}

void FtpTransmissionManager::ftpCommandFinished(int, bool error)
{
	switch (ftp_->currentCommand())
	{
	//连接
	case QFtp::ConnectToHost:
		{
			if (error)
			{
				//连接失败
				emit connectFail();
			}
			else
			{
				//连接成功
				emit connectSuccess();
			}
			return;
		}
		break;
	//下载
	case QFtp::Get:
		{
			if (error)
			{
				//关闭、删除文件
				file_->close();
				file_->remove();
				progressDialog_->reject();
			}
			else
			{
				//下载成功，仅关闭文件
				file_->close();
				progressDialog_->accept();
			}
		}
		break;
	//上传
	case QFtp::Put:
	{
		if (error)
		{
			//关闭、删除文件
			file_->close();
			progressDialog_->reject();
		}
		else
		{
			//下载成功，仅关闭文件
			file_->close();
			progressDialog_->accept();
		}
	}
	break;
	default:
		break;
	}
}