struct Base {
  int y;
  virtual void f() {}
  void h() {}
};

struct Derived : Base {
  int x;
  void f() override {}
  virtual void g() {}
};
/*
vtable Derived: (& - адрес)
&Derived typeinfo (строковое имя)
(вот сюда указывает сам vptr)
&Derived::f
&Derived::g
*/

/*
dynamic cast кидает ошибку только при указатели, со ссылкой = nullptr

Виртуальное наследование в C++ — это такой вид наследования, при котором общий базовый класс в иерархии множественного наследования хранится в объекте в единственном экземпляре.

Виртуальные функции - такие функции, что решение, о том, какую из них выбрать принимается в run time
 
 ABI-Application binary interface (как и где и что компилятор в памяти располагает)

• std: : strong_ordering : Всегда возвращает
less, greater, equal (для полного
упорядочивания, например, чисел).
• std: :weak_ordering : Возвращает less,
greater, equal , но может быть "слабым" если объекты не могут быть полностью упорядочены (например, при перегрузке).
• std: :partial_ordering: Возвращает
less, greater, equal, equivalent
(эквивалентно) или unordered (неупорядочено) (например, для чисел с плавающей точкой, включая NaN).
*/

struct Granny {
  int g;
  void h() {}
};

struct Mom : public Granny {
  int m;
  virtual void f() {}
};

struct Son : public Mom {
  int s;
  void f() override {}
};
/*
объект Son в памяти:
[ ptr ][g][m][s][padding]
^      ^
|      | сюда указывает начало Granny
сюда указывает начало мамы и сына 
То есть при dynamic_cast к бабушке, мы обратно не скастимся (у бабушки нет ссылки vptr), но static_cast сработает нормально
*/


struct Granny {
  int g;
};

struct Mom : virtual public Granny {
  int m;
};

struct Dad : virtual public Granny {
  int d;
};

struct Son : public Mom, public Dad {
  int s;
};

/*
объект Son:
[ ptr(mom) ][m][padding][ ptr (dad)][d][s][g][padding]
ptr либо напряму, либо через vtable указывает на granny
*/


/*
ромб:
     бабушка
    /      \
  мама    папа
    \      /
      сын
объект сын: [g][m][g][d][s]
*/


struct Granny {
  int g;
  virtual void f() {}
};

struct Mom : public Granny {
  int m;
};

struct Dad : public Granny {
  int d;
};

struct Son : public Mom, public Dad {
  int s;
};
/*
[ptr][g][m][ptr][g][d][s]...
^          ^
|          | dad vptr(когда к dad кастимся) (dad in son)
mom vptr (mom in son)
то есть существует 

Dad in son: [top_offset (сдвиг для non virtual thunk)][&son typeinfo][thunk (this -= top_offset; call Son::f())]

non virtual thunk - если от сына скастился к папе а потом вызываю f, ну эта штука двигает указатель в начало сына (сдвиг на 16 байт влево)
типо все обращения к полям нужно делать со сдвигом на 16 влево
*/


struct Granny {
  int g;
  virtual void f() {}
};

struct Mom : virtual public Granny {
  int m;
};

struct Dad : virtual public Granny {
  int d;
};

struct Son : public Mom, public Dad {
  int s;
};
/*
virtual_offset - как далеко мы от виртуального предка
... - padding
Son: [ptr][m]...[ptr][d][s][ptr][g]
       ^          ^          ^
       |          |      (granny in son)
       |      (dad in son)
  (mom in son)
     virtual    top 
     off_set   off_set
Son : [32]      [0]  [&Son Typeinfo] [&Son::f]
Dad : [16]      [16] [&Son Typeinfo] [&Dad f thunk]
in Son
Granny:[-]      [32] [&Son Typeinfo] [&Granny f thunk]

*/



/*
Виртуальные функции нельзя оставлять без определения (ошибка линкера) (либо вне полей класса, либо pure virtual function)
Полиморфный класс - когда есть хотя бы 1 виртуальная функция/деструктор
*/


struct Base {
  virtual void h() = 0;
  void f() {
    h();
  }
  Base () {
    h();
    std::cout << "B\n";
  }
  virtual ~Base() {
    f();
  }
};

struct Derived : public Base {
  void h() override {
    std::cout << "h\n";
  }
  Derived() {
    std::cout << "D\n";
  }
};
/*
// CE 
в конструкторах/деструкторах механизм виртуальных функций отключается (pure virtual func не реализована (=0), ошибка линкера)
Но если в конструкторе Base вызвать f(), то будет Runtime error со словами: Pure virtual function called! (=0 делает заглушку в vtable, которая ведёт в функцию, которая делает Terminate...)

создание Base (указатель на vtable Base(поэтому virtual func не работает)), потом поля родителя, потом конструктор Base -> vptr переставляется на Derived... (vptr перескакивает, чтобы не вызвать нечаяно функцию несуществующего объекта)
Отсюда берутся Construction vtable, которые отрабатывают по типу: конструирование Base внутри Derived...
virtual table of tables - таблица, в которой указано, в каком порядке эти vptr переставляются (с деструктором аналогично)(и со списком инициализации тоже)
*/

struct Base {
  virtual void f(int x = 1) {
    std::cout << x << 'b' << '\n';
  }
};

struct Derived : public Base {
  void f(int x = 2) override {
    std::cout << x << 'd' << '\n';
  }
};
/*
При вызове Base.f() будет 1d (механизм виртуальных функций на аргументы по умолчанию не распросраняется), потому что аргументы по умолчанию обрабатываются в compile time, он видя тип Base, передаёт ему аргумент (x = 1), а в runtime попадает в Derived
*/


struct A {};
struct B : private A {};
struct C : public B {
    void f() {
        A a = A() // CE
    }
};

