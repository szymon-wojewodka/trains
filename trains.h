#ifndef TRAINS_LIB
#define TRAINS_LIB

#include <algorithm>
#include <array>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <format>
#include <initializer_list>
#include <list>
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

namespace trains_details {
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

namespace main_depot_defaults {
  struct LocomotiveInfo {
    std::string designation;
    std::size_t max_wagons;
  };

  constexpr std::array<LocomotiveInfo, 2> locomotives = {
      LocomotiveInfo{.designation = "EU07-001", .max_wagons = 8},
      LocomotiveInfo{.designation = "EP09-020", .max_wagons = 12}};

  constexpr std::array<WagonSeries, 8> wagons = {
      STANDARD_FIRST_CLASS,  STANDARD_SECOND_CLASS, STANDARD_SECOND_CLASS,
      STANDARD_SECOND_CLASS, STANDARD_SECOND_CLASS, STANDARD_SLEEPER,
      STANDARD_SLEEPER,      STANDARD_DINING};
}  // namespace main_depot_defaults

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

  std::strong_ordering operator<=>(const Wagon& other) const {
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
    other.wagons_.clear();
    other.seats_ = 0;
  }

  Train& operator=(Train&& other) noexcept {
    if (this != &other) {
      wagons_ = std::move(other.wagons_);
      locomotive_ = std::move(other.locomotive_);
      seats_ = other.seats_;
      other.wagons_.clear();
      other.seats_ = 0;
    }
    return *this;
  }

  Train(const Train&) = delete;
  Train& operator=(const Train&) = delete;

  Train& operator+=(const Wagon& wagon) {
    if (can_attach(wagon)) {
      attach(Wagon{wagon});
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
  std::size_t seats_{0};

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

class Depot {
public:
  Depot() = default;
  Depot(const Depot&) = delete;
  Depot& operator=(const Depot&) = delete;

  Depot& operator+=(Wagon&& wagon) {
    if (wagon) {
      wagons_.push_back(std::move(wagon));
    }
    return *this;
  }

  Depot& operator+=(Locomotive&& locomotive) {
    if (locomotive) {
      locomotives_.push_back(std::move(locomotive));
    }
    return *this;
  }

  Locomotive release_locomotive() {
    if (locomotives_.empty()) {
      return Locomotive{};
    }
    Locomotive locomotive = std::move(locomotives_.front());
    locomotives_.pop_front();
    return locomotive;
  }

  Wagon release_wagon(WagonSeries series) {
    auto it = std::ranges::find_if(wagons_, [series](const Wagon& wagon) {
      return wagon.series() == series;
    });
    if (it == wagons_.end()) {
      return Wagon{};
    }
    Wagon wagon = std::move(*it);
    wagons_.erase(it);
    return wagon;
  }

  std::size_t locomotives() const {
    return locomotives_.size();
  }

  std::size_t wagons() const {
    return wagons_.size();
  }

  std::size_t wagons(WagonSeries series) const {
    return std::count_if(wagons_.begin(), wagons_.end(), [series](const Wagon& wagon) {
      return wagon.series() == series;
    });
  }

private:
  std::deque<Locomotive> locomotives_;
  std::list<Wagon> wagons_;
};

inline Depot& main_depot() {
  static bool initialized = false;
  static Depot depot;
  if (!initialized) {
    for (const auto& [designation, max_wagons] : main_depot_defaults::locomotives) {
      depot += Locomotive{designation, max_wagons};
    }
    for (const auto series : main_depot_defaults::wagons) {
      depot += Wagon{series};
    }
    initialized = true;
  }
  return depot;
}

class Dispatcher {
public:
  explicit Dispatcher(Depot& depot = main_depot()) : depot_(depot) {}
  Dispatcher(const Dispatcher&) = default;
  Dispatcher& operator=(const Dispatcher&) = delete;

  const Depot& yard() const {
    return depot_;
  }

  // Czy na pewno const?
  Train assemble(WagonSeries series, std::size_t count) const {
    if (depot_.locomotives() == 0) {
      return Train{};
    }
    Train result{depot_.release_locomotive()};
    while (count > 0 && result.size() < result.capacity()) {
      Wagon wagon = depot_.release_wagon(series);
      if (!wagon) {
        break;
      }
      result += std::move(wagon);
      --count;
    }
    return result;
  }

private:
  Depot& depot_;
};

#endif  // TRAINS_LIB
