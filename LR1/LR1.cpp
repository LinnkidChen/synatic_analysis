#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#define TERM 1
#define NON_TERM 2
#define S 83
#define R 82
#define GOTO 71
#define ACC 65
using namespace std;

class syb // symbol
{
public:
  int ch;
  int type;
  syb(int ch_ = 0, int type_ = 0) {
    ch = ch_;
    type = type_;
  }
  bool operator<(const syb &obj) const { return ch < obj.ch; }

  // either Term or non_term. Term stands for terminal.
};

class prod { // production
public:
  syb left;
  vector<syb> right;
  bool operator<(const prod &obj) const { return left.ch < obj.left.ch; }
};

class action {
public:
  action(int state_ = 0, int type_ = 0) {
    state = state_;
    type = type_;
  }
  bool operator<(const action &obj) const { return state < obj.state; }

  int state;

  int type; // S, R, Goto
};

map<string, int> syb_map;
set<int> non_term;
set<int> term;
// vector<f_set> first;
map<int, set<syb>> follow;
map<int, set<syb>> first;

map<pair<int, int>, action> action_tb; //<state,syb>
map<pair<int, int>, action> goto_tb;
int state_count;
syb start_syb;
vector<prod> ans_path;

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
    non_term.insert(it->left.ch);
  }
  for (auto it = grammar.begin(); it != grammar.end(); it++) {
    it->left.type = NON_TERM;
    for (auto it_ = it->right.begin(); it_ != it->right.end(); it_++) {
      if (non_term.find(it_->ch) != non_term.end()) { // is a non_terminal
                                                      // symbol
        it_->type = NON_TERM;
      } else {
        it_->type = TERM;
        term.insert(it_->ch);
      }
    }
  }

  start_syb = grammar[0].left;
  term.insert(map_syb_fun("$"));
}

void print_prod(ofstream &fout, prod const &prod_) {
  cout << map_syb_fun(prod_.left.ch) << "->";
  for (const auto &right : prod_.right) {
    cout << map_syb_fun(right.ch);
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

void read_lr_table() {
  ifstream fin;
  fin.open("LR_table.txt");
  int s_state, e_state;
  string sybl, type;
  while (!fin.eof()) {
    fin >> s_state >> sybl >> type >> e_state;
    if (type == "S") {
      action_tb[make_pair(s_state, map_syb_fun(sybl))] = action(e_state, S);
    } else if (type == "R") {
      action_tb[make_pair(s_state, map_syb_fun(sybl))] = action(e_state, R);
    } else if (type == "G") {
      goto_tb[make_pair(s_state, map_syb_fun(sybl))] = action(e_state, GOTO);
    } else if (type == "A") {
      action_tb[make_pair(s_state, map_syb_fun(sybl))] = action(0, ACC);
    }
  }
  state_count = 16;
}
void print_lr_table() {
  cout << setiosflags(ios::right);
  cout << setw(5) << "state";
  for (const auto &t : term) {
    cout << setw(5) << map_syb_fun(t);
  }
  cout << endl;

  for (int i = 0; i < state_count; i++) {

    cout << setw(5) << i;
    for (const auto &q : term) {
      if (action_tb.find(make_pair(i, q)) != action_tb.end())
        cout << setw(4) << char(action_tb[make_pair(i, q)].type)
             << action_tb[make_pair(i, q)].state;
      else
        cout << setw(5) << " ";
    }
    cout << endl;
  }

  cout.fill('-');
  cout << setw(9 * 5) << "" << endl;
  cout.fill(' ');
  cout << setw(5) << "state";
  for (const auto &t : non_term) {
    cout << setw(5) << map_syb_fun(t);
  }
  cout << endl;
  for (int i = 0; i < state_count; i++) {

    cout << setw(5) << i;
    for (const auto &q : non_term) {
      if (goto_tb.find(make_pair(i, q)) != goto_tb.end())
        cout << setw(5) << goto_tb[make_pair(i, q)].state;
      else
        cout << setw(5) << " ";
    }
    cout << endl;
  }
}
void LR_analysis(string input) {
  input = input + "$";
  vector<int> stack_syb;
  vector<int> state_syb;
  stack_syb.push_back(map_syb_fun("$"));
  state_syb.push_back(0);
  auto inpt_it = input.begin();
  auto syb_it = stack_syb.rbegin();
  auto sta_it = state_syb.rbegin();

  while (1) {
    syb_it = stack_syb.rbegin();
    sta_it = state_syb.rbegin();
    if (action_tb[make_pair(*sta_it, map_syb_fun(string(1, *inpt_it)))].type ==
        S) {
      stack_syb.push_back(map_syb_fun(string(1, *inpt_it)));
      state_syb.push_back(
          action_tb[make_pair(*sta_it, map_syb_fun(string(1, *inpt_it)))]
              .state);
      inpt_it++;
    } else if (action_tb[make_pair(*sta_it, map_syb_fun(string(1, *inpt_it)))]
                   .type == R) {
    }
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
  vector<prod> path;
  string input;

  read_grammar(fin, grammar);
  read_lr_table();
  print_lr_table();
  // cout << "please enter the string" << endl;
  // cin >> input;
  input = "1+1";

  return 0;
}