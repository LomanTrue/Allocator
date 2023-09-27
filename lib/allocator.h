#include <iostream>
#include <vector>
#include <memory>
#include <cstdarg>

class MemoryPool {
 private:
  size_t block_size;
  size_t block_count;
  size_t count_free_bl;
  char* free_block;
  char* pool;
  char* pool_begin;
  char* pool_end;
  std::vector<char*> blocks;
  std::vector<bool> used_blocks;
  
 public:
  MemoryPool(size_t block_size_, size_t block_count_) {
    block_size = block_size_;
    block_count = block_count_;
    count_free_bl = block_count_;
    pool = new char[block_size_ * block_count_];
    pool_begin = pool;
    free_block = pool;

    for (size_t i = 0; i < block_count_; i++) {
      blocks.push_back(free_block);
      used_blocks.push_back(false);
      free_block += block_size_;
    }

    pool_end = free_block;
  }

  ~MemoryPool() {
    delete[] pool;
  }

  void* allocate(size_t size);
  void deallocate(void* block, size_t block_count);
  char* begin();
  char* end();

  bool operator==(MemoryPool other);
  bool operator!=(MemoryPool other);
};

template <typename T, size_t pool_size, size_t first_block_size, size_t... block_sizes>
class MyAllocator {
 private:
  std::vector<MemoryPool*> pools;

  void makePools(size_t f_block_size, ...);

 public:
  using value_type = T;
  using Traits = std::allocator_traits<MyAllocator<T, pool_size, block_sizes...>>;
  using size_type = typename std::allocator<T>::size_type;
  using difference_type = typename std::allocator<T>::difference_type;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  template<typename U>
  struct rebind {
    typedef MyAllocator<U, pool_size, first_block_size, block_sizes...> other;
  };

  MyAllocator() {
    makePools(first_block_size, block_sizes...);
  }
  
  MyAllocator(const MyAllocator<T, pool_size, first_block_size, block_sizes...>& other): pools(other.pools) {};
  template<typename Alloc>
  explicit MyAllocator(const Alloc& other) {
    makePools(first_block_size, block_sizes...);
  }
  
  ~MyAllocator() {
    for (size_t i = 0; i < pools.size(); i++) {
      delete pools[i];
    }
  };

  T* allocate(size_t n);
  void deallocate(T* block, size_t n);

  bool operator==(const MyAllocator<T, pool_size, first_block_size, block_sizes...>& other);
  bool operator!=(const MyAllocator<T, pool_size, first_block_size, block_sizes...>& other);
};



/*
class MemoryPool {
 private:
  size_t block_size;
  size_t block_count;
  size_t count_free_bl;
  void* free_block;
  char* pool;
  char* pool_start;
  char* pool_end;
  std::vector<char*> blocks;

 public:
  MemoryPool(size_t block_size_, size_t block_count_) {
    block_size = block_size_;
    block_count = block_count_;
    count_free_bl = block_count;
    pool = new char[block_size * block_count];
    free_block = pool;
    pool_start = pool;
    char* cur = pool;
    for (size_t i = 0; i < block_count; i++) {
      blocks.push_back(cur);
      cur += block_size;
    }
    pool_end = cur -= block_size;
  }

  ~MemoryPool() {
    delete[] pool;
  }

  void* allocate(size_t size) {
    if (size > count_free_bl * block_size) {
      throw std::bad_alloc{};
    }

    void* block = blocks[0];
    count_free_bl--;
    return block;
  }

  void deallocate(void* block) {
    blocks.push_back(static_cast<char*>(block));
    count_free_bl++;
    return;
  }

  char* PoolStart() {
    return pool_start;
  }

  char* PoolEnd() {
    return pool_end;
  }
};

template <class T, size_t pool_size, size_t size_>
class Myallocator {
  std::vector<MemoryPool*> pools;
 public:
  using value_type = T;
  using Traits = std::allocator_traits<Myallocator<T, pool_size, size_>>;
  using size_type = typename std::allocator<T>::size_type;
  using difference_type = typename std::allocator<T>::difference_type;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  template<typename U>
  struct rebind {
    typedef Myallocator<U, pool_size, size_> other;
  };

  Myallocator() {
    //for (size_t i = 0; i < size_; i++) {
      MemoryPool* new_pool = new MemoryPool(size_, pool_size / size_);
      pools.push_back(new_pool);
    //}
  }

  Myallocator(const Myallocator<T, pool_size, size_>& other) : pools(other.pools) {}
  template<typename Alloc>
  explicit Myallocator(const Alloc& other) {
    //for (size_t i = 0; i < size_; i++) {
      MemoryPool* new_pool = new MemoryPool(size_, pool_size / size_);
    //}
  }

  ~Myallocator() {
    for (size_t i = 0; i < pools.size(); i++) {
      delete pools[i];
    }
  }

  T* allocate(size_t size) {
    for (size_t i = 0; i < pools.size(); i++) {
      try {
        return static_cast<T*>(pools[i]->allocate(size));
      } catch (std::bad_alloc&) {
        continue;
      }
    }

    throw std::bad_alloc{};
  }

  void deallocate(T* block, size_t size) {
    //for (size_t i = 0; i < pools.size(); i++) {
      if (static_cast<void*>(block) >= pools[0]->PoolStart() && static_cast<void*>(block) <= pools[0]->PoolEnd()) {
        pools[0]->deallocate(block);
        return;
      }
    //}
  }

  bool operator==(const Myallocator<T, pool_size, size_>& other) {

  }
  bool operator!=(const Myallocator<T, pool_size, size_>& other) {
    return !(*this == other);
  }
};

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
    block_count = block_count_;
    count_free_bl = block_count;
    pool = new char[block_size * block_count];
    free_block = pool;
    pool_start = pool;
    char* cur = pool;
    for (size_t i = 0; i < block_count - 1; i++) {
      *reinterpret_cast<void**>(cur) = cur + block_size;
      cur += block_size;
    }
    *reinterpret_cast<char**>(cur) = nullptr;
    pool_end = cur;
    //Delete
    char* a = pool;
    for (size_t i = 0; i < 10; i++) {
      a = *reinterpret_cast<char**>(a);
    }
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
    count_free_bl--;
    return block;
  }

  void deallocate(void* block) {
    *reinterpret_cast<void**>(block) = free_block;
    free_block = block;
    count_free_bl++;
    return;
  }

  char* PoolStart() {
    return pool_start;
  }

  char* PoolEnd() {
    return pool_end;
  }
};

template <class T, size_t pool_size, size_t size_>
class Myallocator {
  std::vector<MemoryPool*> pools;
 public:
  using value_type = T;
  using Traits = std::allocator_traits<Myallocator<T, pool_size, size_>>;
  using size_type = typename std::allocator<T>::size_type;
  using difference_type = typename std::allocator<T>::difference_type;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  template<typename U>
  struct rebind {
    typedef Myallocator<U, pool_size, size_> other;
  };

  Myallocator() {
    //for (size_t i = 0; i < size_; i++) {
      MemoryPool* new_pool = new MemoryPool(size_, pool_size / size_);
      pools.push_back(new_pool);
    //}
  }

  Myallocator(const Myallocator<T, pool_size, size_>& other) : pools(other.pools) {}
  template<typename Alloc>
  explicit Myallocator(const Alloc& other) {
    //for (size_t i = 0; i < size_; i++) {
      MemoryPool* new_pool = new MemoryPool(size_, pool_size / size_);
    //}
  }

  ~Myallocator() {
    for (size_t i = 0; i < pools.size(); i++) {
      delete pools[i];
    }
  }

  T* allocate(size_t size) {
    for (size_t i = 0; i < pools.size(); i++) {
      try {
        return static_cast<T*>(pools[i]->allocate(size));
      } catch (std::bad_alloc&) {
        continue;
      }
    }

    throw std::bad_alloc();
  }

  void deallocate(T* block, size_t size) {
    //for (size_t i = 0; i < pools.size(); i++) {
      if (static_cast<void*>(block) >= pools[0]->PoolStart() && static_cast<void*>(block) <= pools[0]->PoolEnd()) {
        pools[0]->deallocate(block);
        return;
      }
    //}
  }

  bool operator==(const Myallocator<T, pool_size, size_>& other) {

  }
  bool operator!=(const Myallocator<T, pool_size, size_>& other) {
    return !(*this == other);
  }
};
 */