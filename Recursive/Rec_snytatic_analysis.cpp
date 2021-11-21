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
set<int> Term;
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
  start_syb = grammar[0].left;
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
      Non_Term.insert(new_nonp_syb.ch);

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
int recur_ana(ofstream &fout, string input, vector<syb> cur,
              vector<prod> &grammar, vector<prod> path) {
  auto it = cur.begin();
  vector<syb> temp;
  vector<prod> temp_p;
  //   if (cur.size() > input.length() * 2)
  //     return 0;
  int succeed = 0;
  int flag = 0; // if cur is all terminate symbols
  int q = 0;
  for (int i = 0; it != cur.end(); i++) {
    if (it->type == NON_TERM) {
      flag++;
      for (auto it_ = grammar.begin(); it_ != grammar.end(); it_++) {

        if (it_->left.ch == it->ch) {
          temp_p = path;
          temp_p.push_back(*it_);
          temp = cur;
          auto _it = temp.begin() + i;
          temp.erase(_it);
          temp.insert(_it, it_->right.begin(), it_->right.end());
          succeed = recur_ana(fout, input, temp, grammar, temp_p);

          if (succeed > 0) {
            return 1;
          }
        }
      }
    } else {
      if (map_syb_fun(it->ch) != string(1, input[q]))
        return 0;
      else
        q++;
    }
    it++;
  }
  if (flag == 0) {
    string temp_str;
    temp_str = "";
    for (auto it = cur.begin(); it != cur.end(); it++) {
      temp_str += map_syb_fun(it->ch);
    }
    if (temp_str == input) {
      ans_path = path;
      return 1;
    }
  }
  return 0;
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
  int succeed;
  read_grammar(fin, grammar);
  rmv_left_rec(grammar);
  print_grammar(fout, grammar);
  cout << "please enter the string" << endl;
  cin >> input;

  vector<syb> cur;

  cur.push_back(start_syb);
  succeed = recur_ana(fout, input, cur, grammar, path);
  if (succeed) {
    print_grammar(fout, ans_path);
  } else
    cout << "Invalid input" << endl;
  return 0;
}