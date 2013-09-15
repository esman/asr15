#include "timer.h"

namespace tmr
{

mseconds_t clock;

void Timer::Reset(mseconds_t duration)
{
  etime_   = clock + duration;
  running_ = true;
  expired_ = false;
}

void Timer::Stop()
{
  running_ = false;
}

bool Timer::Expired()
{
  auto result = false;

  if(running_)
  {
    if(expired_)
    {
      result = true;
    }
    else if(static_cast<int>(clock - etime_) >= 0)
    {
      expired_ = true;
      result = true;
    }
  }

  return result;
}

extern "C" void SysTick_Handler(void)
{
  clock++;
}

} /* namespace tmr */
