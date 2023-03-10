

#include <stdint.h>
#include <sys/time.h>

uint32_t 
custom_tick_get()
{
  static uint64_t start_ms = 0;
  if (start_ms == 0)
  {
    struct timeval tv_start;
    gettimeofday(&tv_start, 0);
    start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
  }

  struct timeval tv_now;
  gettimeofday(&tv_now, 0);
  uint64_t now_ms;
  now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

  uint32_t time_ms = now_ms - start_ms;
  return time_ms;
}

