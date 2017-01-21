
#include "NvStopWatch.h"

#include <windows.h>  
#include <stdio.h>  
#include <Mmsystem.h>

#pragma comment(lib, "Winmm.lib")

namespace jet
{
	namespace util
	{
		class NvWin32StopWatch : public NvStopWatch
		{
		public:
			//! Constructor, default
			NvWin32StopWatch() :
				start_time(), end_time(),
				diff_time(0.0), freq_set(false)
			{
				if (!freq_set) {
					// helper variable
					LARGE_INTEGER temp;

					// get the tick frequency from the OS
					QueryPerformanceFrequency((LARGE_INTEGER*)&temp);

					// convert to type in which it is needed
					freq = ((double)temp.QuadPart);

					// rememeber query
					freq_set = true;
				}
			};

			// Destructor
			~NvWin32StopWatch()
			{ };

			//! Start time measurement
			void start() {
				QueryPerformanceCounter((LARGE_INTEGER*)&start_time);
				m_running = true;
			}

			//! Stop time measurement
			void stop() {
				QueryPerformanceCounter((LARGE_INTEGER*)&end_time);
				diff_time = (float)
					(((double)end_time.QuadPart - (double)start_time.QuadPart) / freq);
				m_running = false;
			}

			//! Reset time counters to zero
			void reset()
			{
				diff_time = 0;
				if (m_running)
					QueryPerformanceCounter((LARGE_INTEGER*)&start_time);
			}

			const float getTime() const {
				if (m_running) {
					return getDiffTime();
				}
				else {
					// time difference in seconds
					return diff_time;
				}
			}

		private:

			// helper functions

			//! Get difference between start time and current time
			float getDiffTime() const {
				LARGE_INTEGER temp;
				QueryPerformanceCounter((LARGE_INTEGER*)&temp);
				return (float)
					(((double)temp.QuadPart - (double)start_time.QuadPart) / freq);
			}

			// member variables

			//! Start of measurement
			LARGE_INTEGER  start_time;
			//! End of measurement
			LARGE_INTEGER  end_time;

			//! Time difference between the last start and stop
			float  diff_time;

			//! tick frequency
			double  freq;

			//! flag if the frequency has been set
			bool  freq_set;
		};

		NvStopWatch* NvStopWatch::create()
		{
			return new NvWin32StopWatch();
		}
	}
}