# 🚦 Inteligentne skrzyżowanie – Arduino UNO

## 1\. Opis projektu

Projekt przedstawia symulację inteligentnego skrzyżowania z wykorzystaniem Arduino UNO, czterech czujników PIR oraz ośmiu diod LED imitujących sygnalizację świetlną.

System wykrywa pojawienie się samochodu na każdym z czterech kierunków. Na podstawie sygnałów z czujników dodaje kierunki do kolejki i przydziela zielone światło oczekującemu kierunkowi.

Program wykorzystuje `attachInterrupt()` dla dwóch czujników oraz kontrolę zmian stanu w funkcji `loop()` dla pozostałych dwóch.

## 2\. Wykorzystane elementy

- Arduino UNO
- 4 czujniki PIR
- 4 czerwone diody LED
- 4 zielone diody LED
- Rezystory do diod LED
- Przewody połączeniowe
- Tinkercad Circuits do symulacji

## 3\. Podłączenie elementów

### Czujniki PIR

| Czujnik | Kierunek | Pin Arduino |
| --- | --- | --- |
| PIR 1 | Dół | D2 |
| PIR 2 | Lewo | D3 |
| PIR 3 | Góra | D10 |
| PIR 4 | Prawo | D11 |

### Sygnalizacja świetlna

| Kierunek | Czerwone światło | Zielone światło |
| --- | --- | --- |
| Dół | D4 | D6 |
| Lewo | D5 | D9 |
| Góra | D7 | D12 |
| Prawo | D8 | D13 |

**Uwaga:** podłączenie musi być zgodne z definicjami pinów w kodzie programu.

## 4\. Działanie programu

1. Po uruchomieniu wszystkie kierunki mają czerwone światło.
2. Czujniki monitorują obecność samochodów.
3. Zmiana sygnału czujnika z `LOW` na `HIGH` oznacza wykrycie aktywnego sygnału i dodanie kierunku do kolejki.
4. Zmiana z `HIGH` na `LOW` oznacza zakończenie aktywnego sygnału czujnika.
5. Jeżeli żaden kierunek nie ma zielonego światła, program wybiera pierwszy oczekujący kierunek.
6. Wybrany kierunek otrzymuje zielone światło na 5 sekund.
7. Po upływie tego czasu wszystkie światła wracają do czerwonego, a program obsługuje następny oczekujący kierunek.

## 5\. Obsługa przerwań

W programie zastosowano dwa mechanizmy wykrywania zmian sygnału.

### `attachInterrupt()`

Dla czujników podłączonych do pinów D2 i D3 użyto funkcji `attachInterrupt()` z trybem `CHANGE`.

cpp

```
attachInterrupt(
  digitalPinToInterrupt(PIR_1),
  ISR_czujnik1,
  CHANGE
);
```

Tryb `CHANGE` wykrywa zarówno zmianę `LOW → HIGH`, jak i `HIGH → LOW`.

### Kontrola zmian w `loop()`

Czujniki podłączone do pinów D10 i D11 są obsługiwane przez porównanie aktualnego i poprzedniego stanu wejścia. Zmiana stanu powoduje wywołanie odpowiedniej funkcji obsługi.

**Ważne:** funkcje obsługujące czujniki 3 i 4 nie są rzeczywistymi przerwaniami sprzętowymi. W Arduino UNO standardowe `attachInterrupt()` jest dostępne na pinach D2 i D3.

## 6\. Funkcje obsługi czujników

Program zawiera cztery osobne funkcje:

- `ISR_czujnik1()` – obsługa czujnika 1.
- `ISR_czujnik2()` – obsługa czujnika 2.
- `ISR_czujnik3()` – obsługa czujnika 3.
- `ISR_czujnik4()` – obsługa czujnika 4.

Zmienne współdzielone pomiędzy przerwaniami i pętlą główną są oznaczone słowem kluczowym `volatile`.

## 7\. Rozpoznawanie zmian sygnału

| Zdarzenie | Znaczenie |
| --- | --- |
| `RISING` | Zmiana z `LOW` na `HIGH` |
| `FALLING` | Zmiana z `HIGH` na `LOW` |
| `CHANGE` | Dowolna zmiana stanu sygnału |
| `LOW` | Sygnał znajduje się w stanie niskim |
| `HIGH` | Sygnał znajduje się w stanie wysokim |

W programie zdarzenia `RISING` i `FALLING` są rozpoznawane na podstawie odczytanego stanu wejścia.

## 8\. Kolejka samochodów

Każdy kierunek posiada własną zmienną kolejki:

- `kolejka[0]` – dół
- `kolejka[1]` – lewo
- `kolejka[2]` – góra
- `kolejka[3]` – prawo

Jeśli kilka czujników wykryje samochody, program obsługuje oczekujące kierunki w kolejności sprawdzania tablicy, zaczynając od dołu.

## 9\. Testowanie w Tinkercadzie

1. Otwórz projekt w Tinkercad Circuits.
2. Wklej kod do edytora Arduino w trybie tekstowym.
3. Kliknij **Start Simulation**.
4. Otwórz Serial Monitor i ustaw prędkość `9600`.
5. Zmień stan wyjścia czujnika na `HIGH`, a następnie na `LOW`.
6. Sprawdź komunikaty o stanie czujnika, zdarzeniach oraz kierunku otrzymującym zielone światło.

Przykładowy komunikat:

text

```
Czujnik 1 | Kierunek: DOL | Stan: HIGH | Zdarzenie: RISING (LOW -> HIGH)
Zielone dla kierunku: DOL
```

## 10\. Podsumowanie

Projekt prezentuje wykorzystanie Arduino UNO do sterowania sygnalizacją świetlną na podstawie sygnałów z czujników. Zastosowanie przerwań dla dwóch czujników oraz kontroli zmian stanu dla pozostałych pozwala porównać oba sposoby obsługi wejść cyfrowych.

Projekt ma charakter edukacyjny i jest przeznaczony do symulacji w Tinkercadzie.
