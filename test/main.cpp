#include <iostream>
#include <ThreadPool.h>

volatile int i = 0;

static void do_something() { i++; }

int main() {
  ThreadPool pool{8};
  for (int j = 0; j < 50000; j++) {
    pool.execute(do_something);
  }
  pool.shutdown();
  return 0;
}