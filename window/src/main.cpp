#include <iostream>
#include "Window.h"

int main(int argc, char *argv[]) {
  Window win("Debug", {800, 650}, {0.05f, 0.05f, 0.05f});
  return win.run();

}
