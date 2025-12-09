struct A {};
struct B : private A {};
struct C : public B {
    void f() {
        A a = A() // CE
    }
};


struct A {};
struct B : public A {};
void f(A) {}
int main() {
  B b;
  f(b); // OK
}

struct A {};
struct B : private A {};
void f(A) {}
int main() {
  B b;
  f(b); // CE
}

enum class A: long{a = 10};
long f(A a) {
  return a; // CE
}