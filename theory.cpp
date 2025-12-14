/*
литералы - 5, 5.0, 's', "rif"
идентификаторы - названия переменных

управляющие конструкции - if, else, else is, switch, циклы...

// Unspecifind Behaviour - разрешено, но документация компилятора не говорит об этом

// Implementation Behavior - разрешено по докупентации компилятора

if (init-statement; bool-expr) {
  statement;
}


int foo(int a, bool c) {return 10;}
int foo2(int, bool) { return 20;}

int main() {
  int(*ptr)(int, bool) = &foo;
  ptr(10, true); // вызвали
  ptr = foo2; // переприсвоили (потому что совпадают сигнатуры)
}

c-style cast:
const_cast<T>(expr)
static_cast<T>(expr)
static_cast<U>(expr) + const_cast<T>(…)
reinterpret_cast<T>(expr)
reinterpret_cast<U>(expr) + const_cast<T>(…)

struct A {
    int x;
    A() { x = 29; }
    A(int, int) {}
    A(int c) : A() {}
    void foo() {}
};
int main() {
  void(A::*ptr)() = &A::foo;
  int A::*p = &A::x;
}

*/