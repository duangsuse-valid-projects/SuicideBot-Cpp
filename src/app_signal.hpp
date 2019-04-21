#pragma once

#include <stdnoreturn.h>
#include <csignal>
#include <iostream>

void reg_signal_handers() {
  signal(SIGINT, [](int _) {
    std::cerr << "Application exit" << std::endl;
    exit(0);
  });
}
