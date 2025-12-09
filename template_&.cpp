#include <iostream>

/*
в шаблонах const и ref убираются, а * опционально

полная специализация - там, где после template пустые <>
*/

// -------------------------------------------------------------------------

template<typename T>
void f(T a, T b) {}
int main() {
  f(1, 1ll); // CE
  f<double>(1, 1ll); // OK
}
/*
в первом случае CE, потому что компилятор не додумывает, а ищет точные совпадения, во втором случае мы ему явно подсказываем, что в шаблон подставить и он просто скастит
*/

// -------------------------------------------------------------------------

template<typename T>
void f(T) {}    // 1
void f(int x){} // 2
int main() {
  int a = 10;
  long b = 10;
  f(a); // частное лучше общего (2) аналогия со скорой и красным светом
  f(b); // (1), потому что можно скаститься, но лучше создать точно
  f<int>(a);  // (1)
  f<long>(a); // (1)
}

// -------------------------------------------------------------------------

template<typename T, typename U>
T foo(U) { return 0; } // OK, сам догадается подставить int в аргумент (НО В return НЕ ДОГАДАЕТСЯ)
int main() {
  foo<int>(1);
}

// -------------------------------------------------------------------------

/*
Сначала компилятор генерит все функции (пытается раскрыть шаблоны), а только потом уже проверяется корректность самой функции(там по возвращаемым типам, есть ли каст и т д)
*/

// -------------------------------------------------------------------------

template<class T, typename U>
void foo() {}
template<typename U>
void foo<int, U>() {} // BAD
template<>
void foo<int, int>() {}
/*
в функциях доступна только полная специализация, а в классах и частичная и полная
*/

// -------------------------------------------------------------------------

template<typename T, typename U>
struct S {};

template<typename T>
struct S<int, T> {};

template<typename T>
struct S<T, int> {};

int main() {
  S<int, int> s = S<int, int>(); // BAD
  /*
  template<typename T>
  struct S<T, int> {};
  если это добавить, то OK
  */
}

// -------------------------------------------------------------------------

template<typename T>
struct S<const T> { int a = 1; };

template<typename T>
struct S<T&> { int a = 2; };

int main() {
  S<const int&> s;
  std::cout << s.a; // 2, ref приоритетнее const
}


template<typename T>
struct S<const T> { int a = 1; };

template<typename T>
struct S<T*> { int a = 2; };

int main() {
  S<const int*> s;
  std::cout << s.a; // 2, точное совпадение (const относится к int), T = const int, ptr убирается (опционально)
}

template<typename T>
struct S<const T> { int a = 1; };

template<typename T>
struct S<T*> { int a = 2; };

int main() {
  S<const int* const> s;
  std::cout << s.a; // 1 (const относится к ptr) T = const int *
}

// -------------------------------------------------------------------------

template<typename T, typename U>
void f(T, U) {}                    // Шаблонная версия 1

template<typename T>
void f(T, T) {std::cout << 1;}     // Шаблонная версия 2

template<>
void f(int, int) {std::cout << 2;} // Частный случай шаблонной версии 2 (специализация)

int main() {
  f(1, 1); // 2
}

/*
Шаблонные версии 1 и 2 - перегрузки функции, компилятор сначала выбирает между ними, а потом проверяет, есть ли специализация у более частного
если бы была перегрузка НЕшаблонная, выбралась бы она
*/


template<typename T, typename U>
void f(T, U) {}                    // Шаблонная версия 1

template<>
void f(int, int) {std::cout << 2;} // специализация шаблонной версии 1

template<typename T>
void f(T, T) {std::cout << 1;}     // Шаблонная версия 2

int main() {
  f(1, 1); // 1
}

/*
здесь компилятор выбрал вторую перегрузку, поэтому дальше первой не пошёл (не пошёл в <int, int>)
*/

/*
!!! в данном случае void f(int, int) относится к шаблонной функции 1, а в предыдущем примере - к 2
Иначе:
*/

template<typename T, typename U>
void f(T, U) {std::cout << 1;}        // Шаблонная версия 1

template<typename T>
void f(T, T) {std::cout << 2;}        // Шаблонная версия 2

template<>
void f(int, double) {std::cout << 3;} // специализация шаблонной версии 1

/*
!!! В перегрузках функций можно разное число шаблонов писать, а у структур НЕТ
*/

template<typename T, typename U>
struct f {};

template<typename T>
struct f {}; 
// ОЧЕНЬ ПЛОХО