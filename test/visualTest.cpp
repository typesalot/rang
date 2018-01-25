#include "rang.hpp"
#include <string>

using namespace std;
using namespace rang;

int main()
{
    cout << endl
         << style::reset << bg::green << fg::gray
         << "If you're seeing green background, then rang works!"
         << style::reset << endl;

    cerr << endl
         << style::reset << bg::red << fg::gray
         << "If you're seeing red background, then rang works!" << style::reset
         << endl;

    clog << endl
         << style::reset << bg::blue << fg::gray
         << "If you're seeing blue background, then rang works!" << style::reset
         << endl;
}
