#include "allocator.h"

void* MemoryPool::allocate(size_t size) {
  if (size > count_free_bl * block_size) {
    throw std::bad_alloc{};
  }

  size_t need_block_count = size / block_size;
  if (size % block_size != 0) {
    need_block_count++;
  }

  size_t count = 0;
  size_t first_block = 0;
  for (size_t i = 0; i < blocks.size(); i++) {
    if (blocks[i]) {
      count++;
    } else {
      first_block = i + 1;
      count = 0;
    }
    if (count == need_block_count) {
      break;
    }
  }

  if (count != need_block_count) {
    throw std::bad_alloc{};
  } else {
    for (size_t i = first_block; i < first_block + need_block_count; i++) {
      used_blocks[i] = true;
    }
    count_free_bl -= need_block_count;
    void* ans = blocks[first_block];
    return ans;
  }
}

void MemoryPool::deallocate(void* block, size_t block_count) {
  count_free_bl += block_count;
  for (size_t i = 0; i < blocks.size(); i++) {
    if (blocks[i] == block) {
      for (size_t j = i; j < i + block_count; j++) {
        used_blocks[j] = false;
      }
      return;
    }
  }
}

char* MemoryPool::begin() {
  return pool_begin;
}

char* MemoryPool::end() {
  return pool_end;
}

bool MemoryPool::operator==(MemoryPool other) {
  if (pool_begin != other.pool_begin || pool_end != other.pool_end || block_size != other.block_size || block_count != other.block_count) {
    return false;
  } else {
    return true;
  }
}

bool MemoryPool::operator!=(MemoryPool other) {
  return !(*this==other);
}

template <typename T, size_t pool_size, size_t first_block_size, size_t... block_sizes>
void MyAllocator<T, pool_size, first_block_size, block_sizes...>::makePools(size_t f_block_size, ...) {
  MemoryPool* cur_pool = new MemoryPool(f_block_size, pool_size / f_block_size);
  pools.push_back(cur_pool);

  size_t pool_count = sizeof...(block_sizes);

  std::va_list pool_block_sizes;
  va_start(pool_block_sizes, f_block_size);
  for (int i = 0; i < pool_count; i++) {
    size_t block_size = va_arg(pool_block_sizes, size_t);
    MemoryPool* cur_pool = new MemoryPool(block_size, pool_size / block_size);
    pools.push_back(cur_pool);
  }
  va_end(pool_block_sizes);
}

template <typename T, size_t pool_size, size_t first_block_size, size_t... block_sizes>
T* MyAllocator<T, pool_size, first_block_size, block_sizes...>::allocate(size_t n) {
  for (size_t i = 0; i < pools.size(); i++) {
    try {
      return static_cast<T*>(pools[i]->allocate(n * sizeof(T)));
    } catch (std::bad_alloc&) {
      continue;
    }
  }

  throw std::bad_alloc{};
}

template <typename T, size_t pool_size, size_t first_block_size, size_t... block_sizes>
void MyAllocator<T, pool_size, first_block_size, block_sizes...>::deallocate(T* block, size_t n) {
  for (size_t i = 0; i < pools.size(); i++) {
    if (static_cast<void*>(block) >= pools[i]->begin() && static_cast<void*>(block) < pools[i]->end()) {
      pools[i]->deallocate(block, n);
    };
  }
}

template <typename T, size_t pool_size, size_t first_block_size, size_t... block_sizes>
bool MyAllocator<T, pool_size, first_block_size, block_sizes...>::operator==(const MyAllocator<T,
                                                                                               pool_size,
                                                                                               first_block_size,
                                                                                               block_sizes...>& other) {
  if (pools.size() != other.pools.size()) {
    return false;
  }
  for (size_t i = 0; i < pools.size(); i++) {
    if (pools[i] != other.pools[i]) {
      return false;
    }
  }

  return true;
}

template <typename T, size_t pool_size, size_t first_block_size, size_t... block_sizes>
bool MyAllocator<T, pool_size, first_block_size, block_sizes...>::operator!=(const MyAllocator<T,
                                                                                               pool_size,
                                                                                               first_block_size,
                                                                                               block_sizes...>& other) {
  return !(*this == other);
}
/*
template<typename T, size_t PoolSize = 8192, size_t FirstChunkSize = 32, size_t... ChunkSizes>
class Allocator {
  std::shared_ptr<std::vector<Pool>> pools;
  void InitPools(size_t first_chunk_size, ...);

 public:
  Allocator() {
    InitPools(FirstChunkSize, ChunkSizes...);
  }
  Allocator(const Allocator<T, PoolSize, FirstChunkSize, ChunkSizes...>& other) : pools(other.pools) {}
  template<typename Alloc>
  explicit Allocator(const Alloc& other) {
    InitPools(FirstChunkSize, ChunkSizes...);
  }

  T* allocate(size_t n);
  void deallocate(T* ptr, size_t n);
  bool operator==(const Allocator<T, PoolSize, FirstChunkSize, ChunkSizes...>& other);
  bool operator!=(const Allocator<T, PoolSize, FirstChunkSize, ChunkSizes...>& other) {
    return !(*this == other);
  }

  using value_type = T;
  using Traits = std::allocator_traits<Allocator<T, PoolSize, FirstChunkSize, ChunkSizes...>>;
  using size_type = typename std::allocator<T>::size_type;
  using difference_type = typename std::allocator<T>::difference_type;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  template<typename U>
  struct rebind {
    typedef Allocator<U, PoolSize, FirstChunkSize, ChunkSizes...> other;
  };
};
 */
