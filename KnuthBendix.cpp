#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// все критические пары
vector<pair<string,string>> AllPairs;

struct Rule{
    string l;
    string r; 
    Rule(const string& left, const string& right) : l(left), r(right) {}
};

// все правила
vector<Rule> Rules;

void initRules(){
    Rules.push_back(Rule("aaaa", "a"));
    Rules.push_back(Rule("aaab", "b"));
    Rules.push_back(Rule("bbba", "ba"));
    Rules.push_back(Rule("bbbb", "bb"));
    Rules.push_back(Rule("ababb", "babb"));
    Rules.push_back(Rule("baaba", "bba"));
    Rules.push_back(Rule("baabb", "bbb"));
    Rules.push_back(Rule("bbaaa", "baab"));
    Rules.push_back(Rule("bbaab", "baa"));
    Rules.push_back(Rule("bbabb", "abab"));
    Rules.push_back(Rule("baba", "baa"));
    Rules.push_back(Rule("babbaa", "babba"));
    Rules.push_back(Rule("babbab", "abb"));
}

// l - левая граница подстроки в s (индекс), которую нужно заменить по правилу
void executeRule(Rule rule, string& s, int l){
    //string& replace (size_t pos, size_t len, const string& str);
    int len = rule.l.length();
    s.replace(l, len, rule.r);
}

//получаем, все критические пары, которые порождаются 2-мя правилами
vector<pair<string,string>> getCriticalPairsForPairRules(Rule rule1, Rule rule2){
    vector<pair<string,string>> v;
    string l1 = rule1.l;
    string l2 = rule2.l;
   
    //какой-то префикс l1 совпадает с суффиксом l2:
    //i - длина префикса l1
    for (int i = 1; i < min(l1.length(), l2.length()); i++){
        if (l2.substr(l2.length() - i, i) == l1.substr(0, i)){
            pair<string,string> p;
            string commonS = l2 + l1.substr(i, l1.length() - i);
            //применяем 1 правило, меняем l1
            executeRule(rule1, commonS, l2.length() - i);
            p.first = commonS;


            //применяем 2 правило, меняем l2
            commonS = l2 + l1.substr(i, l1.length() - i);
            executeRule(rule2, commonS, 0);
            p.second = commonS;

            if (p.first != p.second){
                v.push_back(p);
            }
        }

    }

    //какой-то префикс l1 совпадает с суффиксом l2:
    //i - длина префикса l2
    for (int i = 1; i < min(l1.length(), l2.length()); i++){
        if (l1.substr(l1.length() - i, i) == l2.substr(0, i)){
            pair<string,string> p;
            string commonS = l1 + l2.substr(i, l2.length() - i);
            string copy = commonS;
            //применяем 1 правило, меняем l2
            executeRule(rule1, commonS, 0);
            p.first = commonS;

            //применяем 2 правило, меняем l1
            commonS = l1 + l2.substr(i, l2.length() - i);
            executeRule(rule2, commonS, l1.length() - i);
            p.second = commonS;

            if (p.first != p.second){
                v.push_back(p);
            }
        }

    }
    
    /*
    for (pair<string, string> p : v){
        cout << p.first << " " << p.second << endl;
    }
    cout << "\n"; 
    */
    return v;

}

//получаем критические пары для 1 правила при участии со всеми остальными
void getCriticalPairsForRule(vector<pair<string,string>>& p, Rule rule){
    int size = Rules.size();
    for (int i = 0; i < size; i++){
        vector<pair<string,string>> pairs = getCriticalPairsForPairRules(rule, Rules[i]);

        //Добавляем к одному вектору все элементы второго
        p.insert(p.end(), pairs.begin(), pairs.end());
    }
}

void getAllCriticalPairs(){
    int size = Rules.size();
    for (int i = 0; i < size; i++){
        for (int j = i; j < size; j++){
            vector<pair<string,string>> pairs = getCriticalPairsForPairRules(Rules[i], Rules[j]);

            //Добавляем к одному вектору все элементы второго
            AllPairs.insert(AllPairs.end(), pairs.begin(), pairs.end());
        }
    }
}

//проверим, можно ли применить правило из списка Rules
pair<bool, int> checkMayRuleBeApplied(Rule rule, const string& s){
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

//сравнение строк на выбранном фундированном порядке 
//порядок такой: в начале смотрим на длину, а в случае равенства - сравниваем лексикографически
//если первая строка больше, то возвращаем true, иначе false
bool compare(const string& s1, const string& s2){
    if (s1.length() > s2.length()){
        return true;
    }
    if (s1.length() < s2.length()){
        return false;
    }
    if (s1 > s2){
        return true;
    }
    return false;
}

//приводим строку к нормальной форме
void normalize(string& s){
    for (Rule rule: Rules){
        pair<bool, int> p = checkMayRuleBeApplied(rule, s);
        if (p.first){
            executeRule(rule, s, p.second);
            normalize(s);
        }else{
            continue;
        }
    }
}

//приводим строку к нормальной форме
//но при этом будем также передавать строку-ограничение, то есть
//чтобы если при выполнении какого-то правила строка стала <=, чем строка-ограничение,
//то делаем откат
//count - счетчик, это нужно поскольку, если на 1-ом вызове окажется, что есть
//правило s -> restriction, то будут проблемы
void normalizeWithRestriction(string& s, const string& restriction, int count){
    for (Rule rule: Rules){
        pair<bool, int> p = checkMayRuleBeApplied(rule, s);
        string copy = s;
        if (p.first && !(rule.l == "" && rule.r == "")){
            if (count == 0 && rule.l == s && rule.r == restriction){
                count += 1;
                continue;
            }
            executeRule(rule, s, p.second);
            if (compare(restriction, s)){
                s = copy;
                continue;
            }
            normalizeWithRestriction(s, restriction, count);
        }else{
            continue;
        }
    }
}

//собстна алгоритм Кнута-Бендикса
void KnuthBendix(){
    getAllCriticalPairs();

    
    while (AllPairs.size() > 0){
        pair<string,string> p = AllPairs[0];
        //cout << p.first << " " << p.second << endl; 

        // Удаляем первую пару из вектора
        AllPairs.erase(AllPairs.begin());
        normalize(p.first);
        normalize(p.second);
        //cout << p.first << " " << p.second << endl; 
        if (p.first != p.second){
            pair<string, string> pMaxMin;
            if (compare(p.first, p.second)){
                pMaxMin.first = p.first;
                pMaxMin.second = p.second;
            }else{
                pMaxMin.first = p.second;
                pMaxMin.second = p.first;
            }
            Rule r(pMaxMin.first, pMaxMin.second);
            Rules.push_back(r);

            vector<pair<string, string>> p;
            getCriticalPairsForRule(p, r);

            //Добавляем к одному вектору все элементы второго
            AllPairs.insert(AllPairs.end(), p.begin(), p.end());
        }
    }
}

//проверка, есть ли среди правил правило c указанной левой
//и правой частью. 
bool checkRule(const string& left, const string& right){
    for (Rule rule: Rules){

        if ((rule.l != left) || (rule.r != right)){
            continue;
        }else{
            return true;
        }
    }
    return false;

}

//будем избавляться от некоторых правил
void reduce(){
    for (Rule& rule: Rules){
        string left = rule.l;
        string right = rule.r;
        //cout << rule.l << " " << rule.r << " " << left << " " << right << endl;
        normalizeWithRestriction(left, right, 0);
        //cout << rule.l << " " << rule.r << " " << left << " " << right << endl;
        if (!checkRule(left, right)){
            if (left == right){
                //cout << 1 << rule.l << " " << rule.r << " " << left << " " << right << endl;
                //просто избавимся от старого правила
                rule.l = "";
                rule.r = ""; 
            }else{
                //cout << 2 << rule.l << " " << rule.r << " " << left << " " << right << endl;
                //проводим новое правило, вместо старого
                rule.l = left;
                rule.r = right;
            }
        }else{
            if (rule.l != left || rule.r != right){
                //просто избавимся от старого правила
                //cout << 3 << rule.l << " " << rule.r << " " << left << " " << right << endl;
                rule.l = "";
                rule.r = "";
            }
        }
        
    }
}

int main(){
    initRules();
    KnuthBendix();
    reduce();
    /*
    string l = "bbbb";
    string r = "bb";
    normalizeWithRestriction(l, r, 0);
    cout << l << endl;
    */
    for (Rule rule: Rules){
        if (!(rule.l == "" && rule.r == "")){
            cout << rule.l << " " << rule.r << endl;
        } 
    }
}


/*
без reduce()
должно получиться

aaaa a
aaab b
bbba ba
bbbb bb
ababb babb
baaba bba
baabb bbb
bbaaa baab
bbaab baa
bbabb abab
baba baa
babbaa babba
babbab abb
babba bb
babb bab
bab abb
baaa abb
abaa baa
baa abb
abbb abb
abb bb
bba bb
bb ba
aba ba

с reduce() должно получиться
aaaa a
aaab b
aaaba babb
baba baab
bab baa
babbb babba
aaba baa
baa abb
aba bb
bb ba
*/

