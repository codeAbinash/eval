#ifndef COLOR_HPP
#define COLOR_HPP

#include <windows.h>
#include <algorithm>
#include <list>
#include <unordered_map>

// #include <iostream>
// #include <string>
// #include <type_traits>
// #include <utility>

namespace hue {
constexpr int DEFAULT_COLOR = 7;
constexpr int BAD_COLOR = -256;

const std::unordered_map<std::string, int> CODES = {
    {"black", 0},         {"k", 0},   {"blue", 1},          {"b", 1},
    {"green", 2},         {"g", 2},   {"aqua", 3},          {"a", 3},
    {"red", 4},           {"r", 4},   {"purple", 5},        {"p", 5},
    {"yellow", 6},        {"y", 6},   {"white", 7},         {"w", 7},
    {"grey", 8},          {"e", 8},   {"light blue", 9},    {"lb", 9},
    {"light green", 10},  {"lg", 10}, {"light aqua", 11},   {"la", 11},
    {"light red", 12},    {"lr", 12}, {"light purple", 13}, {"lp", 13},
    {"light yellow", 14}, {"ly", 14}, {"bright white", 15}, {"bw", 15}};

const std::unordered_map<int, std::string> NAMES = {
    {0, "black"},        {1, "blue"},          {2, "green"},
    {3, "aqua"},         {4, "red"},           {5, "purple"},
    {6, "yellow"},       {7, "white"},         {8, "grey"},
    {9, "light blue"},   {10, "light green"},  {11, "light aqua"},
    {12, "light red"},   {13, "light purple"}, {14, "light yellow"},
    {15, "bright white"}};

inline bool is_good(int c) { return 0 <= c && c < 256; }

inline int itoc(int c) { return is_good(c) ? c : BAD_COLOR; }

inline int itoc(int a, int b) { return itoc(a + b * 16); }

// std::string to color
int stoc(std::string a) {
    // convert s to lowercase, and format variants like  "light_blue"
    std::transform(a.begin(), a.end(), a.begin(), [](char c) {
        if ('A' <= c && c <= 'Z')
            c = c - 'A' + 'a';
        else if (c == '_' || c == '-')
            c = ' ';
        return c;
    });

    // operator[] on std::unordered_map is non-const, use std::unordered_map::at
    // instead
    return (CODES.find(a) != CODES.end()) ? CODES.at(a) : BAD_COLOR;
}

int stoc(std::string a, std::string b) { return itoc(stoc(a), stoc(b)); }

std::string ctos(int c) {
    return (0 <= c && c < 256) ? "(text) " + NAMES.at(c % 16) + " + " +
                                     "(background) " + NAMES.at(c / 16)
                               : "BAD COLOR";
}

int get() {
    CONSOLE_SCREEN_BUFFER_INFO i;
    return GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &i)
               ? i.wAttributes
               : BAD_COLOR;
}

int get_text() { return (get() != BAD_COLOR) ? get() % 16 : BAD_COLOR; }

int get_background() { return (get() != BAD_COLOR) ? get() / 16 : BAD_COLOR; }

void set(int c) {
    if (is_good(c)) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void set(int a, int b) { set(a + b * 16); }

void set(std::string a, std::string b) { set(stoc(a) + stoc(b) * 16); }

void set_text(std::string a) { set(stoc(a), get_background()); }

void set_background(std::string b) { set(get_text(), stoc(b)); }

void reset() { set(DEFAULT_COLOR); }

int invert(int c) {
    if (is_good(c)) {
        int a = c % 16;
        int b = c / 16;
        return b + a * 16;
    } else
        return BAD_COLOR;
}

std::ostream &yellow(std::ostream &os) {set_text("y");return os;}
std::ostream &light_blue(std::ostream &os) {set_text("lb");return os;}
std::ostream &light_green(std::ostream &os) {set_text("lg");return os;}
std::ostream &light_red(std::ostream &os) {set_text("lr");return os;}
std::ostream & light_yellow(std::ostream & os) { set_text("ly");return os; }
std::ostream & on_grey(std::ostream & os) { set_background("e");    return os; }
    std::ostream & grey(std::ostream & os)                  { set_text("e");          return os; }


}  // namespace hue

namespace dye {
template <typename T>
using bar = typename std::conditional<std::is_same<T, const char *>::value,
                                      std::string, T>::type;

template <typename T>
class colorful;
template <typename T>
class item;

template <typename T>
class colorful : private std::list<item<T>> {
   public:
    using std::list<item<T>>::list;

    colorful<T> &operator+=(const colorful<T> &rhs) {
        this->insert(this->end(), rhs.begin(), rhs.end());
        return *this;
    }

    colorful<T> &operator+=(colorful<T> &&rhs) {
        this->splice(this->end(), std::move(rhs));
        return *this;
    }

    colorful<T> &operator+=(T t) {
        this->push_back(std::move(t));
        return *this;
    }

    void push_front(T t) {
        this->std::list<item<T>>::push_front(item<T>(std::move(t)));
    }

    void push_back(T t) {
        this->std::list<item<T>>::push_back(item<T>(std::move(t)));
    }

    colorful<T> &invert() {
        for (auto &elem : *this) elem.invert();
        return *this;
    }

    template <typename U>
    friend std::ostream &operator<<(std::ostream &, const colorful<U> &);

    template <typename U>
    friend colorful<U> invert(colorful<U> col);
};

template <typename T>
colorful<T> operator+(colorful<T> lhs, colorful<T> rhs) {
    colorful<T> res(std::move(lhs));
    return res += rhs;
}

template <typename T>
colorful<T> operator+(colorful<T> lhs, std::string rhs) {
    colorful<T> res(std::move(lhs));
    res.push_back(std::move(rhs));
    return res;
}

template <typename T>
colorful<T> operator+(const std::string &lhs, colorful<T> rhs) {
    colorful<T> res(std::move(rhs));
    res.push_front(std::move(lhs));
    return res;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const colorful<T> &colorful) {
    for (const auto &elem : colorful) os << elem;
    return os;
}

template <typename T>
colorful<T> invert(colorful<T> col) {
    colorful<T> res(std::move(col));
    for (auto &elem : res) elem.invert();
    return res;
}

template <typename T>
class item {
    T thing;
    int color;

   public:
    item(T t) : thing(std::move(t)), color(hue::get()) {}
    item(T t, int a) : thing(std::move(t)), color(hue::itoc(a)) {}
    item(T t, int a, int b) : thing(std::move(t)), color(hue::itoc(a, b)) {}
    item(T t, std::string a) : thing(std::move(t)), color(hue::stoc(a)) {}
    item(T t, std::string a, std::string b)
        : thing(std::move(t)), color(hue::stoc(a, b)) {}

    item<T> &invert() {
        color = hue::invert(color);
        return *this;
    }

    template <typename U>
    friend class colorful;

    template <typename U>
    friend std::ostream &operator<<(std::ostream &, const item<U> &);
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const item<T> &it) {
    hue::set(it.color);
    os << it.thing;
    hue::reset();
    return os;
}

template <typename T>
using R = colorful<bar<T>>;
template <typename T>
using S = item<bar<T>>;

template <typename T>R<T> yellow(T t) {return R<T>{S<T>(t, "y")};}
template <typename T> R<T> light_blue(T t) {return R<T>{S<T>(t, "lb")};}
template <typename T>R<T> light_green(T t) {return R<T>{S<T>(t, "lg")};}
template <typename T>R<T> light_red(T t) {return R<T>{S<T>(t, "lr")};}
template <typename T>R<T> light_yellow(T t) { return R<T> { S<T>(t, "ly") }; }
template<typename T> R<T> on_grey(T t) { return R<T> { S<T>(t, "k", "e") }; }
    template<typename T> R<T> grey(T t)                         { return R<T> { S<T>(t, "e") }; }

}  // namespace dye

#endif
