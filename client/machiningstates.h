﻿#ifndef LTP_CLIENT_MACHININGSTATES_H_
#define LTP_CLIENT_MACHININGSTATES_H_

#include <vector>
#include <unordered_map>
#include <QString>
#include <QObject>
#include <QTimer>
#include "base/globals.h"
#include "base/singleton.hpp"
#include "base/ftptransmissionmanager.h"

namespace ltp
{
	namespace client
	{
		class MachiningStates : public QObject
		{
			Q_OBJECT
			friend MachiningStates& base::getInstance<MachiningStates>();

		public:
			//将文件路径转换为ftp路径
			static QString filePathToFtpPath(const QString& filePath);
			//将ftp路径转换为文件路径
			static QString ftpPathToFilePath(const QString& ftpPath);
			//ftp访问的路径
			static const QString ftpRootPath;
			//获取远程模式
			base::Mode mode() const;
			//获取远程加工状态
			base::MachiningState machiningState() const;
			//获取远程加载的文件名(包含路径)
			QString machiningFilePath() const;
			//获取远程加载的文件名(不包含路径)
			QString machiningFileName() const;
			//获取远程加载的文件最后修改时间
			int machiningFileLastModifiedTime() const;
			//获取报警等级
			base::ErrorLevel errorLevel() const;
			//获取有效轴
			const std::vector<base::Axis>& validAxes() const;
			//根据轴枚举获取轴地址
			int axisEnumToAxisAddress(base::Axis axisEnum) const;
			//更新正在加工的文件
			void updateMachiningFile();
			//当前主程序加工行号
			int mainProgramLineNumber();

		public slots:
			//远程打开文件
			void remoteOpenFile(const QString& localFilePath, const QString& remoteFilePath);
			//远程打开ftp文件
			void remoteOpenFtpFile(const QString remoteFtpFilePath);

		signals:
			//检测到模式切换时发出信号
			void modeChanged(base::Mode mode);
			//检测到加工状态切换发出信号
			void machiningStateChanged(base::MachiningState machiningState);
			//报错等级变化信号
			void errorLevelChanged(base::ErrorLevel errorLevel);
			//检测到正在加工的文件变化
			void machiningFileChanged(const QString& filePath);
			//本地正在加工的文件变化
			void localMachiningFileChanged(const QString& filePath);
			//有效轴变化信号
			void validAxesChanged(const std::vector<base::Axis>& validAxes);
			//当前加工行号变化
			void mainProgramLineNumberChanged(int lineNumber);

		private:
			//作为单例使用
			MachiningStates(QObject* parent = nullptr);
			~MachiningStates();
			base::Mode mode_;
			base::MachiningState machiningState_;
			base::ErrorLevel errorLevel_;
			//有效轴
			std::vector<base::Axis> validAxes_;
			//更新状态使用的定时器
			QTimer timer_;
			//轴枚举到轴地址的映射
			std::unordered_map<base::Axis, int> axesAddress_;
			//当前加载文件名
			QString machiningFilePath_;
			//当前加载文件在控制器上的最后修改时间
			int machiningFileLastModifiedTime_;
			//当前加载文件的本地文件名
			const QString localMachiningFilePath_;
			//上传和下载文件
			base::FtpTransmissionManager ftpTransmissionManager_;
			//当前主程序加工行号
			int mainProgramLineNumber_;

		private slots:
			//更新状态，确定是否变化，发送相应信号
			void updateState();
			//网络连接成功，更新轴信息
			void updateAxesInformation();
			// 模式切换
			void modeChanged(int);
			// 倍率变化
			void rateChanged(int);
			// 右侧按钮start,stop,reset
			void startButtonClicked();
			void stopButtonClicked();
			void resetButtonClicked();
			void startButtonRealeased();
			void stopButtonRealeased();
			void resetButtonRealeased();
		};
	}
}

#endif // LTP_CLIENT_MACHININGSTATES_H_