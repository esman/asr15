#ifndef TIMER_H_
#define TIMER_H_

#include <cstdint>

namespace tmr
{

typedef uint32_t mseconds_t;

class Timer
{
public:
  void Reset(mseconds_t duration);
  void Stop();
  bool Running() { return running_; }
  bool Expired();

private:
  mseconds_t etime_{};
  bool running_{};
  bool expired_{};
};

} /* namespace tmr */


#endif /* TIMER_H_ */
