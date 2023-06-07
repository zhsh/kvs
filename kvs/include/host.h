#include <cstdlib>
#include <stdio.h>

class SerialIO {
  public:
  inline void begin(int) {}
  inline void print(int a) { printf("%d", a); }
  inline void print(int a, int fmt) { printf("%x", a); }
  inline void print(const char* a) { printf("%s", a); }
  inline void println(int a) { printf("%d\n", a); }
  inline void println(int a, int fmt) { printf("%x\n", a); }
  inline void println(const char* a) { printf("%s\n", a); }
  inline void println() { printf("\n"); }
};

inline void delay(int) {}
inline void fail() { abort(); }

extern SerialIO Serial;

