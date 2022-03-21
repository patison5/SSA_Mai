#include <iostream>
#include <exception>

#include "header/config.h"

using namespace std;

int main([[maybe_unused]] int argc, [[maybe_unused]] char**argv) {
    cout << "~~~~~~~~~~~~~~~~ hello world! ~~~~~~~~~~~~~~~~" << endl;
    cout << get_config_string() << endl;
    cout << "result: " << complex_calculation(2, 2) << endl;
    return 0;
}
