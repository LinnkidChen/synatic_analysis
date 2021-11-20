#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
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

class prod { // production
public:
  syb left;
  vector<syb> right;
  bool operator<(const prod &obj) const { return left.ch < obj.left.ch; }
};

map<string, int> syb_map;
set<int> non_term;
set<int> term;
// vector<f_set> first;
map<int, set<syb>> follow;
map<int, set<syb>> first;
map<pair<int, int>, set<prod>> predict;
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
  int change_flag = 1;
  while (change_flag) {
    change_flag = 0;
    for (auto it = grammar.begin(); it != grammar.end(); it++) {
      for (auto it_ = it->right.begin(); it_ != it->right.end(); it_++) {
        if (it_->type == NON_TERM) {
          if (it_ + 1 == it->right.end()) { // A->aB
            for (auto it__ = follow[it->left.ch].begin();
                 it__ != follow[it->left.ch].end(); it__++) {
              if (follow[it_->ch].insert(it__->ch).second) {
                change_flag++;
              }
            }
          } else if ((it_ + 1)->type == NON_TERM) { // A->aBb
                                                    // A->aBb,empty in first(b)
            if (first[(it_ + 1)->ch].find(empty_ch) !=
                first[(it_ + 1)->ch].end()) {
              for (auto it__ = follow[it->left.ch].begin();
                   it__ != follow[it->left.ch].end(); it__++) {
                if (follow[it_->ch].insert(it__->ch).second) {
                  change_flag++;
                }
              }
            }
            // A->aBb, adding first b to follow B
            for (auto it__ = first[(it_ + 1)->ch].begin();
                 it__ != first[(it_ + 1)->ch].end(); it__++) {
              if (it__->ch != empty_ch)
                if (follow[it_->ch].insert(it__->ch).second) {
                  change_flag++;
                }
            }
          } else {

            if (follow[it_->ch].insert((it_ + 1)->ch).second) {
              change_flag++;
            }
          }
        }
      }
    }
  }
}

void gen_predict_table(vector<prod> const &grammar) {
  for (auto gen_it = grammar.begin(); gen_it != grammar.end();
       gen_it++) { // for every production
    if (non_term.find(gen_it->right[0].ch) != non_term.end()) {
      for (auto fir_it = first[gen_it->right[0].ch].begin();
           fir_it != first[gen_it->right[0].ch].end(); fir_it++) {
        if (fir_it->ch == empty_ch) {
          for (const auto &prd : follow[gen_it->left.ch]) {
            predict[make_pair(gen_it->left.ch, prd.ch)].insert(*gen_it);
          }
        } else { // add A->B for every first(B) which is not empty
          predict[make_pair(gen_it->left.ch, fir_it->ch)].insert(*gen_it);
        }
      }
    } else if (gen_it->right[0].ch == empty_ch) {
      for (const auto &prd : follow[gen_it->left.ch]) {
        predict[make_pair(gen_it->left.ch, prd.ch)].insert(*gen_it);
      }
    } else {
      predict[make_pair(gen_it->left.ch, gen_it->right[0].ch)].insert(*gen_it);
    }
  }
}
void print_predict(ofstream &fout) {
  for (const auto &tem : term) {
    for (const auto &nontem : non_term) {
      if (predict.find(make_pair(nontem, tem)) != predict.end()) {
        cout << "[" << map_syb_fun(nontem) << "," << map_syb_fun(tem) << "] :";
        print_prod(fout, *(predict[make_pair(nontem, tem)].begin()));
        cout << endl;
      }
    }
  }
  int $_ch = map_syb_fun("$");
  for (const auto &nontem : non_term) {
    if (predict.find(make_pair(nontem, $_ch)) != predict.end()) {
      cout << "[" << map_syb_fun(nontem) << "," << map_syb_fun($_ch) << "] :";
      print_prod(fout, *(predict[make_pair(nontem, $_ch)].begin()));
      cout << endl;
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
  rmv_left_rec(grammar);

  cout << "please enter the string" << endl;
  // cin >> input;
  input = "1+1+2+3+3+1+2";
  vector<syb> cur;

  gen_first(grammar);
  gen_follow(grammar);
  gen_predict_table(grammar);
  print_predict(fout);

  cur.push_back(start_syb);
  //   print_grammar(fout, grammar);
  return 0;
}