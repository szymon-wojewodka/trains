#ifndef TRAINS_LIB
#define TRAINS_LIB

#include <cstddef>
#include <cstdint>
#include <string>

// TODO: aliasy dla size_t
// TODO: stale wagonseries, liczby miejsc
// TODO: const-correctness (metody tez)
// TODO: main_depot, daje referencje do zajezdni glownej,
//  ktora na start konstruuje sie z lokomotywami i wagonami
namespace trains_details {
  constinit std::size_t wagons_ever_built{0};
  constinit std::size_t wagons_in_service{0};
  // counter na id wagonu chyba
}  // namespace trains_details

// wszedzie =, += dajace referencje
// wszystkie liczby size_t
// aliasy dla typow (czy w namespace?)

enum class WagonType : uint8_t { FIRST_CLASS, SECOND_CLASS, SLEEPER, DINING };

// TODO:
//  ctors: 2 i 1 argumentowy, copy, move
//  brak default ctor
//  operator= copy, move
//  operator== (z niego !=), operator< itp. =delete
//  wypisywanie na strumien (operator<<)
//  wszystko musi byc constexpr (co to jest wszystko)
class WagonSeries {
 public:
  WagonType type() {
    // TODO
    return {};
  }

  std::size_t seats() {
    // TODO
    return 0;
  }

 private:
  WagonType type_;
  std::size_t seats_amount_;
};

// TODO:
// robienie unique_id
// default ctor (id = 0)
// ctor przyjmujacy WagonSeries, nowy id
// copy ctor - kopiuje WagonSeries, nowy id
// move ctor
// operator= copy z zachowaniem id, lub nowym jesli bez id
// operator= move (trzeba uwazac na x = std::move(x))
// niejawna konwersja na wagonseries
// konwersja na bool ?
// spaceship i ==
// static metoda in_service (istniejace wagony)
// static metoda ever_built (nadane numery) (mega useless??)
// wypisywanie na strumien
class Wagon {
 public:
  using unique_id_t = std::size_t;
  unique_id_t number() {
    // TODO
    return 0;
  }

  WagonSeries series() {
    // TODO
    return {};
  }

 private:
};

// gdy nazwa == "" -> nazwa = "?"
// default ctor
// move ctor, operator= z move
// kopiowanie = delete
// jawna konwersja na bool
// operator<<
class Locomotive {
 public:
  std::string designation() const {
    // TODO;
    return "";
  }

  std::size_t max_wagons() const {
    // TODO;
    return 0;
  }

 private:
  std::string designation_;
  std::size_t max_wagons_;
};

// przechowywanie listy wagonow do rozwazenia (stl)
// default ctor
// ctor z przenoszeniem lokomotywy, initializer list, musimy jakos pominac puste wagony
// move ctor, operator= z move, po move nie ma lokomotywy i wagonow (jest pusty)
// nie ma kopii
// operator+= lvalue -> kopia, rvalue -> move
// opeartor[] 0->max-1
// konwersja na bool
// operator<< korzystajaca z operator<< lokomotywy i wagonow
class Train {
 public:
  std::size_t size() {
    // TODO stl wie
    return 0;
  }

  std::size_t capacity() {
    // TODO lokomotywa.max_wagon()
    return 0;
  }

  std::size_t seats() const {
    // TODO trzymac licznik
    return 0;
  }

  const Locomotive& locomotive() const {
    // TODO
  }

 private:
};

// jakas struktura (pewnie queue)
// brak kopiowania
// operator+=  (dla wagonu tylko move)
// dziedziczenie po dwoch depotach?
//
class Depot {
 public:
  Locomotive& release_locomotive() {
    // TODO
  }

  Wagon& release_wagon() {
    // TODO
  }

  std::size_t locomotives() {
    // TODO
    return 0;
  }

  std::size_t wagons() {
    // TODO
    return 0;
  }

 private:
};

class Dispatcher {
 public:
  const Depot& yard() {
    // TODO
  }

  // metoda assemble
 private:
};

#endif  // TRAINS_LIB
