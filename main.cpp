#include <iostream>
#include <chrono>
#include <deque>

#include "lib/allocator.cpp"

int main() {
  std::deque<int> au;
  for (int i = 0; i < 10000; i++) {
    au.push_back(i);
  }

  return 0;
}

/*
 #include <iostream>
#include <vector>

class MemoryPool {
 private:
  size_t block_size;
  size_t block_count;
  size_t count_free_bl;
  void* free_block;
  char* pool;
  char* pool_start;
  char* pool_end;

 public:
  MemoryPool(size_t block_size_, size_t block_count_) {
    block_size = block_size_;
    block_count = block_count;
    count_free_bl = block_count;
    pool = new char[block_size * block_count];
    free_block = pool;
    pool_start = pool;
    char* cur = pool;
    for (size_t i = 0; i < block_count - 1; i++) {
      *reinterpret_cast<void**>(cur) = cur + block_size;
      cur += block_size;
    }
    *reinterpret_cast<void**>(cur) = nullptr;
    pool_end = cur;
  }

  ~MemoryPool() {
    delete[] pool;
  }

  void* allocate(size_t size) {
    if (size > count_free_bl * block_size) {
      throw std::bad_alloc();
    }

    void* block = free_block;
    free_block = *reinterpret_cast<void**>(free_block);
    count_free_bl -= 1;
    return block;
  }

  void* deallocate(void* block) {
    *reinterpret_cast<void**>(block) = free_block;
    free_block = block;
    count_free_bl += 1;
  }

  char* PoolStart() {
    return pool_start;
  }

  char* PoolEnd() {
    return pool_end;
  }
};

class Myallocator {
 private:
  std::vector<MemoryPool*> pools;
 public:
  Myallocator(std::pair<size_t,size_t>* pools_, size_t size) {
    for (size_t i = 0; i < size; i++) {
      MemoryPool* new_pool = new MemoryPool(pools_[i].second, pools_[i].first / pools_[i].second);
      pools.push_back(new_pool);
    }
  }

  ~Myallocator() {
    for (size_t i = 0; i < pools.size(); i++) {
      delete pools[i];
    }
  }

  void* allocate(size_t size) {
    for (size_t i = 0; i < pools.size(); i++) {
      try {
        return pools[i]->allocate(size);
      } catch (std::bad_alloc&) {
        continue;
      }
    }

    throw std::bad_alloc();
  }

  void deallocate(void* block) {
    for (size_t i = 0; i < pools.size(); i++) {
      if (block >= pools[i]->PoolStart() && block <= pools[i]->PoolEnd()) {
        pools[i]->deallocate(block);
        return;
      }
    }
  }
};

 */
