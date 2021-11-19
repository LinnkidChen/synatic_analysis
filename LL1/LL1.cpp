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

struct prod { // production
  syb left;
  vector<syb> right;
};

map<string, int> syb_map;
set<int> non_term;
set<int> term;
// vector<f_set> first;
map<int, set<syb>> follow;
map<int, set<syb>> first;
syb start_syb;
vector<prod> ans_path;
int empty_ch;
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
  empty_ch = map_syb_fun("");
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
      non_term.insert(new_nonp_syb.ch);

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
      temp_p.right.push_back(temp_s);
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
int first_procs_non_term(int i, prod const &prdtn) {
  int flag_empty = 0;
  int succeed = 0;
  syb temp = prdtn.right[i];
  for (auto it = first[temp.ch].begin(); it != first[temp.ch].end(); it++) {
    if (it->ch != empty_ch) { // not null
      if (first[prdtn.left.ch].insert(*it).second)
        succeed++;
    } else
      flag_empty++;
  }
  if (flag_empty) {
    if (i < prdtn.right.size())
      first_procs_non_term(i + 1, prdtn);
    else
      first[prdtn.left.ch].insert(syb(empty_ch, TERM));
  }
  return succeed;
}

void gen_first(vector<prod> const &grammar) {
  set<syb> temp_syb;
  for (auto it = non_term.begin(); it != non_term.end(); it++) {

    first[*it];
  } // initial set

  int change_flag = 1;
  while (change_flag) {
    change_flag = 0;
    for (auto it = grammar.begin(); it != grammar.end(); it++) {
      if (term.find(it->right[0].ch) != term.end() ||
          it->right[0].ch == empty_ch) {

        if (first[it->left.ch].insert(it->right[0]).second) // insert success
        {
          change_flag++;
        }
      } else
        change_flag += first_procs_non_term(0, *it);
    }
  }
}

void gen_follow(vector<prod> const &grammar) {
  follow[start_syb.ch].insert(map_syb_fun("$"));
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
  rmv_left_rec(grammar);

  cout << "please enter the string" << endl;
  // cin >> input;
  input = "1+1+2+3+3+1+2";
  vector<syb> cur;

  gen_first(grammar);
  gen_follow(grammar);
  // gen pridict_table

  cur.push_back(start_syb);
  print_grammar(fout, grammar);
  return 0;
}