#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define TERM 1
#define NON_TERM 2
using namespace std;

struct syb // symbol
{
  char ch;
  int type; // either Term or non_term. Term stands for terminal.
};

struct prod { // production
  syb left;
  vector<syb> right;
};

int is_term(char c) {
  if (isupper(c))
    return NON_TERM;
  else
    return TERM;
}
void read_grammar(ifstream &fin, vector<prod> &grammar) {
  string temp_s;
  prod temp_p;

  while (!fin.eof()) {
    syb temp_sym;
    temp_p.right.clear();
    fin >> temp_s;
    temp_sym.ch = temp_s[0];
    temp_sym.type = is_term(temp_s[0]);
    temp_p.left = temp_sym;

    for (auto it = temp_s.begin() + 3; it != temp_s.end(); it++) {
      temp_sym.ch = *it;
      temp_sym.type = is_term(*it);
      temp_p.right.push_back(temp_sym);
    }
    grammar.push_back(temp_p);
  }
}
void print_grammar(ofstream &fout, vector<prod> const &grammar) {
  for (auto it = grammar.begin(); it != grammar.end(); it++) {
    fout << it->left.ch << "->";
    for (auto it_ = it->right.begin(); it_ != it->right.end(); it_++) {
      fout << it_->ch;
    }
    fout << endl;
  }
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

  vector<prod> grammar;
  read_grammar(fin, grammar);
  print_grammar(fout, grammar);
  return 0;
}