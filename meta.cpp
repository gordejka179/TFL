#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;

// ИНВАРИНТЫ:

//1-ое свойство: если буква b есть в строке, то при применении любого правила
//в строке всё равно будет хотя бы 1 буква b
bool f0(const string& prev, const string& curr){
    size_t found1 = prev.find("b");
    if (found1 != std::string::npos) {
        size_t found2 = curr.find("b");
        if (found2 != std::string::npos){
            return true;
        }
        return false;
    }
    return true;
}

//2-ое свойство: число букв b не увеличивается
bool f1(const string& prev, const string& curr){
    if (count(prev.begin(), prev.end(), 'b') >= count(curr.begin(), curr.end(), 'b')){
        return true;
    }
    return false;
}


struct Rule{
    string l;
    string r; 
    Rule(const string& left, const string& right) : l(left), r(right) {}
};


vector<Rule> allRulesT2;

string alphabet = "ab";


void initRulesT2(){
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
 

// генерируем случайные 7 правил и проверим, что при
//выполнении правил сохраняются инварианты
//если нет, то возвращаем прооблемную строку
//и возвращаем номер 1-ого проблемного правила
//( -1 , если всё верно)
//а также вернём номер свойства, которое не выполнилось
pair<pair<string, int>, int> checkString(string s, bool (*f)(const string&, const string&), int num){
    pair<pair<string, int>, int> ans;
    ans.first.first = "";
    ans.first.second = -1;
    int n = allRulesT2.size();
    for (int i = 0; i < 7; i++){
        int ruleNum = getRandomRuleT2(n);
        Rule rule = allRulesT2[ruleNum];
        pair<bool, int> p = checkIsRuleMayBeApplied(rule, s);
        if (p.first == true){
            string copy = s;
            executeRule(rule, s, p.second);
            if (f(copy, s)){
                continue;
            }else{
                ans.second = num;
            }

            ans.first.first = copy;
            ans.first.second = ruleNum;
            return ans;
        }
    }
    return ans;
}


//будем генерировать 50000 тестов
//вернём true, если все тесты успешны,
//иначе false
//и возвращаем строку, на которой была ошибка
//и номер правила, на котором инвариант не сохранился
//и номер инварианта
pair<bool, pair<string, pair<int, int>>> meta(bool (*f)(const string&, const string&), int num){
    pair<bool, pair<string, pair<int, int>>> p1;
    p1.first = true;
    p1.second.first = "";
    p1.second.second.first = -1;
    p1.second.second.second = -1;
    for (int i = 0; i < 50000; i++){
        string s = getRandomString(15, alphabet);
        pair<pair<string, int>, int> p2 = checkString(s, f, num);
        if (p2.first.second != -1){
            p1.first = false;
            p1.second.first = p2.first.first;
            p1.second.second.first = p2.first.second; 
            p1.second.second.second = p2.second;
            return p1;
        }
        
    }
    return p1;
}

int main(){
    initRulesT2();
    //вектор инвариантов
    vector<bool (*)(const string&, const string&)> invariants;
    invariants.push_back(&f0);
    invariants.push_back(&f1);
    
    //номер инварианта
    int num = 0;
    for (auto invariant: invariants){
        pair<bool, pair<string, pair<int, int>>> p = meta(invariant, num);
        cout <<"инвариант номер: " << num << endl;
        if (p.first){
            cout << " для этого инварианта все тесты успешны" << endl;
        }else{
            cout << " ошибка на строке: " << p.second.first << endl;
            cout << " проблемное правило: ";
            cout << allRulesT2[p.second.second.first].l << " -> " << allRulesT2[p.second.second.first].r << endl;
        }
        cout << endl;
        num += 1;
    }

}