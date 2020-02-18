﻿namespace ltp
{
	namespace base
	{
		//模式枚举
		enum Mode
		{
			CODELESS,
			MEMORY,
			REMOTE,
			HANDLE,
			JOG,
			HOME,
			DRNC,
			MDI
		};
		//轴相关数据存储下标
		enum Axis
		{
			X_AXIS,
			Y_AXIS,
			Z_AXIS,
			A_AXIS,
			B_AXIS,
			C_AXIS,
			U_AXIS,
			V_AXIS,
			W_AXIS,
			AXIS_COUNT = 9,
			AXIS_MAX = 16
		};
		//加工状态
		enum MachiningState
		{
			READY,
			BUSY,
			HOLD,
			STOP
		};
		//坐标类型（机械、绝对、增量）
		enum CoordinateType
		{
			MECHANICAL,
			ABSOLUTE,
			INBREMENTAL
		};
	}
}