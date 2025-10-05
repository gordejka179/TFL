#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>


using namespace std;

struct Rule{
    string l;
    string r; 
    Rule(const string& left, const string& right) : l(left), r(right) {}
};


vector<Rule> allRulesT2;
vector<Rule> allRulesT;

string alphabet = "ab";

void initRulesT(){
    allRulesT.push_back(Rule("aaaa", "a"));
    allRulesT.push_back(Rule("aaab", "b"));
    allRulesT.push_back(Rule("bbba", "ba"));
    allRulesT.push_back(Rule("bbbb", "bb"));
    allRulesT.push_back(Rule("ababb", "babb"));
    allRulesT.push_back(Rule("baaba", "bba"));
    allRulesT.push_back(Rule("baabb", "bbb"));
    allRulesT.push_back(Rule("bbaaa", "baab"));
    allRulesT.push_back(Rule("bbaab", "baa"));
    allRulesT.push_back(Rule("bbabb", "abab"));
    allRulesT.push_back(Rule("baba", "baa"));
    allRulesT.push_back(Rule("babbaa", "babba"));
    allRulesT.push_back(Rule("babbab", "abb"));
}

void initRulesT2(){
    /*
    allRulesT2.push_back(Rule("aaaa", "a"));
    allRulesT2.push_back(Rule("aaab", "b"));
    allRulesT2.push_back(Rule("bbaaa", "baab"));
    allRulesT2.push_back(Rule("bbaab", "baa"));
    allRulesT2.push_back(Rule("bbabb", "abab"));
    allRulesT2.push_back(Rule("babbaa", "babba"));
    allRulesT2.push_back(Rule("bab", "abb"));
    allRulesT2.push_back(Rule("baa", "abb"));
    allRulesT2.push_back(Rule("abb", "bb"));
    allRulesT2.push_back(Rule("bba", "bb"));
    allRulesT2.push_back(Rule("bb", "ba"));
    allRulesT2.push_back(Rule("aba", "ba"));
    */
    allRulesT2.push_back(Rule("aaaa", "a"));
    allRulesT2.push_back(Rule("aaab", "b"));
    allRulesT2.push_back(Rule("aaaba", "babb"));
    allRulesT2.push_back(Rule("baba", "baab"));
    allRulesT2.push_back(Rule("bab", "baa"));
    allRulesT2.push_back(Rule("babbb", "babba"));
    allRulesT2.push_back(Rule("aaba", "baa"));
    allRulesT2.push_back(Rule("baa", "abb"));
    allRulesT2.push_back(Rule("aba", "bb"));
    allRulesT2.push_back(Rule("bb", "ba"));
    
}

// l - левая граница подстроки в s (индекс), которую нужно заменить по правилу
void executeRule(Rule rule, string& s, int l){
    //string& replace (size_t pos, size_t len, const string& str);
    int len = rule.l.length();
    s.replace(l, len, rule.r);
}

//проверим, можно ли применить правило из списка Rules
pair<bool, int> checkIsRuleMayBeApplied(Rule rule, const string& s){
    pair<bool, int> p;
    size_t found = s.find(rule.l);
    p.second = found;

    if (found != std::string::npos) {
        p.first = true;
        return p;
    }
    
    p.first = false;
    return p;
}


//приводим строку к нормальной форме
void normalize(string& s){
    for (Rule rule: allRulesT2){
        pair<bool, int> p = checkIsRuleMayBeApplied(rule, s);
        if (p.first){
            executeRule(rule, s, p.second);
            normalize(s);
        }else{
            continue;
        }
    }
}


//генерация случайного номера правила в системе T
// n - сколько всего правил
int getRandomRuleT(int n) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dist(0, n - 1);
    return dist(gen);
}

//генерация случайного номера правила в системе T2
// n - сколько всего правил
int getRandomRuleT2(int n) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dist(0, n - 1);
    return dist(gen);
}

//генерация случайной строки, заданной длины n, состоящей из символов 
//фиксированного алфавита
string getRandomString(int n, const string& alphabet) {
    string s;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dist(0, alphabet.size() - 1);
    for (int i = 0; i < n; i++){
        s += alphabet[dist(gen)];
    }
    return s;
}

// генерируем случайные 7 правил и посмотрим, в какую строку перейдём
//при такой цепочке правил из произвольно выбранной строки в системе T
pair<string, vector<int>> T(string s){
    vector<int> path;
    int n = allRulesT.size();
    for (int i = 0; i < 7; i++){
        int ruleNum = getRandomRuleT(n);
        Rule rule = allRulesT[ruleNum];
        pair<bool, int> p = checkIsRuleMayBeApplied(rule, s);
        path.push_back(ruleNum);
        if (p.first == true){
            executeRule(rule, s, p.second);
        }
    }
    
    pair<string, vector<int>> p;
    p.first = s;
    p.second = path;
    return p;
}


//будем генерировать 50000 тестов
//вернём true, если все тесты успешны,
//иначе false
//возвращаем строку, на которой была ошибка
//и последовательность правил
//ВАЖНО: 
//выбранную строку и строку, которая получится в исходной системе,
//доведём в новой системе до нормальных форм
//они должны совпасть 
pair<bool, pair<string, vector<int>>> fuzzing(){
    pair<bool, pair<string, vector<int>>> p1;
    p1.first = true;
    p1.second.first = "";
    vector<int> emptyVector;
    p1.second.second = emptyVector;
    for (int i = 0; i < 50000; i++){
        string s = getRandomString(15, alphabet);
        pair<string, vector<int>> p2 = T(s);
        string s2 = p2.first;
        vector<int> path = p2.second;
        normalize(s);
        normalize(s2);
        if (s != s2){
            p1.first = false;
            p1.second.first = s;
            p1.second.second = path;
            return p1;
        }
    }
    return p1;
}

int main(){
    initRulesT();
    initRulesT2();
    pair<bool, pair<string, vector<int>>> p = fuzzing();
    if (p.first){
        cout << "все тесты успешны" << endl;
    }else{
        cout << "найдена ошибка: " << p.second.first << " и правила:" << endl;
        for (int ruleNum: p.second.second){
            cout << ruleNum << endl;
        }
    }
}