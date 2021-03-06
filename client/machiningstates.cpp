﻿#include "machiningstates.h"
#include <QTextCodec>
#include "network.h"
#include "base/systemvariables.hpp"
#include "remotevariables.hpp"
#include "physicalbuttonsprocessor.h"
#include "base/globals.h"

using ltp::client::MachiningStates;

const QString MachiningStates::ftpRootPath = "/home/Lynuc/Users/NCFiles";

MachiningStates::MachiningStates(QObject* parent)
	: QObject(parent),
	mode_(base::UNKNOWN_MODE),
	machiningState_(base::READY),
	errorLevel_(base::NO_ERROR),
	localMachiningFilePath_("./NCFiles/machining_file.nc")
{
	//状态刷新
	const int kInterval = 200;
	timer_.start(kInterval);
	connect(&timer_, SIGNAL(timeout()), SLOT(updateState()));
	//网络连接时，确认有效轴
	connect(&base::getInstance<Network>(), SIGNAL(connected(const std::string&)), SLOT(updateAxesInformation()));
	// 下排外设按钮响应
	connect(&base::getInstance<PhysicalButtonsProcessor>(), SIGNAL(bottomButtonModeClicked(int)), this, SLOT(modeChanged(int)));
	connect(&base::getInstance<PhysicalButtonsProcessor>(), SIGNAL(bottomButtonRateClicked(int)), this, SLOT(rateChanged(int)));
	//ftp传输连接
	connect(&base::getInstance<Network>(), SIGNAL(connected(const std::string&)), &ftpTransmissionManager_, SLOT(connect(const std::string&)));
	// 右侧按钮start,stop,reset
	connect(&base::getInstance<PhysicalButtonsProcessor>(),SIGNAL(rightButtonStart()), this, SLOT(startButtonClicked()));
	connect(&base::getInstance<PhysicalButtonsProcessor>(),SIGNAL(rightButtonStop()), this, SLOT(stopButtonClicked()));
	connect(&base::getInstance<PhysicalButtonsProcessor>(),SIGNAL(rightButtonReset()), this, SLOT(resetButtonClicked()));
	connect(&base::getInstance<PhysicalButtonsProcessor>(),SIGNAL(rightButtonStartRealease()), this, SLOT(startButtonRealeased()));
	connect(&base::getInstance<PhysicalButtonsProcessor>(),SIGNAL(rightButtonStopRealease()), this, SLOT(stopButtonRealeased()));
	connect(&base::getInstance<PhysicalButtonsProcessor>(),SIGNAL(rightButtonResetRealease()), this, SLOT(resetButtonRealeased()));
}

MachiningStates::~MachiningStates()
{

}

void MachiningStates::rateChanged(int buttonID)
{
	if (mode() == base::JOG)			// JOG倍率修改
	{
		switch (buttonID)
		{
		case base::BOTTOMBUTTON6:			// X1 5%
			base::getInstance<Network>().setPlcVariable(rmi::G_JOVRD, 0);
			break;
		case base::BOTTOMBUTTON7:			// X10 25%
			base::getInstance<Network>().setPlcVariable(rmi::G_JOVRD, 1);
			break;
		case base::BOTTOMBUTTON8:			// X100 50%
			base::getInstance<Network>().setPlcVariable(rmi::G_JOVRD, 2);
			break;
		case base::BOTTOMBUTTON9:			// X200 100%
			base::getInstance<Network>().setPlcVariable(rmi::G_JOVRD, 3);
			break;
		default:
			break;
		}
	}
	else if (mode() == base::HANDLE)	// 手轮倍率修改
	{
		switch (buttonID)
		{
		case base::BOTTOMBUTTON6:			// X1 5%
			base::getInstance<Network>().setPlcVariable(rmi::G_HWOVRD, 0);
			break;
		case base::BOTTOMBUTTON7:			// X10 25%
			base::getInstance<Network>().setPlcVariable(rmi::G_HWOVRD, 1);
			break;
		case base::BOTTOMBUTTON8:			// X100 50%
			base::getInstance<Network>().setPlcVariable(rmi::G_HWOVRD, 2);
			break;
		case base::BOTTOMBUTTON9:			// X200 100%
			base::getInstance<Network>().setPlcVariable(rmi::G_HWOVRD, 3);
			break;
		default:
			break;
		}
	}
}

void MachiningStates::modeChanged(int buttonID)
{
	// 发送PLC信号修改当前模式
	switch (buttonID)
	{
	case base::BOTTOMBUTTON1:			// 自动
		base::getInstance<Network>().setPlcVariable(rmi::G_MODE, base::MEMORY);
		break;
	case base::BOTTOMBUTTON2:			// JOG
		base::getInstance<Network>().setPlcVariable(rmi::G_MODE, base::JOG);
		break;
	case base::BOTTOMBUTTON3:			// 手轮
		base::getInstance<Network>().setPlcVariable(rmi::G_MODE, base::HANDLE);
		break;
	default:
		break;
	}
}

QString MachiningStates::filePathToFtpPath(const QString& filePath)
{
	if (filePath.indexOf(ftpRootPath) == 0)
	{
		return filePath.mid(ftpRootPath.length() + 1);
	}
	return "";
}

QString MachiningStates::ftpPathToFilePath(const QString& ftpPath)
{
	return ftpRootPath + ftpPath;
}

ltp::base::Mode MachiningStates::mode() const
{
	return static_cast<base::Mode>(static_cast<int>(
		base::getInstance<base::SystemVariables<RemoteVariables>>().macroVariable(base::SYSTEM_MODE)));
}

ltp::base::MachiningState MachiningStates::machiningState() const
{
	auto& systemVariables = base::getInstance<base::SystemVariables<RemoteVariables>>();
	//PLC中就绪标志
	if (systemVariables.plcVariable(rmi::F_READY) != 0)
	{
		return base::READY;
	}
	//PLC中警告标志
	if (systemVariables.plcVariable(rmi::F_HOLDING) != 0)
	{
		return base::HOLD;
	}
	//PLC中运行标志
	if (systemVariables.plcVariable(rmi::F_RUNNING) != 0)
	{
		return base::BUSY;
	}
	//默认为停止状态
	return base::STOP;
}

QString MachiningStates::machiningFilePath() const
{
	//远程接口传输使用utf-8
	QTextCodec* codec = QTextCodec::codecForName("utf8");
	return codec->toUnicode(base::getInstance<Network>().openedFilePath().c_str());
}

QString MachiningStates::machiningFileName() const
{
	//远程接口传输使用utf-8
	QTextCodec* codec = QTextCodec::codecForName("utf8");
	return codec->toUnicode(base::getInstance<Network>().openedFileName().c_str());
}

int MachiningStates::machiningFileLastModifiedTime() const
{
	return base::getInstance<Network>().fileLastModifiedTime(machiningFilePath().toStdString());
}

ltp::base::ErrorLevel MachiningStates::errorLevel() const
{
	auto& systemVariables = base::getInstance<base::SystemVariables<RemoteVariables>>();
	//CI错误和致命错误，表示有报错
	if (systemVariables.plcVariable(rmi::F_CIERR) != 0 ||
		systemVariables.plcVariable(rmi::F_FATALERR) != 0)
	{
		return base::ERROR;
	}
	//PLC的警告标志位
	else if (systemVariables.plcVariable(rmi::F_WARN) != 0)
	{
		return base::WARNNING;
	}
	return base::NO_ERROR;
}

const std::vector<ltp::base::Axis>& ltp::client::MachiningStates::validAxes() const
{
	return validAxes_;
}

int ltp::client::MachiningStates::axisEnumToAxisAddress(base::Axis axisEnum) const
{
	return axesAddress_.at(axisEnum);
}

void ltp::client::MachiningStates::updateMachiningFile()
{
	ftpTransmissionManager_.downloadFile(filePathToFtpPath(machiningFilePath_), localMachiningFilePath_);
	emit localMachiningFileChanged(localMachiningFilePath_);
}

int ltp::client::MachiningStates::mainProgramLineNumber()
{
	if (base::getInstance<base::SystemVariables<RemoteVariables>>().macroVariable(base::PROGRAM_NUMBER) == 0)
	{
		return base::getInstance<base::SystemVariables<RemoteVariables>>().macroVariable(base::CURRENT_PROGRAM_LINE_NUMBER);
	}
	return mainProgramLineNumber_;
}

void MachiningStates::remoteOpenFile(const QString& localFilePath, const QString& remoteFilePath)
{
	//文件路径转换为ftp文件路径
	auto remoteFtpFilePath = filePathToFtpPath(remoteFilePath);
	//上传文件
	ftpTransmissionManager_.uploadFile(localFilePath, remoteFtpFilePath);
	//远程打开
	base::getInstance<Network>().openFile(1, remoteFtpFilePath.toUtf8().data());
}

void ltp::client::MachiningStates::remoteOpenFtpFile(const QString remoteFtpFilePath)
{
	//远程打开
	base::getInstance<Network>().openFile(1, remoteFtpFilePath.toUtf8().data());
}

void MachiningStates::updateState()
{
	//模式
	auto remoteMode = mode();
	if (mode_ != remoteMode)
	{
		mode_ = remoteMode;
		emit modeChanged(mode_);
	}
	//状态
	auto remoteMachiningState = machiningState();
	if (machiningState_ != remoteMachiningState)
	{
		machiningState_ = remoteMachiningState;
		emit machiningStateChanged(machiningState_);
	}
	//报错等级
	auto remoteErrorLevel = errorLevel();
	if (errorLevel_ != remoteErrorLevel)
	{
		errorLevel_ = remoteErrorLevel;
		emit errorLevelChanged(errorLevel_);
	}
	//当前加载的文件名和修改时间
	auto currentFilePath = machiningFilePath();
	auto currentFileLastModifiedTime = machiningFileLastModifiedTime();
	//如果出现变化
	if ((currentFilePath.length() != 0 && currentFilePath != machiningFilePath_) ||
		(currentFileLastModifiedTime != 0 && currentFileLastModifiedTime != machiningFileLastModifiedTime_))
	{
		if (currentFilePath != machiningFilePath_)
		{
			machiningFilePath_ = currentFilePath;
		}
		if (currentFileLastModifiedTime != machiningFileLastModifiedTime_)
		{
			machiningFileLastModifiedTime_ = currentFileLastModifiedTime;
		}
		updateMachiningFile();
		emit machiningFileChanged(machiningFilePath_);
	}
	//当前加工行号
	auto currentMainProgramLineNumber = mainProgramLineNumber();
	if (currentMainProgramLineNumber != mainProgramLineNumber_)
	{
		mainProgramLineNumber_ = currentMainProgramLineNumber;
		emit mainProgramLineNumberChanged(mainProgramLineNumber_);
	}
}

void MachiningStates::updateAxesInformation()
{
	auto& systemVariables = base::getInstance<base::SystemVariables<RemoteVariables>>();
	//控制器上的有效轴
	auto remoteValidAxes = base::axesCharToAxesEnum(systemVariables.validFeedAxes());
	//对比有效轴
	if (validAxes_ != remoteValidAxes)
	{
		validAxes_ = std::move(remoteValidAxes);
		emit validAxesChanged(validAxes_);
	}
	//更新轴地址
	auto remoteAxesAddress = systemVariables.axesAddress();
	axesAddress_.clear();
	for (auto it = remoteAxesAddress.begin(); it != remoteAxesAddress.end(); it++)
	{
		axesAddress_[base::axisCharToAxisEnum(it->first)] = it->second;
	}
}

void MachiningStates::startButtonClicked()
{
	// 启动按钮按下
	base::getInstance<Network>().setPlcVariable(rmi::G_CYCLESTART, 1);
}

void MachiningStates::startButtonRealeased()
{
	// 启动按钮松开
	base::getInstance<Network>().setPlcVariable(rmi::G_CYCLESTART, 0);
}

void MachiningStates::stopButtonClicked()
{
	// 停止按钮按下
	base::getInstance<Network>().setPlcVariable(rmi::G_PROGSTOP, 1);
}

void MachiningStates::stopButtonRealeased()
{
	// 停止按钮松开
	base::getInstance<Network>().setPlcVariable(rmi::G_PROGSTOP, 0);
}

void MachiningStates::resetButtonClicked()
{
	// 重置按钮按下
	base::getInstance<Network>().setPlcVariable(rmi::G_FRESET, 1);
}

void MachiningStates::resetButtonRealeased()
{
	// 重置按钮松开
	base::getInstance<Network>().setPlcVariable(rmi::G_FRESET, 0);
}