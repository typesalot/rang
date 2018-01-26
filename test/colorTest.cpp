#include "rang.hpp"
#include <string>

using namespace std;
using namespace rang;

void printHeading(string heading)
{
    cout << '\n'
         << style::reset << heading << style::reset << bg::reset << fg::reset
         << endl;
}

void test_colors(ostream &os, const winTerm opt)
{
    setWinTermMode(opt);

    printHeading("Text Style Test:");
    os << style::bold << " Bold " << style::reset;
    os << style::italic << " Italic " << style::reset;
    os << style::underline << " Underlined " << style::reset;
    os << style::dim << " Dim " << style::reset;
    os << style::conceal << " Conceal " << style::reset;
    os << style::reversed << " Reversed " << style::reset;
    os << style::blink << " Blink " << style::reset;
    os << style::rblink << " rBlink " << style::reset;
    os << style::crossed << " Crossed " << style::reset << endl;

    printHeading("Background Test:");
    os << bg::green << " Green " << bg::reset;
    os << bg::red << " Red " << bg::reset;
    os << bg::black << " Black " << bg::reset;
    os << bg::yellow << " Yellow " << bg::reset;
    os << bg::blue << " Blue " << bg::reset;
    os << bg::magenta << " Magenta " << bg::reset;
    os << bg::cyan << " Cyan " << bg::reset;
    os << bg::gray << " Grey " << bg::reset << endl;

    printHeading("Foreground Test:");
    os << fg::green << " Green " << fg::reset;
    os << fg::red << " Red " << fg::reset;
    os << fg::black << " Black " << fg::reset;
    os << fg::yellow << " Yellow " << fg::reset;
    os << fg::blue << " Blue " << fg::reset;
    os << fg::magenta << " Magenta " << fg::reset;
    os << fg::cyan << " Cyan " << fg::reset;
    os << fg::gray << " Grey " << fg::reset << endl;

    printHeading("Bright Background Test:");
    os << bgB::green << " Green ";
    os << bgB::red << " Red ";
    os << bgB::black << " Black ";
    os << bgB::yellow << " Yellow ";
    os << bgB::blue << " Blue ";
    os << bgB::magenta << " Magenta ";
    os << bgB::cyan << " Cyan ";
    os << bgB::gray << " Grey " << style::reset << endl;

    printHeading("Bright Foreground Test:");
    os << fgB::green << " Green ";
    os << fgB::red << " Red ";
    os << fgB::black << " Black ";
    os << fgB::yellow << " Yellow ";
    os << fgB::blue << " Blue ";
    os << fgB::magenta << " Magenta ";
    os << fgB::cyan << " Cyan ";
    os << fgB::gray << " Grey " << style::reset << endl;
}

void enumerateWinTerms()
{
    cout << endl;
    cout << "_________________________________________________________________";
    cout << "\n\n"
         << style::reset << style::bold << "Printing for WinTerm = Auto"
         << style::reset << bg::reset << fg::reset << '\n';
    cout << "_________________________________________________________________";
    test_colors(cout, winTerm::Auto);
    test_colors(clog, winTerm::Auto);
    test_colors(cerr, winTerm::Auto);
    cout << "-------------------------------------------------------------\n\n";

    cout << endl;
    cout << "_________________________________________________________________";
    cout << "\n\n"
         << style::reset << style::bold << "Printing for WinTerm = Ansi"
         << style::reset << bg::reset << fg::reset << '\n';
    cout << "_________________________________________________________________";
    test_colors(cout, winTerm::Ansi);
    test_colors(clog, winTerm::Ansi);
    test_colors(cerr, winTerm::Ansi);
    cout << "-------------------------------------------------------------\n\n";

    cout << endl;
    cout << "_________________________________________________________________";
    cout << "\n\n"
         << style::reset << style::bold << "Printing for WinTerm = Native"
         << style::reset << bg::reset << fg::reset << '\n';
    cout << "_________________________________________________________________";
    test_colors(cout, winTerm::Native);
    test_colors(clog, winTerm::Native);
    test_colors(cerr, winTerm::Native);
    cout << "-------------------------------------------------------------\n\n";
}

int main()
{
    cout << "\n\n\n"
         << style::reset << style::underline << style::bold << "Control = Auto"
         << style::reset << bg::reset << fg::reset << endl;
    setControlMode(control::Auto);
    enumerateWinTerms();

    cout << "\n\n\n"
         << style::reset << style::underline << style::bold
         << "Control = Force " << style::reset << bg::reset << fg::reset
         << endl;
    setControlMode(control::Force);
    enumerateWinTerms();

    cout << "\n\n\n"
         << style::reset << style::underline << style::bold << "Control = Off "
         << style::reset << bg::reset << fg::reset << endl;
    setControlMode(control::Off);
    enumerateWinTerms();
}
