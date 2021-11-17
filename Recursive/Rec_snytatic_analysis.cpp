#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#define TERM 1
#define NON_TERM 2
using namespace std;

struct syb // symbol
{
  int ch;
  int type; // either Term or non_term. Term stands for terminal.
};

struct prod { // production
  syb left;
  vector<syb> right;
};

map<string, int> syb_map;
set<int> Non_Term;
// int is_term(char c) {
//   if (isupper(c))
//     return NON_TERM;
//   else
//     return TERM;
// }
int map_syb_fun(string ch) {

  if (syb_map.find(ch) == syb_map.end()) // not find
  {
    syb_map.insert(map<string, int>::value_type(ch, syb_map.size()));
  }
  return syb_map.find(ch)->second;
}
string map_syb_fun(int ch) {
  string rslt;
  rslt = "";
  for (auto it = syb_map.begin(); it != syb_map.end(); it++) {
    if (it->second == ch)
      rslt = it->first;
  }
  return rslt;
}
void read_grammar(ifstream &fin, vector<prod> &grammar) {
  string temp_s;
  prod temp_p;
  size_t div; // place of "->"
  while (!fin.eof()) {
    syb temp_sym;
    temp_p.right.clear();

    fin >> temp_s;
    div = temp_s.find("->");
    temp_sym.ch = map_syb_fun(temp_s.substr(0, div));
    temp_p.left = temp_sym;

    for (auto it = temp_s.begin() + 2 + div; it != temp_s.end(); it++) {
      temp_sym.ch = map_syb_fun(string(1, *it));
      temp_p.right.push_back(temp_sym);
    }
    grammar.push_back(temp_p);
  }
  for (auto it = grammar.begin(); it != grammar.end(); it++) {
    Non_Term.insert(it->left.ch);
  }
  for (auto it = grammar.begin(); it != grammar.end(); it++) {
    it->left.type = NON_TERM;
    for (auto it_ = it->right.begin(); it_ != it->right.end(); it_++) {
      if (Non_Term.find(it_->ch) != Non_Term.end()) { // is a non_terminal
                                                      // symbol
        it_->type = NON_TERM;
      } else
        it_->type = TERM;
    }
  }
  map_syb_fun("");
}
void rmv_left_rec(vector<prod> &grammar) {
  vector<prod> temp_gmr;
  for (auto it = grammar.begin(); it != grammar.end(); it++) {
    if (it->left.ch == it->right.begin()->ch) // is left recursive
    {
      string new_non_prd;
      string left = map_syb_fun(it->left.ch);
      syb new_nonp_syb, temp_s;

      prod temp_p;

      new_non_prd = left + '\'';
      new_nonp_syb.ch = map_syb_fun(new_non_prd);
      new_nonp_syb.type = NON_TERM;

      for (auto it_ = grammar.begin();
           it_ != grammar.end();) {               // for every prod in grammar
        if (map_syb_fun(it_->left.ch) == left) {  // is left recursive
          if (it_->left.ch == it_->right[0].ch) { // A->Ab
            temp_p.left = new_nonp_syb;
            for (auto it__ = it_->right.begin() + 1; it__ != it_->right.end();
                 it__++) {
              temp_p.right.push_back(*it__);
            }
            temp_p.right.push_back(new_nonp_syb);
          }      // add A'->bA'
          else { // A->b then add A->bA'
            temp_p.left = it_->left;
            temp_p.right = it_->right;
            temp_p.right.push_back(new_nonp_syb);
          }
          temp_gmr.push_back(temp_p);
          temp_p.right.clear();
          it_ = grammar.erase(it_);
        } else
          it_++;
      }
      // add A'->none
      temp_p.left = new_nonp_syb;
      temp_s.type = TERM;
      temp_s.ch = map_syb_fun("");
      temp_gmr.push_back(temp_p);
    }
  }
  for (auto it_ = temp_gmr.begin(); it_ != temp_gmr.end(); it_++) {
    grammar.push_back(*it_);
  }
}
void print_grammar(ofstream &fout, vector<prod> const &grammar) {
  for (auto it = grammar.begin(); it != grammar.end(); it++) {
    fout << map_syb_fun(it->left.ch) << "->";
    for (auto it_ = it->right.begin(); it_ != it->right.end(); it_++) {
      fout << map_syb_fun(it_->ch);
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
  rmv_left_rec(grammar);
  print_grammar(fout, grammar);
  return 0;
}