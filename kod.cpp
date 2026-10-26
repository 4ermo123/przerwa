
#define RED_1   2
#define GREEN_1 6
#define RED_2   3
#define GREEN_2 7
#define RED_3   4
#define GREEN_3 8
#define RED_4   5
#define GREEN_4 9

#define PIR_1 10
#define PIR_2 11
#define PIR_3 12
#define PIR_4 13

// Kierunki: 0 = prawo, 1 = góra, 2 = lewo, 3 = dół
const byte redPins[4]   = {RED_1, RED_2, RED_3, RED_4};
const byte greenPins[4] = {GREEN_1, GREEN_2, GREEN_3, GREEN_4};

const char* nazwyKierunkow[4] = {
  "DOL", "PRAWO", "GORA", "LEWO"
};

const unsigned long CZAS_ZIELONEGO = 5000;

// Zmienne współdzielone z przerwaniami
volatile bool kolejka[4] = {false, false, false, false};

volatile byte czujnik1 = 0;
volatile byte czujnik2 = 0;
volatile byte czujnik3 = 0;
volatile byte czujnik4 = 0;

// 1 = RISING, 2 = FALLING
volatile byte typZmiany[4] = {0, 0, 0, 0};

// Flagi zdarzeń dla Serial Monitora
volatile byte zdarzenia = 0;

// Poprzedni stan portu B
volatile byte poprzedniPortB = 0;

int aktywny = -1;
unsigned long czasZmiany = 0;

// Deklaracje funkcji czujników
void ISR_czujnik1();
void ISR_czujnik2();
void ISR_czujnik3();
void ISR_czujnik4();

void setup() {
  Serial.begin(9600);

  for (byte i = 0; i < 4; i++) {
    pinMode(redPins[i], OUTPUT);
    pinMode(greenPins[i], OUTPUT);
  }

  pinMode(PIR_1, INPUT);
  pinMode(PIR_2, INPUT);
  pinMode(PIR_3, INPUT);
  pinMode(PIR_4, INPUT);

  wszystkieCzerwone();

  // Odczyt początkowego stanu pinów D10-D13
  byte port = PINB;
  poprzedniPortB = port;

  czujnik1 = (port & _BV(PB2)) ? 1 : 0;
  czujnik2 = (port & _BV(PB3)) ? 1 : 0;
  czujnik3 = (port & _BV(PB4)) ? 1 : 0;
  czujnik4 = (port & _BV(PB5)) ? 1 : 0;

  // Włączenie przerwań dla D10, D11, D12 i D13
  PCMSK0 |= _BV(PCINT2) |
            _BV(PCINT3) |
            _BV(PCINT4) |
            _BV(PCINT5);

  PCIFR |= _BV(PCIF0);
  PCICR |= _BV(PCIE0);

  Serial.println("Program uruchomiony");
  Serial.println("Oczekiwanie na czujniki PIR...");
}

// Główne przerwanie sprzętowe portu B
ISR(PCINT0_vect) {
  byte aktualnyPortB = PINB;
  byte zmienionePiny = aktualnyPortB ^ poprzedniPortB;

  poprzedniPortB = aktualnyPortB;

  if (zmienionePiny & _BV(PB2)) {
    ISR_czujnik1();
  }

  if (zmienionePiny & _BV(PB3)) {
    ISR_czujnik2();
  }

  if (zmienionePiny & _BV(PB4)) {
    ISR_czujnik3();
  }

  if (zmienionePiny & _BV(PB5)) {
    ISR_czujnik4();
  }
}

// CZUJNIK 1 - PIN 10
void ISR_czujnik1() {
  byte stan = (PINB & _BV(PB2)) ? 1 : 0;
  czujnik1 = stan;

  if (stan == 1) {
    typZmiany[0] = 1;
    kolejka[0] = true;
  } else {
    typZmiany[0] = 2;
  }

  zdarzenia |= 0b00000001;
}

// CZUJNIK 2 - PIN 11
void ISR_czujnik2() {
  byte stan = (PINB & _BV(PB3)) ? 1 : 0;
  czujnik2 = stan;

  if (stan == 1) {
    typZmiany[1] = 1;
    kolejka[1] = true;
  } else {
    typZmiany[1] = 2;
  }

  zdarzenia |= 0b00000010;
}

// CZUJNIK 3 - PIN 12
void ISR_czujnik3() {
  byte stan = (PINB & _BV(PB4)) ? 1 : 0;
  czujnik3 = stan;

  if (stan == 1) {
    typZmiany[2] = 1;
    kolejka[2] = true;
  } else {
    typZmiany[2] = 2;
  }

  zdarzenia |= 0b00000100;
}

// CZUJNIK 4 - PIN 13
void ISR_czujnik4() {
  byte stan = (PINB & _BV(PB5)) ? 1 : 0;
  czujnik4 = stan;

  if (stan == 1) {
    typZmiany[3] = 1;
    kolejka[3] = true;
  } else {
    typZmiany[3] = 2;
  }

  zdarzenia |= 0b00001000;
}

void loop() {
  byte wynik;
  byte typ[4];
  byte stany[4];

  // Bezpieczne skopiowanie danych z ISR
  noInterrupts();

  wynik = zdarzenia;
  zdarzenia = 0;

  typ[0] = typZmiany[0];
  typ[1] = typZmiany[1];
  typ[2] = typZmiany[2];
  typ[3] = typZmiany[3];

  stany[0] = czujnik1;
  stany[1] = czujnik2;
  stany[2] = czujnik3;
  stany[3] = czujnik4;

  interrupts();

  // Wyświetlanie stanu i zbocza każdego czujnika
  for (byte i = 0; i < 4; i++) {
    if (wynik & (1 << i)) {
      Serial.print("Czujnik ");
      Serial.print(i + 1);

      Serial.print(" | Kierunek: ");
      Serial.print(nazwyKierunkow[i]);

      Serial.print(" | Stan: ");
      Serial.print(stany[i] ? "HIGH" : "LOW");

      Serial.print(" | Zdarzenie: ");

      if (typ[i] == 1) {
        Serial.println("RISING (LOW -> HIGH)");
      } else if (typ[i] == 2) {
        Serial.println("FALLING (HIGH -> LOW)");
      } else {
        Serial.println("CHANGE");
      }
    }
  }

  unsigned long teraz = millis();

  // Koniec zielonego po 5 sekundach
  if (aktywny != -1 &&
      teraz - czasZmiany >= CZAS_ZIELONEGO) {
    wszystkieCzerwone();

    Serial.println("Koniec zielonego - wszystkie czerwone");
    aktywny = -1;
  }

  // Obsługa kolejki samochodów
  if (aktywny == -1) {
    for (byte i = 0; i < 4; i++) {
      bool oczekuje;

      noInterrupts();
      oczekuje = kolejka[i];

      if (oczekuje) {
        kolejka[i] = false;
      }

      interrupts();

      if (oczekuje) {
        wlaczZielone(i);

        aktywny = i;
        czasZmiany = millis();

        Serial.print("Zielone dla kierunku: ");
        Serial.println(nazwyKierunkow[i]);

        break;
      }
    }
  }
}

void wlaczZielone(byte i) {
  wszystkieCzerwone();

  digitalWrite(redPins[i], LOW);
  digitalWrite(greenPins[i], HIGH);
}

void wszystkieCzerwone() {
  for (byte i = 0; i < 4; i++) {
    digitalWrite(redPins[i], HIGH);
    digitalWrite(greenPins[i], LOW);
  }
}
