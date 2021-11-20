#include <assert.h>

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#define set unordered_set
#define  map unordered_map
using namespace std;
struct node {
      char left;
      string right;
}; //存放产生式
int T;
struct node product[120];          //产生式
set<char> First[120], Follow[120]; // First集合,Follow集合
vector<char> Terminal_NoEmpty, Terminal,
    NoTerminal; //除去空的终结符号,终结符号,非终结符号
class LL1 {
private:
      vector<char> AnalyseStack; //分析栈
      vector<char> LeftString;   //剩余字符串
      int Table[105][105];       //预测表
public:
  LL1();
  void AnalyseSolve();              //分析处理
      void getTable();              //生成预测表
      void AnalyseString(string s); //分析表达式
      void PrintTable();            //输出预测表
};
class SLR1 {
      private :
        vector<int> StateStack;     //状态栈
          vector<char> SymbolStack;  //符号栈
          map<char, int> NoTerminal; //为非终结符号哈希
          map<char, int> Terminal;   //为终结符号哈希
          int Action[105][105]; //状态i遇到终结符j后的动作， S, R
          int NextState[105][105]; //状态i遇到终结符j后的状态,S1中的1
          int Goto[105][105]; //状态i遇到非终结符号j后到达的状态
          void GetReduce(
      int To, char &need_into, int &need_out, string &ReduceFormula);
      public :
        SLR1();
          void AnalyseString();
};
bool isNoTerminal(char ch) { //判断是不是非终结符
      if  (ch >= 'A' && ch <= 'Z') {
            return true;
        
  }
      return false;
}
int getNIndex(char ch) { //得到非终结符的位置{
    for (int i = 0; i < NoTerminal.size(); i++) {
          if  (NoTerminal[i] == ch) {
                return i;
            
  }
      
}
    return - 1;
}
int getIndex(char ch) { //得到终结符的位置
    for (int i = 0; i < Terminal.size(); i++) {
          if  (Terminal[i] == ch) {
                return i;
            
  }
      
}
    return - 1;
}
void getFirst(char ch) //求出字符的first集合
{
      int cntEmpty = 0, isEmpty = 0, index1 = getNIndex(ch);
    for (int i = 0; i < T; i++) {
          if  (product[i].left != ch) //跳过
            continue;
          if  (!isNoTerminal(product[i].right[0])) { //若右端直接是终结符
                First[index1].insert(
        product[i].right[0]); //把终结符加入到First集合
            
  }
  else  {
            for (int j = 0; j < product[i].right.size(); j++){
                  char now = product[i].right[j];
                  if  (!isNoTerminal(now)) { //遇到终结符填入first集合
                        First[index1].insert(now);
                        break;
                    
  }
                   //此时now是非终结符
                getFirst(now); //递归求first集合
                  int index2 = getNIndex(now);
                for (set<char>::iterator it = First[index2].begin(); it != First[index2].end(); it++) {
                      if  (*it == '$') { //看生成空吗
                            isEmpty = 1;
                        
  }
  else  { //把第一个now的非空First集合加到左侧符号的First集合
                            First[index1].insert(*it);
                        
  }
                  
}
                if  (!isEmpty)
                    break;
                else  { //不生成空,继续循环下一个符号
                      cntEmpty += isEmpty;
                      isEmpty = 0;
                  
}
            
}
            if  (cntEmpty == product[i].right.length())
                First[index1].insert(
    '$'); //如果每一个非终结符号都可置空，则加入空
        
  }
      
}
}
void getFollow(char ch) {         //求出follow集合
      int index1 = getNIndex(ch); //得到ch在非终结符号表中的位置
    for (int i = 0; i < T; i++) {
          int index2 = - 1; //用来表示ch在产生式右部出现的位置
        for (int j = 0; j < product[i].right.length(); j++) {
              if  (product[i].right[j] == ch) {
                    index2 = j;
                    break;
                
  }
          
}
   
        if  (
    index2 != - 1 && index2 != product[i]
                                      .right
                                      .length() - 1) { //在产生式右部出现但不是最后一位
              char now = product[i].right[index2 + 1]; // char后面紧跟着的符号
              if  (!isNoTerminal(now)) {               //如果是终结符
                    Follow[index1].insert(now);        //填入follow集合中
                
  }
  else  { //非终结符号
                    int FlagEmpty = 0, index3 = getNIndex(
                                           now); //查找now在非终结符号的位置
                for (set<char>::iterator it = First[index3].begin(); it != First[index3].end(); it++) {
                      if  (*it == '$')
                        FlagEmpty = 1;
                      else                         Follow[index1].insert(*it);
                  
} //将非终结符now的first集合填入follow中
                if  (FlagEmpty && product[i].left != ch) { //如果有空
                      getFollow(product[i].left);
                      int index4 = getNIndex(product[i].left);
                    for (set<char>::iterator it = Follow[index4].begin(); it != Follow[index4].end(); it++) {
                          Follow[index1].insert(*it);
                      
}
                
}
            
  } //如果是最后一个位置
          
}
else  if  (
    index2 != - 1 && index2 == product[i]
                                      .right
                                      .length() - 1 && ch != product[i].left) {
              getFollow(product[i].left);
              int index5 = getNIndex(product[i].left);
            for (set<char>::iterator it = Follow[index5].begin(); it != Follow[index5].end(); it++) {
                  Follow[index1].insert(*it);
              
} //将左侧符号的follow赋值给ch的follow集
        
}
    
}
}
void SolveFirstandFollow() {
      string s;
      cout << "input number" << endl;
      cin >> T;
      cout << "input productions" << endl;
    for (int i = 0; i < T; i++) {
          cin >> s;
          string temp = "";
        for (int j = 0; j < s.length(); j++) {//去空格
              if  (s[j] == ' ')
                continue;
              temp += s[j];
          
}
        product[i].left = temp[0];             //保存产生式左部
        for (int j = 3; j < temp.length(); j++)//保存产生式右部
            product[i].right += temp[j];        
        for (int j = 0; j < temp.length(); j++) {
              if  (j == 1 || j == 2)
                continue;
              if  (isNoTerminal(temp[j])) {
                     //非终结符号
                int find = 0; //是否已经插入了
                for (int k = 0; k < NoTerminal.size(); k++) {
                      if  (temp[j] == NoTerminal[k]) {
                            find = 1;
                            break;
                        
  }
                  
}
                if  (!find) { //若非终结符表中没有
                      NoTerminal.push_back(temp[j]);
                  
}
            
  }
  else  { //终结符
                    int find = 0;
                for (int k = 0; k < Terminal.size(); k++) {
                      if  (temp[j] == Terminal[k]) {
                            find = 1;
                            break;
                        
  }
                  
}
                if  (!find) { //若终结符表中没有
                      Terminal.push_back(temp[j]);
                  
}
            
  }
          
}
    
}
    Terminal.push_back('#');
    for (int i = 0; i < Terminal.size(); i++) {
          if  (Terminal[i] != '$') //非空
            Terminal_NoEmpty.push_back(Terminal[i]);
      
}
    //获得First集合
    for (int i = 0; i < NoTerminal.size(); i++) {
          getFirst(NoTerminal[i]);
      
}
    for (int i = 0; i < NoTerminal.size(); i++) {
          if  (i == 0)
            Follow[0].insert('#'); //在开始符号中加入结束符号
          getFollow(NoTerminal[i]);
      
}
}
void PrintFirstandFollow() {
      cout << "First" << "\n";
    for (int i = 0; i < NoTerminal.size(); i++) {
          cout << NoTerminal[i] << ":  ";
        for (set<char>::iterator it = First[i].begin(); it != First[i].end(); it++) {
              cout << *it << " ";
          
}
        cout << endl;
    
}
    cout << endl;
    cout << "Follow:" << endl;
    for (int i = 0; i < NoTerminal.size(); i++) {
          cout << NoTerminal[i] << ":  ";
        for (set<char>::iterator it = Follow[i].begin(); it != Follow[i].end(); it++) {
              cout << * it << " ";
          
}
        cout << endl;
    
}
    cout << endl;
}
LL1::LL1() {
      memset(Table, - 1, sizeof(Table)); //初始化Table
}
void LL1::getTable() {
    for (int i = 0; i < T; i++) {//分析每一个产生式
          int index1 = getNIndex(product[i].left);
          int cntEmpty = 0;
        for (int j = 0; j < product[i].right.length(); j++) {//对First(a)中每一个终结符加入符号表
              char now = product[i].right[j];
              if  (!isNoTerminal(now)) { //空的情况放在后面记录
                    if  (now != '$')
                    Table[index1][getIndex(now)] = i;
                    else  {
                          cntEmpty++;
                          Table[index1][getIndex(now)] = i;
                      
    }
                    break;
                
  }
  else  {
                     //非终结符号
                int index2 = getNIndex(now);
                    int flag = 0;
                for (set<char>::iterator it = First[index2].begin(); it != First[index2].end(); it++) {
                      Table[index1][getIndex(*it)] = i;
                      if  (*it == '$') {
                            flag = 1;
                            cntEmpty++;
                        
  }
                  
}
                if  (!flag) {
                      break; //不可制空，直接break;
                  
}
            
  }
          
}
        if  (cntEmpty == product[i].right.size()) {
            for (set<char>::iterator it = Follow[index1].begin(); it != Follow[index1].end(); it++) {
                  Table[index1][getIndex(*it)] = i;
              
}
        
}

    
}
}
void LL1::AnalyseString(string s) { //分析输入串
      LeftString.clear();
    for (int i = 0; i < s.length(); i++)
        LeftString.push_back(s[i]);
    LeftString.push_back('#');
    AnalyseStack.push_back('#');
    AnalyseStack.push_back(NoTerminal[0]); //加入第一个非终结符号
    while (AnalyseStack.size() > 0) {
           //输出当前的分析栈
        string Out = "";
        for (int i = 0; i < AnalyseStack.size(); i++)
            Out += AnalyseStack[i];
        cout << setw(20) << Out;
         //输出剩余字符串
        Out = "";
        for (int i = 0; i < LeftString.size(); i++)
            Out += LeftString[i];
        cout << setw(20) << Out;
         //匹配
        char ch1 = AnalyseStack.back();
        char ch2 = LeftString.front();
        if  (ch1 == ch2 && ch1 == '#') {
              cout << setw(22) << "Accepted!\n";
              return;
          
}
        if  (ch1 == ch2) {
              AnalyseStack.pop_back();
              LeftString.erase(LeftString.begin());
              cout << setw(20) << "Matched " << ch1 << "!\n";
          
}
        else  if  (Table[getNIndex(ch1)][getIndex(ch2)] != - 1) {
               //可以用产生式转移
            int k = Table[getNIndex(ch1)][getIndex(ch2)]; //用第几个产生式
              AnalyseStack.pop_back();
              if  (product[k].right != "$") {
                for (int i = product[k].right.length() - 1; i >= 0; i--) {//反向入栈
                      AnalyseStack.push_back(product[k].right[i]);
                  
}
            
  }
              cout << setw(20) << "Infer:  " << product[k].left << "->" 
                   << product[k].right << "\n";
          
}
        else  {
               //出错了
            cout << setw(20) << "Error!\n";
              return;
          
}
    
}
}
void LL1::PrintTable() { //输出表
      cout << "  ";
    for (int i = 0; i < Terminal.size(); i++) {
          cout << setw(10) << Terminal[i];
      
}
    cout << "\n";
    for (int i = 0; i < NoTerminal.size(); i++) {
          cout << NoTerminal[i] << ":";
        for (int j = 0; j < Terminal.size(); j++) {
              if  (Table[i][j] != - 1) {
                    cout << setw(10) << product[Table[i][j]].right;
                
  }
              else                 cout << "       ";
          
}
        cout << endl;
    
}
    cout << endl;
}
void LL1::AnalyseSolve() {
      SolveFirstandFollow();
      PrintFirstandFollow();
      getTable();
      PrintTable();
      string s;
      cout << "input string" << "\n";
      cin >> s;
      cout << setw(20) << "stack" << setw(20) << "leftstring" << setw(20) 
           << "process" << "\n";
      AnalyseString(s);
}
void SLR1::GetReduce(
    int To, char & need_into, int & need_out, string & ReduceFormula) {
       //获取规约信息
    if  (To == 1) {
            need_into = 'E';
            need_out = 3;
            ReduceFormula = "E->E+T";
        
  }
  else  if  (To == 2) {
            need_into = 'E';
            need_out = 3;
            ReduceFormula = "E->E-T";
        
  }
  else  if  (To == 3) {
            need_into = 'E';
            need_out = 1;
            ReduceFormula = "E->T";
        
  }
  else  if  (To == 4) {
            need_into = 'T';
            need_out = 3;
            ReduceFormula = "T->T*F";
        
  }
  else  if  (To == 5) {
            need_into = 'T';
            need_out = 3;
            ReduceFormula = "T->T/F";
        
  }
  else  if  (To == 6) {
            need_into = 'T';
            need_out = 1;
            ReduceFormula = "T->F";
        
  }
  else  if  (To == 7) {
            need_into = 'F';
            need_out = 3;
            ReduceFormula = "F->(E)";
        
  }
  else  if  (To == 8) {
            need_into = 'F';
            need_out = 1;
            ReduceFormula = "F->num";
        
  }
}
SLR1::SLR1() {
       //初始化SLR1分析表
    NoTerminal.clear();
      Terminal.clear();
      memset(Action, - 1, sizeof Action);
      memset(NextState, - 1, sizeof NextState);
      memset(Goto, - 1, sizeof Goto);
       //给终结符号哈希
    Terminal.insert(make_pair('+', 1));
      Terminal.insert(make_pair('-', 2));
      Terminal.insert(make_pair('*', 3));
      Terminal.insert(make_pair('/', 4));
      Terminal.insert(make_pair('0', 5));
      Terminal.insert(make_pair('1', 5));
      Terminal.insert(make_pair('2', 5));
      Terminal.insert(make_pair('3', 5));
      Terminal.insert(make_pair('4', 5));
      Terminal.insert(make_pair('5', 5));
      Terminal.insert(make_pair('6', 5));
      Terminal.insert(make_pair('7', 5));
      Terminal.insert(make_pair('8', 5));
      Terminal.insert(make_pair('9', 5));
      Terminal.insert(make_pair('(', 6));
      Terminal.insert(make_pair(')', 7));
      Terminal.insert(make_pair('#', 8));
       //哈希非终结符号
    NoTerminal.insert(make_pair('E', 1));
      NoTerminal.insert(make_pair('T', 2));
      NoTerminal.insert(make_pair('F', 3));
      Action[0]
            [5] = 1,
               Action[0]
                     [6] = 1; //构造分析表1 ：表示移进 2 ：表示规约 3 ：表示接受
      Action[1][1] = 1, Action[1][2] = 1, Action[1][8] = 3;
      Action[2][1] = 2, Action[2][2] = 2, Action[2][3] = 1, Action[2][4] = 1,
                  Action[2][7] = 2, Action[2][8] = 2;
      Action[3][5] = 1, Action[3][6] = 1;
      Action[4][1] = 2, Action[4][2] = 2, Action[4][3] = 2, Action[4][4] = 2,
                  Action[4][7] = 2, Action[4][8] = 2;
      Action[5][5] = 1, Action[5][6] = 1;
      Action[6][5] = 1, Action[6][6] = 1;
      Action[7][5] = 1, Action[7][6] = 1;
      Action[8][5] = 1, Action[8][6] = 1;
      Action[9][1] = 2, Action[9][2] = 2, Action[9][3] = 2, Action[9][4] = 2,
                  Action[9][7] = 2, Action[9][8] = 2;
      Action[10][1] = 1, Action[10][2] = 1, Action[10][7] = 1;
      Action[11][1] = 2, Action[11][2] = 2, Action[11][3] = 2,
                   Action[11][4] = 2, Action[11][7] = 2, Action[11][8] = 2;
      Action[12][1] = 2, Action[12][2] = 2, Action[12][3] = 1,
                   Action[12][4] = 1, Action[12][7] = 2, Action[12][8] = 2;
      Action[13][1] = 2, Action[13][2] = 2, Action[13][3] = 1,
                   Action[13][4] = 1, Action[13][7] = 2, Action[13][8] = 2;
      Action[14][1] = 2, Action[14][2] = 2, Action[14][3] = 2,
                   Action[14][4] = 2, Action[14][7] = 2, Action[14][8] = 2;
      Action[15][1] = 2, Action[15][2] = 2, Action[15][3] = 2,
                   Action[15][4] = 2, Action[15][7] = 2, Action[15][8] = 2;

      NextState[0][5] = 4, NextState[0][6] = 3;
      NextState[1][1] = 5, NextState[1][2] = 6;
      NextState[2][1] = 3, NextState[2][2] = 3, NextState[2][3] = 7,
                     NextState[2][4] = 8, NextState[2][7] = 3,
                     NextState[2][8] = 3;
      NextState[3][5] = 4, NextState[3][6] = 3;
      NextState[4][1] = 8, NextState[4][2] = 8, NextState[4][3] = 8,
                     NextState[4][4] = 8, NextState[4][7] = 8,
                     NextState[4][8] = 8;
      NextState[5][5] = 4, NextState[5][6] = 3;
      NextState[6][5] = 4, NextState[6][6] = 3;
      NextState[7][5] = 4, NextState[7][6] = 3;
      NextState[8][5] = 4, NextState[8][6] = 3;
      NextState[9][1] = 6, NextState[9][2] = 6, NextState[9][3] = 6,
                     NextState[9][4] = 6, NextState[9][7] = 6,
                     NextState[9][8] = 6;
      NextState[10][1] = 5, NextState[10][2] = 6, NextState[10][7] = 11;
      NextState[11][1] = 7, NextState[11][2] = 7, NextState[11][3] = 7,
                      NextState[11][4] = 7, NextState[11][7] = 7,
                      NextState[11][8] = 7;
      NextState[12][1] = 1, NextState[12][2] = 1, NextState[12][3] = 7,
                      NextState[12][4] = 8, NextState[12][7] = 1,
                      NextState[12][8] = 1;
      NextState[13][1] = 2, NextState[13][2] = 2, NextState[13][3] = 7,
                      NextState[13][4] = 8, NextState[13][7] = 2,
                      NextState[13][8] = 2;
      NextState[14][1] = 4, NextState[14][2] = 4, NextState[14][3] = 4,
                      NextState[14][4] = 4, NextState[14][7] = 4,
                      NextState[14][8] = 4;
      NextState[15][1] = 5, NextState[15][2] = 5, NextState[15][3] = 5,
                      NextState[15][4] = 5, NextState[15][7] = 5,
                      NextState[15][8] = 5;

      Goto[0][1] = 1, Goto[0][2] = 2, Goto[0][3] = 9;
      Goto[3][1] = 10, Goto[3][2] = 2, Goto[3][3] = 9;
      Goto[5][2] = 12, Goto[5][3] = 9;
      Goto[6][2] = 13, Goto[6][3] = 9, Goto[7][3] = 14, Goto[8][3] = 15;
       //输出SLR1分析表
    cout << "                                         Action" << "\n";
      cout << setw(10) << "State";
      cout << setw(10) << "+";
      cout << setw(10) << "-";
      cout << setw(10) << "*";
      cout << setw(10) << "/";
      cout << setw(10) << "num";
      cout << setw(10) << "(";
      cout << setw(10) << ")";
      cout << setw(10) << "#";
    for (int i = 0; i < 16; i++) {
          cout << setw(10) << i;
        for (int j = 1; j <= 8; j++) {
              if  (Action[i][j] == - 1) {
                    cout << "          ";
                
  }
              else  {
                    string temp = "";
                    if  (Action[i][j] == 1) {
                          temp += "S";
                          temp += to_string(NextState[i][j]);
                      
    }
                    else  if  (Action[i][j] == 2) {
                          temp += "R";
                          temp += to_string(NextState[i][j]);
                      
    }
                    else  {
                          temp += "ACC";
                      
    }
                    cout << setw(10) << temp;
                
  }
          
}
        cout << endl;
    
}
    cout << endl;
   
    cout << "                   Goto" << "\n"; //输出Goto表
    cout << setw(10) << "State";
    cout << setw(10) << "E";
    cout << setw(10) << "T";
    cout << setw(10) << "F";
    cout << endl;
    for (int i = 0; i < 16; i++) {
          cout << setw(10) << i;
        for (int j = 1; j <= 3; j++) {
              if  (Goto[i][j] == - 1) {
                    cout << "          ";
                
  }
              else  {
                    string temp = "";
                    temp += to_string(Goto[i][j]);
                    cout << setw(10) << temp;
                
  }
          
}
        cout << endl;
    
}
    cout << endl;
}
void SLR1::AnalyseString() {
      StateStack.clear();
      SymbolStack.clear();
      string s;
      cout << "input strings" << "\n";
   
    cin >> s;
     
    cout << setw(20) << "statestack" << setw(20) << "symbolstack" << setw(20) 
          << "leftstring" << setw(20) << "process" << "\n";
      string LeftString = "";
    for (int i = 0; i < s.length(); i++)
        LeftString += s[i];
    LeftString.push_back('#');
    StateStack.push_back(0);
    SymbolStack.push_back('#');
    while (1) {
          string Out = "";
        //输出分析栈
        for (int i = 0; i < StateStack.size(); i++)
            Out += to_string(StateStack[i]);
        cout << setw(20) << Out;
        Out = "";
        for (int i = 0; i < SymbolStack.size(); i++)
            Out += SymbolStack[i];
        cout << setw(20) << Out;
        Out = "";
        for (int i = 0; i < LeftString.size(); i++)
            Out += LeftString[i];
        cout << setw(20) << Out;
        int state1 = StateStack.back();
        char ch = LeftString.front();
        if  (Action[state1][Terminal[ch]] == 1) {
               //移进
            int To = NextState[state1][Terminal[ch]];
              SymbolStack.push_back(ch); //压栈
              StateStack.push_back(To);
              LeftString.erase(LeftString.begin()); //指向下一个符号
              string temp = "";
              temp += "Shift ";
              temp += to_string(To);
              cout << setw(20) << temp << "\n";
          
}
        else  if  (Action[state1][Terminal[ch]] == 2) {
               //规约
            int To = NextState[state1][Terminal[ch]];
              char need_into; //需要进栈的非终结字符
              int need_out;   //需要从栈中弹出几个字符
              string ReduceFormula;
              GetReduce(To, need_into, need_out, ReduceFormula);
            for (int i = 0; i < need_out; i++) {
                  StateStack.pop_back();
                  SymbolStack.pop_back(); //出栈
              
}
            SymbolStack.push_back(need_into);
            StateStack.push_back(
    Goto[StateStack.back()][NoTerminal[need_into]]); //入栈
            string temp = "Reduce by ";
            temp += ReduceFormula;
            cout << setw(20) << temp << "\n";
        
}
        else  if  (Action[state1][Terminal[ch]] == 3) {
              cout << setw(20) << "ACCEPTED!\n";
              return;
          
}
        else  {
              cout << setw(20) << "Error!\n";
              return;
          
}
    
}
}
int main() {
      LL1 LL;
      LL.AnalyseSolve();
      SLR1 LR;
      LR.AnalyseString();
      return 0;
