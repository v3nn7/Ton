# Dokumentacja Języka Ton

## Spis Treści
1.  Wprowadzenie
2.  Typy Danych i Literały
3.  Deklaracja Zmiennych
4.  Operatory
5.  Instrukcje Sterujące
6.  Funkcje
7.  Funkcje Wbudowane
8.  Przykłady Kodu
9.  Kompilacja i Uruchamianie Programów

## 1. Wprowadzenie
Ton to prosty język programowania zaprojektowany do celów edukacyjnych i demonstracyjnych. Posiada podstawowe konstrukcje programistyczne, takie jak zmienne, typy danych, operatory, instrukcje sterujące oraz funkcje. Ten dokument opisuje aktualny stan implementacji (MVP). Elementy planowane: literały 0x/0b, operator trójargumentowy `?:`, `typeof()`, sekwencje ucieczki w stringach (np. \n, \t, \", \\), stringi wieloliniowe `"""..."""`, pełna składnia `struct`.

## 2. Typy Danych i Literały

Język Ton obsługuje następujące podstawowe typy danych:

*   **int**: Liczby całkowite, np. `10`, `-5`, `0`.
*   **float**: Liczby zmiennoprzecinkowe, np. `3.14`, `-0.5`, `1.0`.
*   **string**: Ciągi znaków, ujęte w podwójne cudzysłowy, np. `"Witaj świecie!"`, `"Ton"`.
*   **bool**: Wartości logiczne, `true` lub `false`.
*   **char**: Pojedyncze znaki, ujęte w pojedyncze cudzysłowy, np. `'a'`, `'Z'`.
*   **pointer**: Wskaźniki do pamięci, przechowujące adresy innych zmiennych lub bloków pamięci.

### Wskaźniki

Wskaźniki w języku Ton pozwalają na programowanie niskopoziomowe i bezpośrednie zarządzanie pamięcią. Wskaźnik przechowuje adres pamięci, gdzie znajduje się dana wartość.

#### Operatory wskaźnikowe:
*   `&`: Operator adresowania - zwraca adres zmiennej
*   `*`: Operator dereferencji - zwraca wartość znajdującą się pod adresem wskaźnika

#### Przykłady użycia wskaźników:

```ton
let liczba: int = 42;
let wskaznik: pointer = &liczba;  // wskaznik przechowuje adres zmiennej liczba
let wartosc: int = *wskaznik;     // wartosc = 42 (dereferencja wskaźnika)
```


## 3. Deklaracja Zmiennych

W języku Ton zmienne deklaruje się za pomocą słów kluczowych `let` (dla zmiennych mutowalnych) lub `const` (dla stałych, niemutowalnych zmiennych). Deklaracja zmiennej wymaga podania jej nazwy, typu oraz opcjonalnie wartości początkowej.

### Składnia

```ton
let <nazwa_zmiennej>: <typ> = <wartość_początkowa>;
const <nazwa_stałej>: <typ> = <wartość_początkowa>;
```

*   `<nazwa_zmiennej>` / `<nazwa_stałej>`: Unikalna nazwa identyfikująca zmienną lub stałą.
*   `<typ>`: Typ danych zmiennej (np. `int`, `float`, `string`, `bool`, `char`).
*   `<wartość_początkowa>`: Opcjonalna wartość przypisywana zmiennej podczas deklaracji. Dla `const` jest ona obowiązkowa.

### Przykłady

```ton
let wiek: int = 30;
let imie: string = "Anna";
const PI: float = 3.14159;
let czyAktywny: bool = true;

// Deklaracja bez inicjalizacji (tylko dla let)
let wynik: int;
wynik = 100;

// Błąd: próba zmiany wartości stałej
// PI = 3.0;
```


## 4. Operatory

Język Ton obsługuje operatory arytmetyczne, porównania, logiczne, przypisania (również złożone), inkrementacji/dekrementacji, operatory dostępu oraz wskaźnikowe.

### Operatory Arytmetyczne
Służą do wykonywania podstawowych operacji matematycznych.

*   `+`: Dodawanie (np. `a + b`)
*   `-`: Odejmowanie (np. `a - b`)
*   `*`: Mnożenie (np. `a * b`)
*   `/`: Dzielenie (np. `a / b`)
*   `%`: Reszta z dzielenia (modulo) (np. `a % b`)

### Operatory Porównania
Służą do porównywania wartości i zwracają wartość logiczną (`true` lub `false`).

*   `==`: Równe (np. `a == b`)
*   `!=`: Nierówne (np. `a != b`)
*   `<`: Mniejsze niż (np. `a < b`)
*   `>`: Większe niż (np. `a > b`)
*   `<=`: Mniejsze lub równe (np. `a <= b`)
*   `>=`: Większe lub równe (np. `a >= b`)

### Operatory Logiczne
Służą do łączenia lub negowania wyrażeń logicznych.

*   `&&`: Logiczne AND (np. `a && b`)
*   `||`: Logiczne OR (np. `a || b`)
*   `!`: Logiczne NOT (negacja) (np. `!a`)

### Operatory Przypisania
Służą do przypisywania wartości do zmiennych.

*   `=`: Proste przypisanie (np. `zmienna = wartość;`)
*   `+=`, `-=`, `*=`, `/=`, `%=`: Przypisania złożone, np. `x += 2` (odpowiednik `x = x + 2`)

### Inkrementacja i dekrementacja

*   `++x`, `x++`: Inkrementacja (pre/post)
*   `--x`, `x--`: Dekrementacja (pre/post)

### Operatory dostępu

*   `.`: Dostęp do pola/elementu (np. `obiekt.pole`)
*   `[]`: Indeksowanie (np. `tab[0]`)
*   `->`: Skrót dla dostępu przez wskaźnik (np. `ptr->pole`)

### Operatory Wskaźnikowe
Służą do pracy z wskaźnikami i zarządzania pamięcią.

*   `&`: Operator adresowania - zwraca adres zmiennej (np. `&zmienna`)
*   `*`: Operator dereferencji - zwraca wartość znajdującą się pod adresem wskaźnika (np. `*wskaznik`)

#### Przykład użycia operatorów wskaźnikowych:

```ton
let x: int = 10;
let ptr: pointer = &x;    // ptr przechowuje adres zmiennej x
let val: int = *ptr;      // val = 10 (wartość pod adresem ptr)
*ptr = 20;                // zmienia wartość x na 20 przez wskaźnik
```


## 5. Instrukcje Sterujące

Język Ton oferuje podstawowe instrukcje sterujące, które pozwalają na warunkowe wykonywanie kodu oraz tworzenie pętli.

### Instrukcja Warunkowa `if-else`
Instrukcja `if-else` pozwala na wykonanie bloku kodu, jeśli dany warunek jest prawdziwy, oraz opcjonalnie innego bloku, jeśli warunek jest fałszywy.

#### Składnia

```ton
if (<warunek>) {
    // kod do wykonania, jeśli warunek jest prawdziwy
} else {
    // kod do wykonania, jeśli warunek jest fałszywy (opcjonalnie)
}
```

#### Przykłady

```ton
let x: int = 10;

if (x > 5) {
    print("x jest większe od 5");
} else {
    print("x jest mniejsze lub równe 5");
}

let y: int = 3;
if (y % 2 == 0) {
    print("y jest parzyste");
} else {
    print("y jest nieparzyste");
}
```

### Pętla `while`
Pętla `while` wykonuje blok kodu tak długo, jak długo określony warunek jest prawdziwy.

#### Składnia

```ton
while (<warunek>) {
    // kod do wykonania w pętli
}
```

#### Przykłady

```ton
let i: int = 0;
while (i < 5) {
    print("Iteracja: " + i);
    i = i + 1;
}
```

### Pętle: `while`, `for`, `loop`

- `while (warunek) { ... }` — klasyczna pętla warunkowa.
- `for (init; warunek; krok) { ... }` — pętla w stylu C (obsługiwana przez parser).
- `loop { ... }` — pętla nieskończona, z wyjściem przez `break`.

#### Składnia

```ton
while (i < 10) { ... }

for (let i: int = 0; i < 10; i = i + 1) {
    // ciało pętli
}

loop {
    // ciało pętli
    if (<warunek_wyjścia>) break;
}
```

#### break i continue
- `break;` — natychmiast wychodzi z najbliższej pętli.
- `continue;` — przerywa aktualną iterację i przechodzi do kolejnej.

```ton
for (let i: int = 0; i < 10; i = i + 1) {
    if (i % 2 == 0) continue; // pomiń parzyste
    if (i > 7) break;          // wyjdź po 7
    print(i);
}
```


## 6. Funkcje

Funkcje w języku Ton pozwalają na grupowanie kodu w celu jego ponownego użycia i organizacji. Funkcje mogą przyjmować argumenty i zwracać wartości.

### Deklaracja Funkcji

#### Składnia

```ton
fn <nazwa_funkcji>(<parametr1>: <typ1>, <parametr2>: <typ2>, ...) -> <typ_zwracany> {
    // ciało funkcji
    return <wartość>; // opcjonalnie
}
```

*   `<nazwa_funkcji>`: Nazwa funkcji.
*   `<parametrN>`: Nazwa parametru.
*   `<typN>`: Typ danych parametru.
*   `<typ_zwracany>`: Typ danych wartości zwracanej przez funkcję. Jeśli funkcja nic nie zwraca, można użyć `void` lub pominąć `-> <typ_zwracany>`.

### Wywołanie Funkcji
Funkcje wywołuje się, podając ich nazwę, a następnie listę argumentów w nawiasach.

#### Składnia

```ton
<nazwa_funkcji>(<argument1>, <argument2>, ...);
```

#### Przykłady

```ton
fn dodaj(a: int, b: int) -> int {
    return a + b;
}

fn powitanie(imie: string) {
    print("Witaj, " + imie + "!");
}

let suma: int = dodaj(5, 3);
print("Suma: " + suma);

powitanie("Alicja");
```

### Instrukcja `return`
Instrukcja `return` służy do zwracania wartości z funkcji i natychmiastowego zakończenia jej wykonania.

#### Składnia

```ton
return <wartość>;
```

#### Przykłady

```ton
fn sprawdzDodatnia(liczba: int) -> bool {
    if (liczba > 0) {
        return true;
    } else {
        return false;
    }
}

let czyDodatnia: bool = sprawdzDodatnia(10);
print("Czy dodatnia: " + czyDodatnia);
```


## 7. Funkcje Wbudowane

Język Ton udostępnia kilka wbudowanych funkcji, które ułatwiają interakcję z użytkownikiem, wyświetlanie danych oraz zarządzanie pamięcią.

### `print(<wartość>)`
Funkcja `print` służy do wyświetlania wartości na standardowym wyjściu (konsoli). Może przyjmować argumenty różnych typów danych.

#### Składnia

```ton
print(<wartość>);
```

#### Przykłady

```ton
print("Witaj, Ton!");
let liczba: int = 123;
print(liczba);
let prawda: bool = true;
print(prawda);
```

### Funkcje Zarządzania Pamięcią

#### `alloc(<rozmiar>)`
Funkcja `alloc` służy do alokacji pamięci o określonym rozmiarze w bajtach. Zwraca wskaźnik do zaalokowanego bloku pamięci.

##### Składnia

```ton
alloc(<rozmiar>: int) -> pointer;
```

##### Przykład

```ton
let ptr: pointer = alloc(100);  // alokuje 100 bajtów pamięci
```

#### `free(<wskaźnik>)`
Funkcja `free` służy do zwalniania wcześniej zaalokowanej pamięci. Przyjmuje wskaźnik do bloku pamięci, który ma zostać zwolniony.

##### Składnia

```ton
free(<wskaźnik>: pointer);
```

##### Przykład

```ton
let ptr: pointer = alloc(100);
// ... użycie pamięci ...
free(ptr);  // zwalnia pamięć
```

##### Uwaga
Zawsze należy zwalniać pamięć zaalokowaną za pomocą `alloc()`, aby uniknąć wycieków pamięci. Nie należy używać wskaźnika po wywołaniu `free()`.

### `read_line()`
Funkcja `read_line` służy do odczytywania linii tekstu ze standardowego wejścia (konsoli) i zwraca ją jako wartość typu `string`.

#### Składnia

```ton
read_line();
```

#### Przykłady

```ton
print("Podaj swoje imię:");
let imie_uzytkownika: string = read_line();
print("Witaj, " + imie_uzytkownika + "!");

print("Podaj swój wiek:");
let wiek_str: string = read_line();
// Konwersja string na int (jeśli dostępna w języku Ton, w przeciwnym razie wymagałoby to dodatkowej funkcji)
// let wiek: int = to_int(wiek_str);
// print("Twój wiek to: " + wiek);
```

### Operacje bitowe (funkcje wbudowane)
Funkcje te działają na liczbach całkowitych i zwracają `int`. Walidują liczbę argumentów i wymagają argumentów typu całkowitego.

- bit_and(a, b) -> a & b
- bit_or(a, b)  -> a | b
- bit_xor(a, b) -> a ^ b
- bit_not(a)    -> ~a
- bit_shl(a, n) -> a << n
- bit_shr(a, n) -> a >> n

#### Przykłady

```ton
let a: int = 6;  // 110
let b: int = 3;  // 011
print(bit_and(a, b)); // 2
print(bit_or(a, b));  // 7
print(bit_xor(a, b)); // 5
print(bit_shl(a, 1)); // 12
print(bit_shr(a, 1)); // 3
print(bit_not(0));    // -1 (w konwencji dwójkowej)
print(bit_not(a));    // ~6 == -7 (w konwencji dwójkowej)
```


## 8. Przykłady Kodu

Poniżej przedstawiono kilka przykładów kodu w języku Ton, ilustrujących różne funkcje i konstrukcje języka.

### Przykład 1: Prosty program "Witaj świecie!"

```ton
print("Witaj świecie!");
```

### Przykład 2: Deklaracja zmiennych i operacje arytmetyczne

Uwaga: aktualnie operatory arytmetyczne (+, -, *, /) oraz unarny minus (-a) działają wyłącznie na typie `int`. Dzielenie przez zero jest błędem wykonania.

```ton
let a: int = 10;
let b: int = 20;
let suma: int = a + b;
print("Suma a i b: " + suma);

// Przykład unarnego minusa
let c: int = -a; // c == -10

// Operacje dzielenia (zabezpieczenie przed dzieleniem przez zero)
let d: int = a / 2; // OK
// let e: int = a / 0; // spowoduje Runtime Error: Division by zero

// (Przykłady z float poniżej są koncepcyjne; obsługa float może nie być jeszcze dostępna)
const PI: float = 3.14;
let promien: float = 5.0;
let obwod: float = 2 * PI * promien;
print("Obwód koła: " + obwod);
```

### Przykład 3: Instrukcja warunkowa `if-else`

```ton
let wiek: int = 18;

if (wiek >= 18) {
    print("Jesteś pełnoletni.");
} else {
    print("Jesteś niepełnoletni.");
}
```

### Przykład 4: Pętla `while`

```ton
let licznik: int = 0;
while (licznik < 5) {
    print("Licznik: " + licznik);
    licznik = licznik + 1;
}
```

### Przykład 5: Funkcje

```ton
fn pomnoz(x: int, y: int) -> int {
    return x * y;
}

let wynik_mnozenia: int = pomnoz(4, 6);
print("Wynik mnożenia: " + wynik_mnozenia);

fn pozdrow(imie: string) {
    print("Cześć, " + imie + "!");
}

pozdrow("Marcin");
```

### Przykład 6: Interakcja z użytkownikiem (`read_line`)

```ton
print("Podaj swoje ulubione zwierzę:");
let zwierze: string = read_line();
print("Twoje ulubione zwierzę to: " + zwierze);
```


## 9. Kompilacja i Uruchamianie Programów

Programy napisane w języku Ton są interpretowane. Oznacza to, że nie są kompilowane do kodu maszynowego przed wykonaniem, lecz są wykonywane linia po linii przez interpreter Ton.

### Uruchamianie Interpretera

Aby uruchomić program Ton, należy wywołać interpreter Ton, podając ścieżkę do pliku źródłowego jako argument.

#### Składnia

```bash
ton <nazwa_pliku>.ton
```

*   `<nazwa_pliku>.ton`: Ścieżka do pliku zawierającego kod źródłowy języka Ton.

#### Przykłady

Załóżmy, że masz plik `hello.ton` z następującą zawartością:

```ton
print("Witaj, Ton!");
```

Aby uruchomić ten program, wykonaj w terminalu:

```bash
ton hello.ton
```

Wynik:

```
Witaj, Ton!
```

### Punkt wejścia: main i tryb skryptowy

- Ton wykonuje wszystkie instrukcje na najwyższym poziomie (top-level) w pliku.
- Jeśli zdefiniowana jest funkcja `main`, interpreter wykona ją po instrukcjach top-level w świeżym, lokalnym środowisku, a zwracana wartość typu int stanie się kodem wyjścia procesu. Jeśli `main` zwróci wartość niebędącą int lub nie ma jawnego `return`, kod wyjścia domyślnie wynosi 0.
- Jeśli `main` nie jest zdefiniowana, interpreter działa w trybie skryptowym: wykonuje instrukcje top-level i kończy z kodem wyjścia 0.

#### Przykład z `main`

```ton
print("Najpierw wykonuje się top-level.");

fn main() -> int {
    print("Witaj z funkcji main!");
    return 0; // kod wyjścia procesu
}
```

### Obsługa Błędów

Interpreter Ton zgłasza błędy napotkane podczas parsowania lub wykonywania programu, wskazując numer linii i kolumny, w której wystąpił błąd, oraz krótki opis problemu.

#### Przykład Błędu Parsowania

Jeśli w pliku `blad.ton` masz:

```ton
let x: int = ;
```

Uruchomienie `ton blad.ton` może zwrócić błąd podobny do:

```
Parser Error at line 1, column 14: Expected an expression
```

#### Przykład Błędu Wykonania

Jeśli w pliku `dzielenie.ton` masz:

```ton
let a: int = 10;
let b: int = 0;
let wynik: int = a / b;
print(wynik);
```

Uruchomienie `ton dzielenie.ton` może zwrócić błąd podobny do:

```
Runtime Error at line 3, column 17: Division by zero
```

## Przykłady Zaawansowane

### Praca z Wskaźnikami i Zarządzaniem Pamięcią

```ton
fn main() -> int {
    // Alokacja pamięci
    let ptr: int* = alloc(sizeof(int));
    *ptr = 42;
    
    print("Wartość pod wskaźnikiem: ");
    print(*ptr);
    
    // Zwolnienie pamięci
    free(ptr);
    
    return 0;
}
```

### Operacje na Stringach z TonLib

```ton
fn main() -> int {
    let tekst: string = "Hello World";
    
    // Wyciągnięcie podstringa
    let pozdrowienie: string = substring(tekst, 0, 5); // "Hello"
    let swiat: string = substring(tekst, 6); // "World"
    
    // Wyszukiwanie w stringu
    let pozycja: int = index_of(tekst, "World"); // 6
    
    // Zamiana tekstu
    let nowy_tekst: string = replace(tekst, "World", "Ton"); // "Hello Ton"
    
    print(pozdrowienie);
    print(swiat);
    print(nowy_tekst);
    
    return 0;
}
```

### Wykorzystanie Struktur Danych

```ton
fn main() -> int {
    // Tworzenie listy
    let lista: TonList* = tonlist_create();
    tonlist_push(lista, create_value_int(10));
    tonlist_push(lista, create_value_int(20));
    tonlist_push(lista, create_value_int(30));
    
    print("Rozmiar listy: ");
    print(tonlist_size(lista));
    
    // Tworzenie mapy
    let mapa: TonMap* = tonmap_create();
    tonmap_set(mapa, "klucz1", create_value_string("wartość1"));
    tonmap_set(mapa, "klucz2", create_value_int(42));
    
    let wartosc: Value = tonmap_get(mapa, "klucz1");
    
    // Tworzenie zbioru
    let zbior: TonSet* = tonset_create();
    tonset_add(zbior, "element1");
    tonset_add(zbior, "element2");
    tonset_add(zbior, "element1"); // Duplikat - nie zostanie dodany
    
    print("Rozmiar zbioru: ");
    print(tonset_size(zbior)); // Wypisze 2
    
    // Zwolnienie pamięci
    tonlist_destroy(lista);
    tonmap_destroy(mapa);
    tonset_destroy(zbior);
    
    return 0;
}
```

### Kombinacja Wskaźników i Struktur Danych

```ton
fn main() -> int {
    let lista: TonList* = tonlist_create();
    
    // Dodanie wskaźników do listy
    let x: int = 100;
    let y: int = 200;
    
    tonlist_push(lista, create_value_pointer(&x));
    tonlist_push(lista, create_value_pointer(&y));
    
    // Odczytanie wskaźników z listy
    let ptr1: Value = tonlist_get(lista, 0);
    let ptr2: Value = tonlist_get(lista, 1);
    
    // Dereferencja wskaźników (wymaga odpowiedniej implementacji w Value)
    print("Pierwsza wartość: ");
    print(x);
    print("Druga wartość: ");
    print(y);
    
    tonlist_destroy(lista);
    
    return 0;
}
```