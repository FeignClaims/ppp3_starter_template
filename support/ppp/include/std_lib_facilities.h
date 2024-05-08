/*
std_lib_facilities.h
*/

/*
simple "Programming: Principles and Practice using C++ (second edition)" course header to
be used for the first few weeks.
It provides the most common standard headers (in the global namespace)
and minimal exception/error support.

Students: please don't try to understand the details of headers just yet.
All will be explained. This header is primarily used so that you don't have
to understand every concept all at once.

By Chapter 10, you don't need this file and after Chapter 21, you'll understand it

Revised April 25, 2010: simple_error() added

Revised November 25 2013: remove support for pre-C++11 compilers, use C++11: <chrono>
Revised November 28 2013: add a few container algorithms
Revised June 8 2014: added #ifndef to workaround Microsoft C++11 weakness
Revised Febrary 2 2015: randint() can now be seeded (see exercise 5.13).
Revised August 3, 2020: a cleanup removing support for ancient compilers
*/

#ifndef H112
#define H112 080315L

// NOLINTBEGIN
#include <algorithm>      // IWYU pragma: export
#include <array>          // IWYU pragma: export
#include <cmath>          // IWYU pragma: export
#include <cstdint>        // IWYU pragma: export
#include <cstdlib>        // IWYU pragma: export
#include <forward_list>   // IWYU pragma: export
#include <fstream>        // IWYU pragma: export
#include <iomanip>        // IWYU pragma: export
#include <iostream>       // IWYU pragma: export
#include <list>           // IWYU pragma: export
#include <random>         // IWYU pragma: export
#include <regex>          // IWYU pragma: export
#include <sstream>        // IWYU pragma: export
#include <stdexcept>      // IWYU pragma: export
#include <string>         // IWYU pragma: export
#include <unordered_map>  // IWYU pragma: export
#include <vector>         // IWYU pragma: export

//------------------------------------------------------------------------------

using Unicode = std::int64_t;

//------------------------------------------------------------------------------

using namespace std;

template <class T>
string to_string(T const& t) {
  ostringstream os;
  os << t;
  return os.str();
}

struct Range_error : out_of_range {  // enhanced vector range error reporting
  int index;
  Range_error(int i) : out_of_range("Range error: " + to_string(i)), index(i) {}
};

// trivially range-checked vector (no iterator checking):
template <class T>
struct Vector : public std::vector<T> {
  using size_type = typename std::vector<T>::size_type;

  /* #ifdef _MSC_VER
	// microsoft doesn't yet support C++11 inheriting constructors
	Vector() { }
	explicit Vector(size_type n) :std::vector<T>(n) {}
	Vector(size_type n, const T& v) :std::vector<T>(n, v) {}
	template <class I>
	Vector(I first, I last) : std::vector<T>(first, last) {}
	Vector(initializer_list<T> list) : std::vector<T>(list) {}
*/
  using std::vector<T>::vector;  // inheriting constructor

  T& operator[](unsigned int i)  // rather than return at(i);
  {
    if (i < 0 || this->size() <= i) {
      throw Range_error(i);
    }
    return std::vector<T>::operator[](i);
  }
  T const& operator[](unsigned int i) const {
    if (i < 0 || this->size() <= i) {
      throw Range_error(i);
    }
    return std::vector<T>::operator[](i);
  }
};

// disgusting macro hack to get a range checked vector:
#define vector Vector

// trivially range-checked string (no iterator checking):
struct String : std::string {
  using size_type = std::string::size_type;
  //	using string::string;

  char& operator[](unsigned int i)  // rather than return at(i);
  {
    if (i < 0 || size() <= i) {
      throw Range_error(i);
    }
    return std::string::operator[](i);
  }

  char const& operator[](unsigned int i) const {
    if (i < 0 || size() <= i) {
      throw Range_error(i);
    }
    return std::string::operator[](i);
  }
};

namespace std {

template <>
struct hash<String> {
  size_t operator()(String const& s) const {
    return hash<std::string>()(s);
  }
};

}  // namespace std

struct Exit : runtime_error {
  Exit() : runtime_error("Exit") {}
};

// error() simply disguises throws:
inline void error(string const& s) {
  throw runtime_error(s);
}

inline void error(string const& s, string const& s2) {
  error(s + s2);
}

inline void error(string const& s, int i) {
  ostringstream os;
  os << s << ": " << i;
  error(os.str());
}

template <class T>
char* as_bytes(T& i)  // needed for binary I/O
{
  void* addr = &i;  // get the address of the first byte
  // of memory used to store the object
  return static_cast<char*>(addr);  // treat that memory as bytes
}

inline void keep_window_open() {
  cin.clear();
  cout << "Please enter a character to exit\n";
  char ch{};
  cin >> ch;
}

inline void keep_window_open(string const& s) {
  if (s == "") {
    return;
  }
  cin.clear();
  cin.ignore(120, '\n');
  for (;;) {
    cout << "Please enter " << s << " to exit\n";
    string ss;
    while (cin >> ss && ss != s) {
      cout << "Please enter " << s << " to exit\n";
    }
    return;
  }
}

// error function to be used (only) until error() is introduced in Chapter 5:
inline void simple_error(string const& s)  // write ``error: s and exit program
{
  cerr << "error: " << s << '\n';
  keep_window_open();  // for some Windows environments
  exit(1);
}

// make std::min() and std::max() accessible on systems with antisocial macros:
#undef min
#undef max

// run-time checked narrowing cast (type conversion). See ???.
template <class R, class A>
R narrow_cast(A const& a) {
  R r = R(a);
  if (A(r) != a) {
    error(string("info loss"));
  }
  return r;
}

// random number generators. See 24.7.

inline default_random_engine& get_rand() {
  static default_random_engine ran;  // note: not thread_local
  return ran;
}

inline void seed_randint(int s) {
  get_rand().seed(s);
}

inline int randint(int min, int max) {
  return uniform_int_distribution<>{min, max}(get_rand());
}

inline int randint(int max) {
  return randint(0, max);
}

//inline double sqrt(int x) { return sqrt(double(x)); }	// to match C++0x

// container algorithms. See 21.9.   // C++ has better versions of this:

template <typename C>
using Value_type = typename C::value_type;

template <typename C>
using Iterator = typename C::iterator;

template <typename C>
// requires Container<C>()
void sort(C& c) {
  std::sort(c.begin(), c.end());
}

template <typename C, typename Pred>
// requires Container<C>() && Binary_Predicate<Value_type<C>>()
void sort(C& c, Pred p) {
  std::sort(c.begin(), c.end(), p);
}

template <typename C, typename Val>
// requires Container<C>() && Equality_comparable<C,Val>()
Iterator<C> find(C& c, Val v) {
  return std::find(c.begin(), c.end(), v);
}

template <typename C, typename Pred>
// requires Container<C>() && Predicate<Pred,Value_type<C>>()
Iterator<C> find_if(C& c, Pred p) {
  return std::find_if(c.begin(), c.end(), p);
}
// NOLINTEND

#endif  //H112