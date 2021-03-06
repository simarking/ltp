﻿#ifndef LTP_BASE_PUSHBUTTON_H_
#define LTP_BASE_PUSHBUTTON_H_

#include <QtGui/QPushButton>

namespace ltp
{
	namespace base
	{
		class PushButton : public QPushButton
		{
			Q_OBJECT
		 
		public:
			PushButton(QWidget *parent = 0);
			~PushButton(); 
		};

		class DialogButton : public QPushButton
		{
			Q_OBJECT

		public:
			DialogButton(QWidget *parent = 0);
			~DialogButton(); 
		};
	}
}


#endif // LTP_BASE_PUSHBUTTON_H_