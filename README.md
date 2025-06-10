# SPSC Queue

A compact, header-only **single-producer / single-consumer (SPSC) ring buffer** for lock-free communication between exactly one writer thread and one reader thread.

* **Header-only** – just drop `spsc_queue.h` into your project
* **Wait-free push / pop** – O(1) operations using `std::atomic`
* **Cache-friendly** – head & tail are 64-byte aligned to reduce false sharing
* **Compile-time size** – buffer length `N` chosen at compile time (must be a power of two)

## Requirements

* C++11 or newer
* A platform with 64-byte cache lines for best performance (not mandatory)

## Quick Start

```cpp
#include "spsc_queue.h"

SPSC_Queue<int, 1024> q; // 1024-slot queue

// Producer thread
q.push(42);

// Consumer thread
int value;
if (q.pop(value))
{
  // value == 42
}
```

## Template Parameters

| Parameter | Meaning                                     |
| --------- | ------------------------------------------- |
| `T`       | Element type stored in the queue            |
| `N`       | Ring-buffer length (must be a power of two) |

---

## Public Interface

| Method                | Description                                                                 |
| --------------------- | --------------------------------------------------------------------------- |
| `bool push(const T&)` | Enqueue one element.<br>Returns **false** if the queue is full.             |
| `bool pop(T& out)`    | Dequeue one element into `out`.<br>Returns **false** if the queue is empty. |

Both methods are wait-free for the designated producer or consumer thread, respectively.

## Example: Producer / Consumer Threads

```cpp
#include <thread>
#include <iostream>
#include "spsc_queue.h"

SPSC_Queue<std::string, 256> q;

void producer()
{
  for (int i = 0; i < 1000; ++i)
  {
    while (!q.push("msg " + std::to_string(i))) { /* spin until space */ }
  }
}

void consumer()
{
  std::string msg;
  for (int i = 0; i < 1000; ++i)
  {
    while (!q.pop(msg)) { /* spin until data */ }
    std::cout << msg << '\n';
  }
}

int main()
{
  std::thread t1(producer);
  std::thread t2(consumer);
  t1.join();
  t2.join();
}
```

## Design Notes

* **Power-of-two length** allows fast modulo via bit-masking.
* **Memory ordering**:

  * Producer: relaxed load of `tail`, acquire load of `head`, release store of `tail`.
  * Consumer: relaxed load of `head`, acquire load of `tail`, release store of `head`.
* Only **one** thread may call `push` and **one** thread may call `pop`. Using additional threads will lead to undefined behaviour.

## License

MIT – see the [LICENSE](./LICENSE) file.
