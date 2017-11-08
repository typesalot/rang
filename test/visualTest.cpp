#include "rang.hpp"

using namespace std;
using namespace rang;

int main()
{
    cout << endl
         << control::forceColor  // For appveyor terminal
         << style::reset << bg::green << fg::gray
         << "If you're seeing green background, then rang works!"
         << style::reset << endl;
}
