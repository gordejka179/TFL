use std::time::{SystemTime, UNIX_EPOCH};

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

// Простой самодельный RNG
struct SimpleRng {
    state: u64,
}

impl SimpleRng {
    fn new() -> Self {
        let seed = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_nanos() as u64;
        Self { state: seed }
    }
    
    fn gen_range(&mut self, range: std::ops::Range<usize>) -> usize {
        self.state = self.state.wrapping_mul(6364136223846793005).wrapping_add(1);
        let start = range.start;
        let end = range.end;
        start + (self.state as usize) % (end - start)
    }
}

static mut ALL_RULES_T2: Vec<Rule> = Vec::new();
static mut ALL_RULES_T: Vec<Rule> = Vec::new();
const ALPHABET: &str = "ab";

fn init_rules_t() {
    unsafe {
        ALL_RULES_T.push(Rule::new("aaaa", "a"));
        ALL_RULES_T.push(Rule::new("aaab", "b"));
        ALL_RULES_T.push(Rule::new("bbba", "ba"));
        ALL_RULES_T.push(Rule::new("bbbb", "bb"));
        ALL_RULES_T.push(Rule::new("ababb", "babb"));
        ALL_RULES_T.push(Rule::new("baaba", "bba"));
        ALL_RULES_T.push(Rule::new("baabb", "bbb"));
        ALL_RULES_T.push(Rule::new("bbaaa", "baab"));
        ALL_RULES_T.push(Rule::new("bbaab", "baa"));
        ALL_RULES_T.push(Rule::new("bbabb", "abab"));
        ALL_RULES_T.push(Rule::new("baba", "baa"));
        ALL_RULES_T.push(Rule::new("babbaa", "babba"));
        ALL_RULES_T.push(Rule::new("babbab", "abb"));
    }
}

fn init_rules_t2() {
    unsafe {
        ALL_RULES_T2.push(Rule::new("bb", "ba"));
        ALL_RULES_T2.push(Rule::new("aba", "ba"));
        ALL_RULES_T2.push(Rule::new("baa", "ba"));
        ALL_RULES_T2.push(Rule::new("bab", "ba"));
        ALL_RULES_T2.push(Rule::new("aaaa", "a"));
        ALL_RULES_T2.push(Rule::new("aaab", "b"));
    }
}

fn execute_rule(rule: &Rule, s: &mut String, l: usize) {
    let len = rule.l.len();
    s.replace_range(l..l + len, &rule.r);
}

fn check_is_rule_may_be_applied(rule: &Rule, s: &str) -> (bool, usize) {
    if let Some(found) = s.find(&rule.l) {
        (true, found)
    } else {
        (false, 0)
    }
}

fn normalize(s: &mut String) {
    unsafe {
        for rule in &ALL_RULES_T2 {
            let (can_apply, pos) = check_is_rule_may_be_applied(rule, s);
            if can_apply {
                execute_rule(rule, s, pos);
                normalize(s);
            }
        }
    }
}

fn get_random_rule_t() -> usize {
    let mut rng = SimpleRng::new();
    unsafe {
        rng.gen_range(0..ALL_RULES_T.len())
    }
}

fn get_random_string(n: usize) -> String {
    let mut rng = SimpleRng::new();
    let alphabet: Vec<char> = ALPHABET.chars().collect();
    (0..n)
        .map(|_| {
            let idx = rng.gen_range(0..alphabet.len());
            alphabet[idx]
        })
        .collect()
}

// генерируем случайные 7 правил и посмотрим, в какую строку перейдём
//при такой цепочке правил из произвольно выбранной строки в системе T
fn t(s: &str) -> (String, Vec<usize>) {
    let mut current = s.to_string();
    let mut path = Vec::new();

    for _ in 0..7 {
        let rule_num = get_random_rule_t();
        unsafe {
            let rule = &ALL_RULES_T[rule_num];
            let (can_apply, pos) = check_is_rule_may_be_applied(rule, &current);
            path.push(rule_num);
            
            if can_apply {
                execute_rule(rule, &mut current, pos);
            }
        }
    }

    (current, path)
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
fn fuzzing() -> (bool, (String, Vec<usize>)) {
    for _ in 0..50000 {
        let s = get_random_string(15);
        let (s2, path) = t(&s);
        
        let mut normalized_s = s.clone();
        let mut normalized_s2 = s2.clone();
        
        normalize(&mut normalized_s);
        normalize(&mut normalized_s2);

        if normalized_s != normalized_s2 {
            return (false, (normalized_s, path));
        }
    }

    (true, ("".to_string(), Vec::new()))
}

fn main() {
    init_rules_t();
    init_rules_t2();

    let (success, (error_string, path)) = fuzzing();

    if success {
        println!("все тесты успешны");
    } else {
        println!("найдена ошибка: {} и правила:", error_string);
        for rule_num in path {
            println!("{}", rule_num);
        }
    }
}