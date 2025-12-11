#include <exception>
#include <iostream>
#include <new>

/*
Раньше в сях использовалась глобальная переменная ERRNO (в linux), которая хранила
Код последней ошибки, и люди вручную бесконечно её проверяли

Исключения дропают операторы: new (std::bad_alloc), dynamic_cast (std::bad_cast), typeid (std::bad_typeid (typeid от nullptr)), throw 
*/

int Div(int a, int b) {
  if (b == 0) throw std::logic_error("Division by zero!");
  return a / b;
}

int main() {
  try {
    Div(1, 0);
  } catch (std::logic_error& err) {
    std::cout << err.what() << '\n'; // Division by zero!\n
  } catch(...) {
    // ХЗ
  }
}

// -------------------------------------------------------------------------

int main() {
  std::vector<int> a;
  int x = 1;
  try {
    a[1e18] = 1;
    int y = x / 0;
  } catch (...) {
    // RE, потому что SegFault - не exception
    // RE, Division by zero - не exception
    // Также Stack overflow
    // Это всё не exception с точки зрения плюсов, это exception с точки зрения ОС(не ловятся) 
    // Если бы не поймали то, что ловится, причина падения для OC - abort()
    // То, что мы ловим throw - совершенно другой уровень абстракции
    // Процессор увидел деление на 0, позвал операционку, операционка кильнула программу, которая это сделала
    // В том же python там везде ифы, которые страхуют и сам python ОС не убивает

    // Runtime errors (низкоуровневые): segFault, Floating Point Exceptions (деление на 0), abort(от std::terminate (например, от непойманного исключения (Это UB, деструктор не вызовется), pure virtual func called, assert))

    // Logic_error - косяк юзера (например, segfault)
    // Runtime_error - ошибки, в которыз пользователь не виноват (мб случился stack_overflow)
  }
}

// -------------------------------------------------------------------------

struct A{
  A() {std::cout << "Create\n";}
  A(const A&) {std::cout << "Copy\n";}
  ~A() {std::cout << "Del\n";}
};

void foo(int x) noexcept(false) {
  A a; //создаётся A1 на стеке
  if (x == 0) {
    throw a; // Копия А2 (в динамической памяти в Runtime)
  }
} // Del A1

int main() {
  A a5;
  try {
    foo(0);
  } catch (...) { // можно только после всех именных
    std::cout << "Caught!\n";
  } // Del A2


  catch (A a) { // Была бы ещё 1 копия, но уже на стеке (A3)
    std::cout << "Caught!\n";
  } // Del A3

  catch (A& a) { // ссылка на объект A2 из динамической памяти
    std::cout << "Caught!\n";
  }

  catch (A& a) { // Дальше пускаем предыдущее исключение
    throw;
  }

  catch (A& a) { // Удаляем предыдущее исключение (сам объект A2 уничтожится) и бросаем новое (или можно его же и скопировать (a))
    throw a5;
  }
}

// -------------------------------------------------------------------------

int main() {
  try { int* a = new int[1e17]; } 
  catch (std::bad_alloc& ex) {} // Компилятор резервирует для этой ошибки место в статической памяти
}

// -------------------------------------------------------------------------

int main() {
  try { throw 1; } 
  catch (double)    { std::cout << "db\n"; }
  catch (long long) { std::cout << "ll\n"; }
} // CE

// Будет Terminate, потому что никакие конверсий между типами нет
// Есть перегрузки только с & или const или можно поймать родителя, если был брошен наследник (например, если наследуется от std::exception):

catch (const int) { std::cout << "RefI\n"; }
catch (int&) { std::cout << "ConstI\n"; }

struct A{};
struct B : A{};
int main() {
  try { throw B(); } 
  catch (A) { std::cout << "Par\n"; }
  // Если бы было приватное наследование, то CE
  // Если бы было ромбовидное наследование, то Granny не поймается просто
  // Если бы было ромбовидное виртуальное наследование, то Granny бы поймалась
}

// Вот это бы поймалось

// Идёт поочереди и ловится первый подходящий

// -------------------------------------------------------------------------

// Идиома RAII (Resource Acquisition Is Initialization) (Приобретение ресурса есть инициализация)

void g(int x) {
  if (x == 0) throw 1;
}

void f(int x = 0) {
  int* p = new int(x);
  g(*p);
  delete p;
}

// При развёртывании стека, p не удалится -> Утечка памяти

struct Guard { // Его нельзя копировать
  int* p;
  Guard(int* p) : p(p) {}
  Guard(const Guard&)            = delete;
  Guard& operator=(const Guard&) = delete;
  ~Guard() { delete p; }
};

void g(int x) { if (x == 0) throw 1; }

void f(int x = 0) {
  int* p = new int(x);
  Guard G(p);
  g(*p);
}

// А вот так всё OK

// -------------------------------------------------------------------------

struct A{
  A() {std::cout << "A\n";}
  ~A() {std::cout << "~A\n";}
};

struct S{
  A* a; // Нужен RAII
  A aa; // Дуструктор полей вызовется
  S(int x = 0): a(new A()) {
    if (x == 0) throw 1;
    std::cout << "S\n"; // Не вызовется
  }
  ~S() {std::cout << "~S\n"; delete a;} // Не вызовется
};

int main() {
  try { S(0);} // Вывод: A A ~A
  catch (...) {}
}

// -------------------------------------------------------------------------

// Function try block

struct A{
  A(int x) {
    std::cout << "A\n";
    if (x == 0) throw 1;
  }
  ~A() {std::cout << "~A\n";}
};

struct S{
  A a; // успел проинициализироваться -> вызовется деструктор
  A aa;
  A aaa;
  S(int x = 0) try : a(1), aa(0)/* throw */, aaa(2) {
    std::cout << "S\n";
  } catch (...) {
    std::cout << "Caught!\n";
  }

  ~S() {std::cout << "~S\n"; }
};

int main() {
  try { S(0);} // Вывод: A A ~A Caught! wtf
  catch (...) {std::cout << "wtf\n";} // В случае S(0) бросится (throw;) по правилам, потому что S в limbo
}

void f() try {

} catch(...) { ... }

// -------------------------------------------------------------------------

struct S{
  ~S() {
    std::cout << "~S\n";
    throw 1;
  }
};

// Будет std::terminate, потому что здесь стоит неявно noexcept(true)

struct S{
  ~S() noexcept(false) {
    std::cout << "~S\n";
    throw 1;
  }
};

// А так ловится это исключение

struct S{
  ~S() noexcept(false) {
    std::cout << "~S\n";
    throw 1;
  }
};

int main() {
  try { S s; throw 1; }
  catch (...) {}
}

// А вот так хана, потому что нет обработчика исключений для деструктора

struct S{
  ~S() noexcept(false) {
    std::cout << "~S\n";
    if (!std::uncaught_exceptions()) throw 1;
  }
};

// Можно проверить, летит ли исключение

void foo() { throw 2; }

struct S{
  ~S() noexcept(false/true) {
    std::cout << "~S\n";
    try { foo(); }
    catch (...) {}
  }
};

int main() {
  try { S s; throw 1; }
  catch (...) {}
}

// Пример, когда одновременно летит 2 исключения

// НО: Если во время unwinding stack (развёртывание стека) произойдёт ещё 1 исключение в деструкторах, то ГГ

// -------------------------------------------------------------------------

void f() noexcept(true)  {}
void g() noexcept(false) {}

int main() {
  std::cout << noexcept(f()) << ' ' << noexcept(g()) << '\n'; // 1 0
}

// noexcept - когда в коде вообще ничего пойти не так не может, поэтому operator[] в vector не помечен noexcept

// -------------------------------------------------------------------------

struct S {
  S() noexcept(true) {}
  ~S() noexcept(false) {}
};

// -------------------------------------------------------------------------

/*
Гарантии безопасности: 
NoThrow (std::vector.begin()... move семантика, .empty())
Strong (если прилетает исключение, то всё возвращается, как было) (std::vector::push_back)
Basic (можно продолжать, но объект не факт, что будет такм же, каким был) (std::sort)
No exception guarantee (UB при дальнейшей работе) (std::terminate)



Ошибки - общее понятие, в которое входят исключения, но исключения ловятся с определённым сообщением
исключения - throw
*/