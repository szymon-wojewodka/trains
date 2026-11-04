#ifndef TRAINS_LIB
#define TRAINS_LIB

#include <cstddef>

//TODO: aliasy dla size_t
//TODO: stale wagonseries, liczby miejsc
//TODO: const-correctness (metody tez) 
//TODO: main_depot, daje referencje do zajezdni glownej,
// ktora na start konstruuje sie z lokomotywami i wagonami 
namespace trains_details {
  constinit std::size_t wagons_ever_built{0};
  constinit std::size_t wagons_in_service{0};
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
//  wypisywanie na strumien (operator<<)
//  wszystko musi byc constexpr (co to jest wszystko)
class WagonSeries {
 public:
  WagonType type() {
    // TODO
  }

  std::size_t seats() {
    // TODO
  }

 private:
  WagonType type;
  std::size_t seats_amount;
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
  }

  WagonSeries series() {
    // TODO
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
      //TODO;
    }

    std::size_t max_wagons() const {
      //TODO;
    }

  private:
    std::string designation;
    std::size_t max_wagons;
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
     //TODO stl wie 
   }

   std::size_t capacity() {
      //TODO lokomotywa.max_wagon()
   }

   std::size_t seats() const {
      //TODO trzymac licznik
   }

   const Locomotive& locomotive() const {
     //TODO
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
     //TODO
   }

   Wagon& release_wagon() {
     //TODO
   }

   std::size_t locomotives() {
     //TODO
   }

   std::size_t wagons() {
     //TODO
   }
 private:

};

class Dispatcher {
 public:
   const Depot& yard() {
     //TODO
   }

   // metoda assemble
 private:
};

#endif  // TRAINS_LIB
