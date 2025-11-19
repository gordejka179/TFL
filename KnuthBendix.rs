use std::cmp::min;

// все критические пары
static mut ALL_PAIRS: Vec<(String, String)> = Vec::new();

struct Rule {
    l: String,
    r: String,
}

impl Rule {
    fn new(left: &str, right: &str) -> Self {
        Self {
            l: left.to_string(),
            r: right.to_string(),
        }
    }
}

// все правила
static mut RULES: Vec<Rule> = Vec::new();

fn init_rules() {
    unsafe {
        RULES.push(Rule::new("aaaa", "a"));
        RULES.push(Rule::new("aaab", "b"));
        RULES.push(Rule::new("bbba", "ba"));
        RULES.push(Rule::new("bbbb", "bb"));
        RULES.push(Rule::new("ababb", "babb"));
        RULES.push(Rule::new("baaba", "bba"));
        RULES.push(Rule::new("baabb", "bbb"));
        RULES.push(Rule::new("bbaaa", "baab"));
        RULES.push(Rule::new("bbaab", "baa"));
        RULES.push(Rule::new("bbabb", "abab"));
        RULES.push(Rule::new("baba", "baa"));
        RULES.push(Rule::new("babbaa", "babba"));
        RULES.push(Rule::new("babbab", "abb"));
    }
}

// l - левая граница подстроки в s (индекс), которую нужно заменить по правилу
fn execute_rule(rule: &Rule, s: &mut String, l: usize) {
    let len = rule.l.len();
    s.replace_range(l..l + len, &rule.r);
}

//получаем, все критические пары, которые порождаются 2-мя правилами
fn get_critical_pairs_for_pair_rules(rule1: &Rule, rule2: &Rule) -> Vec<(String, String)> {
    let mut v = Vec::new();
    let l1 = &rule1.l;
    let l2 = &rule2.l;
   
    //какой-то префикс l1 совпадает с суффиксом l2:
    //i - длина префикса l1
    for i in 1..min(l1.len(), l2.len()) {
        if l2[l2.len() - i..] == l1[0..i] {
            let common_s = l2.to_string() + &l1[i..];
            let mut s1 = common_s.clone();
            //применяем 1 правило, меняем l1
            execute_rule(rule1, &mut s1, l2.len() - i);
            
            let mut s2 = common_s;
            //применяем 2 правило, меняем l2
            execute_rule(rule2, &mut s2, 0);
            
            if s1 != s2 {
                v.push((s1, s2));
            }
        }
    }

    //какой-то префикс l1 совпадает с суффиксом l2:
    //i - длина префикса l2
    for i in 1..min(l1.len(), l2.len()) {
        if l1[l1.len() - i..] == l2[0..i] {
            let common_s = l1.to_string() + &l2[i..];
            let mut s1 = common_s.clone();
            //применяем 1 правило, меняем l2
            execute_rule(rule1, &mut s1, 0);
            
            let mut s2 = common_s;
            //применяем 2 правило, меняем l1
            execute_rule(rule2, &mut s2, l1.len() - i);
            
            if s1 != s2 {
                v.push((s1, s2));
            }
        }
    }
    
    v
}

//получаем критические пары для 1 правила при участии со всеми остальными
fn get_critical_pairs_for_rule(rule: &Rule) -> Vec<(String, String)> {
    let mut p = Vec::new();
    unsafe {
        for other_rule in &RULES {
            let pairs = get_critical_pairs_for_pair_rules(rule, other_rule);
            p.extend(pairs);
        }
    }
    p
}

fn get_all_critical_pairs() {
    unsafe {
        for i in 0..RULES.len() {
            for j in i..RULES.len() {
                let pairs = get_critical_pairs_for_pair_rules(&RULES[i], &RULES[j]);
                ALL_PAIRS.extend(pairs);
            }
        }
    }
}

//проверим, можно ли применить правило из списка Rules
fn check_may_rule_be_applied(rule: &Rule, s: &str) -> (bool, usize) {
    if let Some(found) = s.find(&rule.l) {
        (true, found)
    } else {
        (false, 0)
    }
}

//сравнение строк на выбранном фундированном порядке 
//порядок такой: в начале смотрим на длину, а в случае равенства - сравниваем лексикографически
//если первая строка больше, то возвращаем true, иначе false
fn compare(s1: &str, s2: &str) -> bool {
    if s1.len() > s2.len() {
        return true;
    }
    if s1.len() < s2.len() {
        return false;
    }
    s1 > s2
}

//приводим строку к нормальной форме
fn normalize(s: &mut String) {
    unsafe {
        for rule in &RULES {
            let (can_apply, pos) = check_may_rule_be_applied(rule, s);
            if can_apply {
                execute_rule(rule, s, pos);
                normalize(s);
            }
        }
    }
}

//приводим строку к нормальной форме
//но при этом будем также передавать строку-ограничение, то есть
//чтобы если при выполнении какого-то правила строка стала <=, чем строка-ограничение,
//то делаем откат
//count - счетчик, это нужно поскольку, если на 1-ом вызове окажется, что есть
//правило s -> restriction, то будут проблемы
fn normalize_with_restriction(s: &mut String, restriction: &str, count: i32) {
    unsafe {
        for rule in &RULES {
            let (can_apply, pos) = check_may_rule_be_applied(rule, s);
            let copy = s.clone();
            if can_apply && !(rule.l.is_empty() && rule.r.is_empty()) {
                if count == 0 && rule.l == **s && rule.r == restriction {
                    continue;
                }
                execute_rule(rule, s, pos);
                if compare(restriction, s) {
                    *s = copy;
                    continue;
                }
                normalize_with_restriction(s, restriction, count + 1);
            }
        }
    }
}

//собстна алгоритм Кнута-Бендикса
fn knuth_bendix() {
    get_all_critical_pairs();
    
    unsafe {
        while !ALL_PAIRS.is_empty() {
            let mut p = ALL_PAIRS.remove(0);
            normalize(&mut p.0);
            normalize(&mut p.1);
            
            if p.0 != p.1 {
                let p_max_min = if compare(&p.0, &p.1) {
                    (p.0, p.1)
                } else {
                    (p.1, p.0)
                };
                
                let r = Rule::new(&p_max_min.0, &p_max_min.1);
                let new_pairs = get_critical_pairs_for_rule(&r);
                RULES.push(r);
                ALL_PAIRS.extend(new_pairs);
            }
        }
    }
}

//проверка, есть ли среди правил правило c указанной левой
//и правой частью. 
fn check_rule(left: &str, right: &str) -> bool {
    unsafe {
        for rule in &RULES {
            if rule.l == left && rule.r == right {
                return true;
            }
        }
    }
    false
}

//будем избавляться от некоторых правил
fn reduce() {
    unsafe {
        for rule in &mut RULES {
            let mut left = rule.l.clone();
            let right = rule.r.clone();
            
            normalize_with_restriction(&mut left, &right, 0);
            
            if !check_rule(&left, &right) {
                if left == right {
                    rule.l.clear();
                    rule.r.clear();
                } else {
                    rule.l = left;
                    rule.r = right;
                }
            } else if rule.l != left || rule.r != right {
                rule.l.clear();
                rule.r.clear();
            }
        }
    }
}

fn main() {
    init_rules();
    knuth_bendix();
    reduce();

    unsafe {
        for rule in &RULES {
            if !rule.l.is_empty() && !rule.r.is_empty() {
                println!("{} {}", rule.l, rule.r);
            }
        }
    }
}