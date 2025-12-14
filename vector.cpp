#include <cstdint>

template <typename T>
class vector {
 private:
  size_t sz_;
  size_t cp_;
  T* data_;

 public:
  
  void Reserve(size_t ncap) {
    if (ncap <= cp_) return;
    T* nd = static_cast<T*>(::operator new(ncap * sizeof(T)));
    // Не в блоке try, потому что мы ещё ничего создать не успели, catch попросту не нужен

    size_t idx = 0;
    try {
      for (; idx < sz_; ++idx) {
        new(nd + idx) T(data_[idx]); // (placement new) Не nd[idx], потому что объект T ещё не создан, а мы уже operator= вызываем
      }
    } catch (...) {
      for (size_t jdx = 0; jdx < idx; ++jdx) {
        nd[jdx].~T();
      }
      delete[] reinterpret_cast<char*>(nd);
      throw;
    }

    for (size_t i = 0; i < sz_; ++i) {
      data_[i].~T();
    }
    delete[] reinterpret_cast<char*>(data_);

    data_ = nd;
    cp_ = ncap;
  }

  void push_back(const T& oth) {
    if (sz_ == cp_) Reserve(cp_ * 2);
    new(data_ + 1) T(oth);
    ++sz_;
  }

  void pop_back() {
    (data_ + sz_ - 1)->~T();
    --sz_;
  }
};

class bit_reference {
 private:
  uint8_t idx_;
  uint8_t* block_;
 public:
  bit_reference(uint8_t* block, uint8_t idx): block_(block), idx_(idx) {}

  bit_reference& operator=(const bool oth) {
    if (oth) *block_ |= (1U << idx_);
    else *block_ &= ~(1U << idx_);
    return *this;
  }

  operator bool() const noexcept {
    return (*block_ & (1U << idx_)) > 0U;
  }
};

template<>
class vector<bool> {
 private:
  uint8_t* data_;
  size_t sz;
  size_t cap;
 public:
  bit_reference operator[](size_t idx) {
    return bit_reference((data_+ idx / 8UZ), idx % 8UZ);
  }
};