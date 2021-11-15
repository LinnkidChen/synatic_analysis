#include <cctype>
#include <fstream>
#include <iostream>

#define TERM 1
#define NON_TERM 2
using namespace std;
struct syb // symbol
{
  char ch;
  int type; // either Term or non_term. Term stands for terminal.
};

int is_term(char c) {
  if (isupper(c))
    return NON_TERM;
  else
    return TERM;
}

int main() {
  ifstream fin;
  ofstream fout;
  fin.open("input.txt");
  fout.open("output.txt");
  if (!fin.is_open() || !fout.is_open()) {
    cout << "Fail to open file" << endl;
    exit(0);
  }

  return 0;
}