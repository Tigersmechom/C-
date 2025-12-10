#include <array>
#include <iostream>

/*
в шаблонах const и ref убираются, а * опционально

полная специализация - там, где после template пустые <>

typename = class

type trait - шаблонные структуры, позволяющие делать операции с типами на этаме компиляции, например, std::is_same
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

// -------------------------------------------------------------------------

// Template template parameters

template<typename T, template <typename, typename> typename Container>
struct Stack {
  Container<T, T> cont;
};

template<typename T, template <typename> typename Container = std::vector>
struct Stack {
  Container<T> cont;
};

// -------------------------------------------------------------------------

// Если не сделать базу, то будет ошибка выполнения самой программы компилятор

template<size_t N, size_t D>
struct Helper {
  static const bool value = N % D == 0 ? false : Helper<N, D - 1>::value;
};

template<size_t N>
struct Helper<N, 1> {
  static const bool value = true;
};

template<size_t N>
struct IsPrime {
  static const bool value = Helper<N, N - 1>::value;
};

template<>
struct IsPrime<1> {
  static const bool value = false;
};

int main() {
  std::cout << IsPrime<6 + 1>::value;
}

// -------------------------------------------------------------------------

// Depended names (зависимые имена)

template <typename T>
struct S {
  using A = int;
};
template <>
struct S<double> {
  static const int A = 5;
};

int x = 0;

template <typename T>
void f() {
  S<T>::A* x;
}

/*
при f<double> будет выражение: 5 * 0; (expression)
в данном случае компилятор до подстановки T хочет распарсить выражение
!!! и по стандарту сказано, что подобные случаи - всегда expression, на не название типов
*/ 

template <typename T>
void f() {
  typename S<T>::A* x;
}
/*
А теперь int* x, то есть в данном случае S<T>::A - тип (мы явно указали)
НО: нельзя теперь double запихать, потому что синтаксис неизменен: либо тип, либо выражение для всех T
*/

// -------------------------------------------------------------------------

template <typename T>
struct S {
  template <int N>
  using A = std::array<int, N>;
};
int x = 0;

template <typename T>
void f() {
  S<T>::A< 10 > x; // Expression (переменная A меньше 10, больше x)
}

// -------------------------------------------------------------------------

template <typename T>
struct S {
  template <int N>
  using A = std::array<int, N>;
};
int x = 0;

template <typename T>
void f() {
  typename S<T>::template A< 10 > x; // std::array<int, 10> x
}

// -------------------------------------------------------------------------

template <typename T>
struct S {
  template <int N>
  void foo(int) {}
};

template <typename T>
void bar(int x, int y) {
  S<T> s;
  s.template foo<1>(x + y);
}
/*
Здесь без template будет выражение (если foo - поле S)
*/

// -------------------------------------------------------------------------

template <typename T>
struct Base {
  int x = 0;
};

template <>
struct Base<double> {
};

template <typename T>
struct Derived : Base<T> {
  void f() {
    ++this->x;
  }
};

/*
Просто запомнить, что здесь нужен this (обращение к полю шаблонного родителя), или ++Base<T>::x;
*/

// -------------------------------------------------------------------------

template <typename ... types> // объявили пачку типов
void f(types ... tx) { // ... - распаковали пачку типов (tx - пачка переменных)
  std::cout << sizeof...(tx) << '\n';
  next(tx...); // тоже распаковываем дальше
}

// -------------------------------------------------------------------------

template <typename T>
void print(T head) {                      // 1
  std::cout << head << '\n';
}

template <typename T, typename ... types>
void print(T head, types ... tail) {      // 2
  std::cout << head << '\n';
  print(tail...);
}

/*
с одним аргументом выиграет 1, также обязательно должна быть остаточная функция (конечное число аргументов), можно и такую (но обязательно перед variadic template function)
*/

void print() {}

template <typename T, typename ... types>
void print(const T& head, const types& ... tail) {
  std::cout << head << '\n';
  print(tail...);
}

/*
Можно и так (по ссылке), применится к каждому объекту
*/

// -------------------------------------------------------------------------

// Fold expressions (вырвжение свёртки since c++17)

template<typename... tail>
struct S {
  static const bool value = (std::is_pointer_v<tail> && ...); 
};

// Compile time

template<typename Head, typename... tail>
struct is_same_types {
  static const bool value = (std::is_same_v<Head, tail> && ...);
};


1) Unary right fold (E op ...) becomes (E_1 op (... op (E_(N-1) op E_N)))
2) Unary left fold (... op E) becomes (((E_1 op E_2) op ...) op E_N)
3) Binary right fold (E op ... op I) becomes (E_1 op (... op (E_(N−1) op (E_N op I))))
4) Binary left fold (I op ... op E) becomes ((((I op E_1) op E_2) op ...) op E_N)

template <typename ... types>
void print(const types& ... tail) {
  (std::cout << ... << tail) << '\n';
}

3): 
(a + ... + z)     →  (a + (b + (c + z)))
(a && ... && true) → (a && (b && (c && true)))

template<typename... Ts>
auto sum_right_init(auto init, Ts... args) {
    return (args + ... + init);  // binary right fold
}

2)
(... + args)   →  ((a + b) + c)
(... && args)  →  ((a && b) && c)

// -------------------------------------------------------------------------

// CRTP (Curiously Recursive Template Pattern)

template<typename T>
struct Base {
  T a; // CE
  T* ptr; // OK
  T& ref; // OK
  T func() {
    static_cast<T*>(this)->add();
    return T();
  }
};

struct Derived : Base<Derived> { void add() {} };