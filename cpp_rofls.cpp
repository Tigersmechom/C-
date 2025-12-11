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
