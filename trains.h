#ifndef TRAINS_LIB
#define TRAINS_LIB

#include <cstddef>

namespace trains_details {
  // counter na id wagonu chyba
}

// wszedzie =, += dajace referencje
// wszystkie liczby size_t
// aliasy dla typow (czy w namespace?)

enum class WagonType { FIRST_CLASS, SECOND_CLASS, SLEEPER, DINING };

// TODO:
//  ctors: 2 i 1 argumentowy, copy, move
//  brak default ctor
//  operator= copy, move
//  operator== (z niego !=), operator< itp. =delete
//  wypisywanie na strumien (nie wiem czy chodzi o cos typu toString?)
class WagonSeries {
 public:
  WagonType type() {
    // TODO
  }

  size_t seats() {
    // TODO
  }

 private:
  WagonType type;
  // size_t seats_amount;
};

// TODO:
// robienie unique_id
// default ctor (id = 0)
// ctor przyjmujacy WagonSeries
// copy ctor - kopiuje WagonSeries, nowy id
// move ctor
// operator= copy z zachowaniem id, lub nowym jesli bez id
// operator= move (trzeba uwazac na x = std::move(x))
// konwersja na bool ?
// spaceship i ==
// static metoda in_service (istniejace wagony)
// static metoda ever_built (nadane numery) (mega useless??)
// wypisywanie na strumien
class Wagon {
 public:
  unique_id_t number() {
    // TODO
  }

  WagonSeries series() {
    // TODO
  }

 private:
};

class Locomotive {
 public:
 private:
};

// kopiowanie wagonu czy nowe id?
class Train {
 public:
 private:
};

class Depot {
 public:
 private:
};

class Dispatcher {
 public:
 private:
};

#endif  // TRAINS_LIB
