// Copyright (c) [2024] [Jovan J. E. Odassius]
//
// License: MIT (See the LICENSE file in the root directory)
// Github: https://github.com/untyper/spsc-queue

#ifndef SPSC_QUEUE_H
#define SPSC_QUEUE_H

#include <atomic>
#include <cstddef>

template<typename T, std::size_t N>
class SPSC_Queue
{
  static_assert((N& (N - 1)) == 0, "N must be power-of-two");
  alignas(64) std::atomic<std::size_t> head{ 0 }, tail{ 0 };
  T buffer[N];

public:
  bool push(const T& v)
  {
    auto t = tail.load(std::memory_order_relaxed);
    auto n = (t + 1) & (N - 1);

    if (n == head.load(std::memory_order_acquire))
    {
      return false; // Full
    }

    buffer[t] = v;
    tail.store(n, std::memory_order_release);
    return true;
  }

  bool pop(T& out)
  {
    auto h = head.load(std::memory_order_relaxed);

    if (h == tail.load(std::memory_order_acquire))
    {
      return false; // Empty
    }

    out = buffer[h];
    head.store((h + 1) & (N - 1), std::memory_order_release);
    return true;
  }
};

#endif // SPSC_QUEUE_H
