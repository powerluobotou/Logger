#pragma once

#include "../Macro.h"

#include "mutex.h"

namespace utils {
	class Mutex;
	class Cond {
	public:
		Cond();
		~Cond();
		void wait(Mutex& mutex);
		void wait(Mutex& mutex, double seconds);
		void notify();
		void notifyAll();
	private:
		pthread_cond_t cond_;
	};
}