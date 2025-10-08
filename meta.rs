use std::time::{SystemTime, UNIX_EPOCH};

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

// ИНВАРИНТЫ:

//1-ое свойство: если буква b есть в строке, то при применении любого правила
//в строке всё равно будет хотя бы 1 буква b
fn f0(prev: &str, curr: &str) -> bool {
    let found1 = prev.find('b');
    if found1.is_some() {
        let found2 = curr.find('b');
        if found2.is_some() {
            return true;
        }
        return false;
    }
    true
}

//2-ое свойство: число букв b не увеличивается
fn f1(prev: &str, curr: &str) -> bool {
    let count_prev = prev.chars().filter(|&c| c == 'b').count();
    let count_curr = curr.chars().filter(|&c| c == 'b').count();
    count_prev >= count_curr
}

//3-ье свойство: Пусть матрица A = 1 0 и матрица B = 2 -1
//                                 0 1               2 -1

// Если заменить в строке все буквы a на матрицу A , а буквы b на матрицу B,
//то можно посчитать произведение. В исходной системе это произведение не будет меняться
// при любом правиле
 
type Matrix = [[i32; 2]; 2];

const A: Matrix = [[1, 0], [0, 1]];
const B: Matrix = [[2, -1], [2, -1]];

//перемножаем 2 матрицы
fn multiply_matrices(x: &Matrix, y: &Matrix) -> Matrix {
    let mut result = [[0; 2]; 2];
    for row in 0..2 {
        for column in 0..2 {
            for k in 0..2 {
                result[row][column] += x[row][k] * y[k][column];
            }
        }
    }
    result
}

//перемножаем все матрицы
fn multiply_all_matrices(s: &str) -> Matrix {
    if s.is_empty() {
        return [[1, 0], [0, 1]]; // единичная матрица
    }
    
    let mut chars = s.chars();
    let first_char = chars.next().unwrap();
    let mut result = if first_char == 'a' { A } else { B };
    
    for c in chars {
        let matrix = if c == 'a' { A } else { B };
        result = multiply_matrices(&result, &matrix);
    }
    
    result
}

fn print_matrix(a: &Matrix) {
    for row in 0..2 {
        for column in 0..2 {
            print!("{} ", a[row][column]);
        }
        println!();
    }
}

fn f2(prev: &str, curr: &str) -> bool {
    let a = multiply_all_matrices(prev);
    let b = multiply_all_matrices(curr);
    
    for row in 0..2 {
        for column in 0..2 {
            if a[row][column] != b[row][column] {
                return false;
            }
        }
    }
    true
}

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

static mut ALL_RULES_T2: Vec<Rule> = Vec::new();
const ALPHABET: &str = "ab";

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

fn get_random_rule_t2() -> usize {
    let mut rng = SimpleRng::new();
    unsafe {
        rng.gen_range(0..ALL_RULES_T2.len())
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

// генерируем случайные 7 правил и проверим, что при
//выполнении правил сохраняются инварианты
//если нет, то возвращаем прооблемную строку
//и возвращаем номер 1-ого проблемного правила
//( -1 , если всё верно)
//а также вернём номер свойства, которое не выполнилось
fn check_string(s: &str, f: fn(&str, &str) -> bool, num: i32) -> ((String, i32), i32) {
    let mut current = s.to_string();
    let mut ans = ((String::new(), -1), -1);
    
    unsafe {
        for _ in 0..7 {
            let rule_num = get_random_rule_t2();
            let rule = &ALL_RULES_T2[rule_num];
            let (can_apply, pos) = check_is_rule_may_be_applied(rule, &current);
            
            if can_apply {
                let copy = current.clone();
                execute_rule(rule, &mut current, pos);
                
                if !f(&copy, &current) {
                    ans.0 .0 = copy;
                    ans.0 .1 = rule_num as i32;
                    ans.1 = num;
                    return ans;
                }
            }
        }
    }
    
    ans
}

//будем генерировать 50000 тестов
//вернём true, если все тесты успешны,
//иначе false
//и возвращаем строку, на которой была ошибка
//и номер правила, на котором инвариант не сохранился
//и номер инварианта
fn meta(f: fn(&str, &str) -> bool, num: i32) -> (bool, (String, (i32, i32))) {
    for _ in 0..50000 {
        let s = get_random_string(15);
        let result = check_string(&s, f, num);
        
        if result.0 .1 != -1 {
            return (false, (result.0 .0, (result.0 .1, result.1)));
        }
    }
    
    (true, (String::new(), (-1, -1)))
}

fn main() {
    init_rules_t2();
    
    //вектор инвариантов
    let invariants: Vec<fn(&str, &str) -> bool> = vec![f0, f1, f2];
    
    //номер инварианта
    let mut num = 0;
    for invariant in invariants {
        let (success, (error_string, (rule_num, invariant_num))) = meta(invariant, num);
        println!("инвариант номер: {}", num);
        if success {
            println!(" для этого инварианта все тесты успешны");
        } else {
            println!(" ошибка на строке: {}", error_string);
            unsafe {
                if rule_num >= 0 {
                    let rule = &ALL_RULES_T2[rule_num as usize];
                    println!(" проблемное правило: {} -> {}", rule.l, rule.r);
                }
            }
        }
        println!();
        num += 1;
    }
}