#pragma once

namespace irs {

template<class Type>
class PriorityQueue {
 private:
  Type* heap_;
  size_t size_;
  bool own_;
  size_t max_size_;

  void UpHeap() {
    size_t i = size_;
    Type node = heap_[i];  // save bottom node (WAS object)
    int32_t j = ((uint32_t)i) >> 1;
    while (j > 0 && LessThan(node, heap_[j])) {
      heap_[i] = heap_[j];  // shift parents down
      i = j;
      j = ((uint32_t)j) >> 1;
    }
    heap_[i] = node;  // install saved node
  }
  void DownHeap() {
    size_t i = 1;
    Type node = heap_[i];  // save top node
    size_t j = i << 1;     // find smaller child
    size_t k = j + 1;
    if (k <= size_ && LessThan(heap_[k], heap_[j])) {
      j = k;
    }
    while (j <= size_ && LessThan(heap_[j], node)) {
      heap_[i] = heap_[j];  // shift up child
      i = j;
      j = i << 1;
      k = j + 1;
      if (k <= size_ && LessThan(heap_[k], heap_[j])) {
        j = k;
      }
    }
    heap_[i] = node;  // install saved node
  }

 protected:
  PriorityQueue() {
    size_ = 0;
    own_ = false;
    heap_ = NULL;
    max_size_ = 0;
  }

  virtual bool LessThan(Type a, Type b) = 0;

  void Initialize(const size_t maxSize, bool deleteOnClear) {
    size_ = 0;
    own_ = deleteOnClear;
    max_size_ = maxSize;
    size_t heapSize = max_size_ + 1;
    heap_ = new Type[heapSize];
  }

 public:
  virtual ~PriorityQueue() {
    Clear();
    delete[] heap_;
  }

  void Put(Type element) {
    size_++;
    heap_[size_] = element;
    UpHeap();
  }

  bool Insert(Type element) {
    if (size_ < max_size_) {
      Put(element);
      return true;
    } else if (size_ > 0 && !LessThan(element, Top())) {
      if (own_) {
        delete heap_[1];
      }
      heap_[1] = element;
      AdjustTop();
      return true;
    } else
      return false;
  }

  Type Top() {
    if (size_ > 0)
      return heap_[1];
    else
      return NULL;
  }

  Type Pop() {
    if (size_ > 0) {
      Type result = heap_[1];   // save first value
      heap_[1] = heap_[size_];  // move last to first

      heap_[size_] = (Type)0;  // permit GC of objects
      size_--;
      DownHeap();  // adjust heap_
      return result;
    } else
      return (Type)NULL;
  }

  void AdjustTop() { DownHeap(); }

  size_t Size() { return size_; }

  void Clear() {
    for (size_t i = 1; i <= size_; i++) {
      if (own_) {
        delete heap_[i];
      }
    }
    size_ = 0;
  }
  Type operator[](size_t pos_) { return heap_[pos_ + 1]; }
  Type Get(size_t pos_) { return heap_[pos_ + 1]; }

  void SetOwn(bool own) { own_ = own; }
};

}  // namespace irs
