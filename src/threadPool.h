#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

template <class F> class BindArgsMover {
public:
  explicit BindArgsMover(F &&f) : f_(std::forward<F>(f)) {}

  template <class... Args> auto operator()(Args &&...args) {
    f_(std::move(args)...);
  }

private:
  F f_;
};

template <class F, class... Args> auto bind_simple(F &&f, Args &&...args) {
  return std::bind(BindArgsMover<F>(std::forward<F>(f)),
                   std::forward<Args>(args)...);
}

class ThreadPool {
public:
  ThreadPool() = default;
  ThreadPool(ThreadPool &&) = default;
  ~ThreadPool() { shutdown(); }

  explicit ThreadPool(size_t thread_count) : data_(std::make_shared<data>()) {
    for (size_t i = 0; i < thread_count; ++i) {
      data_->threads_.emplace_back(std::thread([data = data_] {
        std::unique_lock<std::mutex> lk(data->mtx_);
        for (;;) {
          if (!data->tasks_.empty()) {
            auto current = std::move(data->tasks_.front());
            data->tasks_.pop();
            lk.unlock();
            current();
            lk.lock();
          } else if (data->is_shutdown_) {
            break;
          } else {
            data->cond_.wait(lk);
          }
        }
      }));
    }
  }

  template <class F, class... Args> void execute(F &&f, Args &&...args) {
    auto task = bind_simple(f, args...);
    {
      std::lock_guard<std::mutex> lk(data_->mtx_);
      data_->tasks_.emplace(task);
    }
    data_->cond_.notify_one();
  }

  bool empty() { return data_->tasks_.empty(); }

  void shutdown() {
    if ((bool)data_) {
      {
        std::lock_guard<std::mutex> lk(data_->mtx_);
        data_->is_shutdown_ = true;
      }

      data_->cond_.notify_all();
      for (auto &th : data_->threads_) {
        if (th.joinable()) {
          th.join();
        }
      }
    }
  }

private:
  struct data {
    std::mutex mtx_;
    std::condition_variable cond_;
    bool is_shutdown_ = false;
    std::queue<std::function<void()>> tasks_;
    std::vector<std::thread> threads_;
  };

  std::shared_ptr<data> data_;
};

#endif // THREADPOOL_H
