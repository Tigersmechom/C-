int a = 10;
long& b = a;
/*
при такой записи будет CE, потому что здесь после b = rvalue (каст int -> long), поэтому нужен const
*/

struct S {
  void f() {}
  void g() const {
    f(); // CE
  }
};

// -------------------------------------------------------------------------

template<typename T>
void foo(T) {
  std::cout << 1;
}
template<>
void foo<int>(const int) {
  std::cout << 2;
}
int main() {
  int a = 10;
  const int b = 49;
  foo(a);
  foo(b);
  // Ok, 22
}

// CE:
template<typename T>
void foo(T) {
  std::cout << 1;
}
template<>
void foo<int>(int&) {
  std::cout << 2;
}

// -------------------------------------------------------------------------

void foo(int&) {std::cout << 1;}
void foo(const int&) {std::cout << 2;}
int main() {
  int a = 10;
  foo(a); // 1
}


void foo(int*) {std::cout << 1;}
void foo(const int*) {std::cout << 2;}
int main() {
  int a = 10;
  foo(&a); // 1
}


void foo(int) {std::cout << 1;}
void foo(const int) {std::cout << 2;} // error: redefinition of 'foo'
int main() {
  int a = 10;
  foo(a);
}


void foo(int*) {std::cout << 1;}
void foo(int* const) {std::cout << 2;} // error: redefinition of 'foo'
int main() {
  int a = 10;
  foo(a);
}

// -------------------------------------------------------------------------

struct A {
  int y = 49;
  int x = 39;;
  int foo() {return x;}
  A() : y(foo())/* UB */ {}
};

int main() {
  A a;
  std::cout << a.y;
  // a.x = 39
  // a.y = UB
}

// -------------------------------------------------------------------------

int main() {
  int a = 0;
  int& b = a;
  int& c = b;
  int& d = b > c ? b : c;
  d++;
  std::cout << a << b << c << d; // 1111
}

// -------------------------------------------------------------------------

/*
std::conditional_t<bool, T1, T2> (bool ? T1 : T2)
*/

// -------------------------------------------------------------------------

int main() {
  std::vector<int> a(2, 1);
  auto it = a.begin();
  *it++; // сначала ++, потом *
  for(auto i : a) std::cout << i; // 11
}

// ++*++it; тогда 12 (сначала ++, потом *, потом ++)

// -------------------------------------------------------------------------

struct G {
  virtual void foo() {}
  virtual void goo() {std::cout <<1;}
};

struct D : virtual G {
  void foo() {};
  void goo() {std::cout <<2;};
};

struct M : virtual G {
  void foo() {};
  void goo() {std::cout <<3;};
};

struct S : M, D {
  void foo() {};
  void goo() {std::cout << 4;};
};

struct I {
  long long a;
  long long b;
};

int main() {
  S s;
  void (S::*ptr)() = &S::D::goo;
  D& d = s;
  (s.*ptr)();
  //(d.*ptr)(); CE
  I huy = reinterpret_cast<I&>(ptr);
  std::cout << std::hex << huy.a << ' ' << huy.b; // 8 11 (первый - сдвиг в vtable, второй - сдвиг относительно начала объекта, 1, потому что это флаг, виртуальная или нет функция, иначе был бы указатель)
}

// -------------------------------------------------------------------------

int main() {
  int final = 29;
  int override = 39;
  int module = 38;
  int import = 938;
  // это ключевые слова

}

// -------------------------------------------------------------------------

int main() {
  std::vector a = {1, 2};
}

// -------------------------------------------------------------------------

template <typename T>
bool GetMax(const T& a, const T& b) {
    return std::is_same_v< decltype(a > b ? a : b),  const T& >; // true
}

template <typename T, typename U>
bool GetMax(const T& a, const U& b) {
    return std::is_same_v< decltype(a > b ? a : b), T >; // true
}

int main() {
    std::cout << GetMax(1, 2);
    int a = 3;
    int b = 9;
    int& c = a;
    int& d = b;
    int& e = c > d ? c : d; // OK, decltype(e) = int& 
    // Это из-за того, что ? из-за разных типов делает копию, но если типы одинаковые, то всё ок
}

// -------------------------------------------------------------------------

namespace N {
  int x = 10;
}

int main() {
  using namespace N; // OK
  int x = 20;
}

int main() {
  using N::x;
  int x = 20; // CE
}

// -------------------------------------------------------------------------

int main() {
  int x = 0;
  x++ && ++x;
  std::cout << x; // 1, потому что && ленивый
}

// -------------------------------------------------------------------------

int main() {
  struct A {
  int x = 0;

} gg;
  A g = gg;
}

// -------------------------------------------------------------------------

struct S {
  const std::string& r;
  S() : r(std::string("hi")) {} // r повиснет сразу после конструктора
};

// -------------------------------------------------------------------------