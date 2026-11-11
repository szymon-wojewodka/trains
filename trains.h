#ifndef TRAINS_LIB
#define TRAINS_LIB

#include <cstddef>
#include <cstdint>
#include <deque>
#include <format>
#include <initializer_list>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

enum class WagonType : std::uint8_t {
  FIRST_CLASS,
  SECOND_CLASS,
  SLEEPER,
  DINING
};

// TODO: aliasy dla size_t
// TODO: const-correctness (metody tez)
// TODO: main_depot, daje referencje do zajezdni glownej,
//  ktora na start konstruuje sie z lokomotywami i wagonami
namespace trains_details {
  // counter na id wagonu chyba

  struct WagonInfo {
    std::size_t seat_count;
    std::string_view name;
  };

  namespace standard_series {
    constexpr WagonInfo FIRST_CLASS = {.seat_count = 54, .name = "1 klasa"};
    constexpr WagonInfo SECOND_CLASS = {.seat_count = 80, .name = "2 klasa"};
    constexpr WagonInfo SLEEPER = {.seat_count = 30, .name = "sypialny"};
    constexpr WagonInfo DINING = {.seat_count = 32, .name = "restauracyjny"};
  }  // namespace standard_series

  constexpr WagonInfo get_wagon_info(WagonType type) {
    using enum WagonType;
    switch (type) {
      case FIRST_CLASS:
        return standard_series::FIRST_CLASS;
      case SECOND_CLASS:
        return standard_series::SECOND_CLASS;
      case SLEEPER:
        return standard_series::SLEEPER;
      case DINING:
        return standard_series::DINING;
    }
    std::unreachable();
  }
}  // namespace trains_details

// wszedzie =, += dajace referencje
// wszystkie liczby size_t
// aliasy dla typow (czy w namespace?)

class WagonSeries {
 public:
  constexpr explicit WagonSeries(WagonType type, std::size_t seats)
      : type_(type), seats_count_(seats) {}

  // Not explicit on purpose.
  constexpr WagonSeries(WagonType type)
      : WagonSeries(type, trains_details::get_wagon_info(type).seat_count) {}

  WagonSeries() = delete;
  constexpr WagonSeries(const WagonSeries&) = default;
  constexpr WagonSeries(WagonSeries&&) = default;

  constexpr WagonSeries& operator=(const WagonSeries&) = default;
  constexpr WagonSeries& operator=(WagonSeries&&) = default;
  constexpr bool operator==(const WagonSeries&) const = default;

  constexpr WagonType type() const {
    return type_;
  }

  constexpr std::size_t seats() const {
    return seats_count_;
  }

 private:
  WagonType type_;
  std::size_t seats_count_;
};

inline std::ostream& operator<<(std::ostream& out, const WagonSeries& series) {
  return out << std::format("[{}, miejsca: {}]",
                            trains_details::get_wagon_info(series.type()).name,
                            series.seats());
}

inline constexpr WagonSeries STANDARD_FIRST_CLASS{WagonType::FIRST_CLASS};
inline constexpr WagonSeries STANDARD_SECOND_CLASS{WagonType::SECOND_CLASS};
inline constexpr WagonSeries STANDARD_SLEEPER{WagonType::SLEEPER};
inline constexpr WagonSeries STANDARD_DINING{WagonType::DINING};

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
//
// static metoda in_service (istniejace wagony)
// Utrzymanie licznika in service w:
//  - konstruktorach:
//    - bezargumentowym: 0
//    - Wagon(seria): +1
//    - kopiującym: wagon niepusty ? +1 : 0
//    - przenoszącym: 0
//  - operatorach:
//    - "=" :
//      - przenoszącym:
//        - a niepusty ? -1 : 0
//      - kopiującym:
//        - a pusty i b niepusty ? +1
//        - a niepusty i b pusty ? -1
//        - wpp (I): 0
//        - edge case z przypisaniem na siebie: 0
//          to jest szczególny przypadek (I):
//            Jeśli a = b to a pusty -> b pusty i a niepusty -> b niepusty
//   - destruktorze:
//      - wagon niepusty ? -1 : 0
//      - przy przenoszeniu (operator, konstruktor) trzeba jawnie uczynic ten
//        przenoszony pustym zeby nie powtarzac odejmowania w desktruktorze
//  Więcej grzechów nie pamiętam
//
// static metoda ever_built (nadane numery) (mega useless??)
// wypisywanie na strumien
class Wagon {
 public:
  using unique_id_t = std::size_t;

  Wagon() : number_(EMPTY_NUMBER), series_(WagonType::SLEEPER) {}
  explicit Wagon(WagonSeries series)
      : number_(build_wagon()), series_(series) {}

  Wagon(const Wagon& wagon)
      : number_(wagon ? build_wagon() : EMPTY_NUMBER), series_(wagon.series()) {}

  Wagon(Wagon&& wagon) noexcept
      : number_(wagon.number()), series_(wagon.series()) {
    wagon.number_ = EMPTY_NUMBER;
  }

  ~Wagon() {
    if (*this) {
      destroy_wagon();
    }
  }

  Wagon& operator=(const Wagon& wagon) {
    series_ = wagon.series();
    if (!wagon && *this) {
      number_ = EMPTY_NUMBER;
      destroy_wagon();
    } else if (wagon && !(*this)) {
      number_ = build_wagon();
    }
    return *this;
  }

  Wagon& operator=(Wagon&& wagon) noexcept {
    if (&wagon != this) {
      if (*this) {
        destroy_wagon();
      }
      number_ = wagon.number();
      series_ = wagon.series();
      wagon.number_ = EMPTY_NUMBER;
    }
    return *this;
  }

  auto operator<=>(const Wagon& other) const {
    return number_ <=> other.number();
  }

  bool operator==(const Wagon& other) const {
    return other.number() == number_;
  }

  explicit operator bool() const {
    return number_ != EMPTY_NUMBER;
  }

  operator WagonSeries() const {
    return series_;
  }

  unique_id_t number() const {
    return number_;
  }

  WagonSeries series() const {
    return series_;
  }

  static std::size_t in_service() {
    return wagons_in_service;
  }

  static std::size_t ever_built() {
    return wagons_ever_built;
  }

 private:
  static constexpr unique_id_t EMPTY_NUMBER = 0;
  unique_id_t number_;
  WagonSeries series_;
  inline static constinit std::size_t wagons_ever_built{0};
  inline static constinit std::size_t wagons_in_service{0};

  static std::size_t build_wagon() {
    ++wagons_ever_built;
    ++wagons_in_service;
    return wagons_ever_built;
  }

  static void destroy_wagon() {
    --wagons_in_service;
  }
};

inline std::ostream& operator<<(std::ostream& out, const Wagon& wagon) {
  if (wagon) {
    return out << "Wagon nr " << wagon.number() << ' ' << wagon.series();
  }
  return out << "Wagon pusty";
}

// gdy nazwa == "" -> nazwa = "?"
// default ctor
// move ctor, operator= z move
// kopiowanie = delete
// jawna konwersja na bool
// operator<<
class Locomotive {
 public:
  explicit Locomotive(std::string designation, std::size_t max_wagons)
      : designation_(designation.empty() ? "?" : std::move(designation)),
        max_wagons_(max_wagons) {}

  Locomotive() = default;
  Locomotive(const Locomotive&) = delete;
  Locomotive(Locomotive&& other) noexcept
      : designation_(std::move(other.designation_)), max_wagons_(other.max_wagons_) {
    other.designation_.clear();
    other.max_wagons_ = 0;
  }

  Locomotive& operator=(const Locomotive&) = delete;

  Locomotive& operator=(Locomotive&& other) noexcept {
    if (&other != this) {
      designation_ = std::move(other.designation_);
      other.designation_.clear();
      max_wagons_ = other.max_wagons_;
      other.max_wagons_ = 0;
    }
    return *this;
  }

  explicit operator bool() const {
    return !designation_.empty();
  }

  const std::string& designation() const {
    return designation_;
  }

  std::size_t max_wagons() const {
    return max_wagons_;
  }

 private:
  std::string designation_;
  std::size_t max_wagons_{0};
};

inline std::ostream& operator<<(std::ostream& out, const Locomotive& locomotive) {
  if (locomotive) {
    return out << std::format("{} (uciąg: {})", locomotive.designation(),
                              locomotive.max_wagons());
  }
  return out << "brak lokomotywy";
}

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
  Train() = default;

  explicit Train(Locomotive&& locomotive, std::initializer_list<Wagon> wagons = {})
      : locomotive_(std::move(locomotive)) {
    for (const auto& wagon : wagons) {
      *this += wagon;
    }
  }

  Train(Train&& other) noexcept
      : locomotive_(std::move(other.locomotive_)),
        wagons_(std::move(other.wagons_)),
        seats_(other.seats_) {
    other.locomotive_ = {};
    other.wagons_ = {};
    other.seats_ = 0;
  }
  Train& operator=(Train&& other) noexcept {
    if (this != &other) {
      wagons_ = std::move(other.wagons_);
      locomotive_ = std::move(other.locomotive_);
      seats_ = other.seats_;
      other.locomotive_ = {};
      other.wagons_ = {};
      other.seats_ = 0;
    }
    return *this;
  }

  Train(const Train&) = delete;
  Train& operator=(const Train&) = delete;

  Train& operator+=(const Wagon& wagon) {
    if (can_attach(wagon)) {
      Wagon wagon_copy{wagon};
      attach(std::move(wagon_copy));
    }
    return *this;
  }
  Train& operator+=(Wagon&& wagon) {
    if (can_attach(wagon)) {
      attach(std::move(wagon));
    }
    return *this;
  }

  const Wagon& operator[](std::size_t position) const {
    return wagons_[position];
  }

  explicit operator bool() const {
    return static_cast<bool>(locomotive_);
  }

  std::size_t size() const {
    return wagons_.size();
  }

  std::size_t capacity() const {
    return locomotive_.max_wagons();
  }

  std::size_t seats() const {
    return seats_;
  }

  const Locomotive& locomotive() const {
    return locomotive_;
  }

 private:
  Locomotive locomotive_;
  std::deque<Wagon> wagons_;
  size_t seats_{0};

  bool is_full() const {
    return wagons_.size() >= locomotive_.max_wagons();
  }

  void attach(Wagon&& wagon) {
    seats_ += wagon.series().seats();
    wagons_.push_back(std::move(wagon));
  }

  bool can_attach(const Wagon& wagon) const {
    return wagon && !is_full();
  }
};

inline std::ostream& operator<<(std::ostream& out, const Train& train) {
  out << "Skład: " << train.locomotive();
  for (std::size_t i = 0; i < train.size(); i++) {
    out << "\n\t" << train[i];
  }
  return out;
}

// jakas struktura (pewnie queue)
// brak kopiowania
// operator+=  (dla wagonu tylko move)
// dziedziczenie po dwoch depotach?
//
class Depot {
 public:
  Depot() = default;
  Depot(const Depot&) = delete;
  void operator=(const Depot&) = delete;


  void operator+=(Locomotive&& locomotive) {
    locomotives_.push_back(std::move(locomotive));
  }

  Locomotive release_locomotive() {
    if (locomotives_.empty()) {
      return Locomotive{};
    }
    Locomotive locomotive = std::move(locomotives_.front());
    locomotives_.pop_front();
    return locomotive;
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
  std::deque<Locomotive> locomotives_;
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
