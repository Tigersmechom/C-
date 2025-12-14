#include <iterator>
#include <iostream>

/*
Iterator : умеет инкрементиться (не умеет читать, что под собой)
OutputIterator : Итератор, это итератор, через который можно бесконечно записывать значения (инкрементироваться), но не обязательно можно читать и необязательно можно несколько раз итерироваться по нему
IntputIterator : он может только 1 раз пройтись по контейнеру (1 раз вправо и всё) и читать, что под ним находится
FrowardIterator : Как Input_iter, только может несколько раз пройтись по контейнеру (односвязный список(forward_list), unordered_set/map)
BiDirectionIterator : умеет декрементиться (set, map, двусвязный список)
RandomAccessIterator : умеет делать += -= (например, deque)
ContiguousIterator : (vector, array), для него выполняется: *(&(*iter) + n) == *(iter + n)
*/

// -------------------------------------------------------------------------

/*
const_iterator - просто под собой хранит const T*, то есть нельзя *++it;

reverse_iterator - std::reverse_iterator<iterator>, его самому реализовывать не нужно, он делает так:
rbegin = end, а при операции разыменования: сначала --, потом разыменование (--begin() = end())
reference operator*() const {
    Iter tmp = current;   // копия!
    --tmp;
    return *tmp;
}
*/

// -------------------------------------------------------------------------

/*
Range-based for: 
for (int x : std::set<int>)

под капотом раскрывается

for (std::set<int>::iterator it = set.begin(); it != s.end(); ++it)
*/

// -------------------------------------------------------------------------

// Iterator_traits 

template <typename Iter>
void foo(Iter it) {
  typename std::iterator_traits<Iter>::value_type x = *it;
}


/*
iterator_category — “какой это итератор” (Legacy-категория)
Это тэг-типы, которые описывают, какие операции гарантированы:
std::input_iterator_tag — можно читать и двигаться только вперёд, обычно одно проход (например, istream_iterator)
std::forward_iterator_tag — можно читать/писать (если не const), двигаться вперёд, мультипроход (например, forward_list)
std::bidirectional_iterator_tag — плюс --it (например, list, set, map)
std::random_access_iterator_tag — плюс арифметика it + n, it2 - it1, it[n] (например, vector, deque, сырые указатели)

value_type — “какие значения хранит/дает итератор” (без ссылочности). Это “чистый” тип элемента: у vector<int>::iterator это int

difference_type — Тип для: результата last - first (если это random access), счётчиков в std::distance, std::advance, индексов смещений. Обычно это std::ptrdiff_t.

pointer — “указатель-подобный” тип (обычно T* или что-то вроде прокси).

reference — тип результата разыменования *it (обычно T&, но может быть прокси).


*/


// -------------------------------------------------------------------------

// std::advance(it, n) - просто делает it+=n (в зависимости от итератора либо фориком ++, либо +=, перегрузка работает через std::iterator_tag)

// -------------------------------------------------------------------------


struct A {
  bool operator()(int x) { return x&1;}
};

int main() {
  std::vector<int> a(10, 1);
  std::vector<int> b(10);
  std::copy_if(a.begin(), a.end(), b.begin(), A());
  for(auto i : b) std::cout << i;
}

// std::copy(откуда.begin(), откуда.end(), куда) - тупой алгоритм: разыменовывание, присваивание, инкрементирование

// std::copy(откуда.begin(), откуда.end(), куда, предикат(функтор, возвращающий bool)) - тупой алгоритм: разыменовывание, присваивание, инкрементирование

// при выходе за пределы - UB (например, a > b по размеру), здесь нужен Output_iterator (back_inserter)

// -------------------------------------------------------------------------

// Адаптер - то, что изменяет интерфейс, типо queue, priority_queue - адаптер поверх контейнера deque или rIter - адапрет над iter

