#pragma once
#include "Arduino.h"

#include "globals.h"
#include "timeoutInterval.h"
namespace TimerTimeout
{
	struct TimeoutInterval_TCC: public TimeoutInterval{
		Tcc* tc;
		bool isOneShot;

		TimeoutInterval_TCC(int _tcIdx);
		void enable() override;

		void start(bool oneshot, int top) override;

		int getValue() override;

		void setValue(int value) override;

		void stop() override;

		void restart() override;

		void runISR() override;
	};
}
