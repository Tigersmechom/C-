#include <exception>
#include <iostream>

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

    // Logic_error - косяк юзера (например, segfault)
    // Runtime_error - ошибки, в которыз пользователь не виноват (мб случился stack_overflow)
  }
}