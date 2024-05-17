#ifndef __TYPES_H__
#define __TYPES_H__

#include <cstdint>
#include <format>
#include <string>
#include <unordered_map>
#include <vector>

#include <CLS.pb.h>
#include <bitset>
#include <chrono>
#include <sstream>
#include <fmt/core.h>

// Serialize ordinal properties
#define TORD(m, f) m[#f] = to_rdstr(f)
#define VTORD(m, f) m.push_back( std::make_pair( #f, to_rdstr(f) ) );

// To serialize vector-like properties
#define TORDV(m, f) m[#f] = to_rdstrv(f)
#define VTORDV(m, f) m.push_back( std::make_pair( #f, to_rdstrv(f) ) );

template <typename T>
std::string to_rdstrv(const std::vector<T>& v) {
	std::stringstream ss;	//it is slower in ~2.3times then printf-family http://www.fastformat.org/performance.html
	for (const T& t : v) {
		t.to_rds(ss);
		ss << ';';
	}
	return ss.str();
}

template <>
std::string to_rdstrv(const std::vector<uint32_t>& v);

template <typename T>
void rdcast(const std::string_view& s, T& t){

};


typedef std::vector<std::pair<std::string, std::string>> hvector;// std::flat_map?
typedef std::unordered_map<std::string, std::string> hmap;// std::flat_map?
typedef std::unordered_set<std::string> hset;// std::flat_map?

typedef uint32_t tzid;

template <>
void rdcast(const std::string_view& s, bool& t);

template <>
inline void rdcast(const std::string_view& s, uint8_t& t) {
	if(!s.empty())
		t = s[0];
}

template <>
inline void rdcast(const std::string_view& s, std::string& t) {
	t = s;
}

template <>
void rdcast(const std::string_view& s, int16_t& t);

template <>
void rdcast(const std::string_view& s, uint16_t& t);

template <>
void rdcast(const std::string_view& s, int32_t& t);

template <>
void rdcast(const std::string_view& s, uint32_t& t);

template <>
void rdcast(const std::string_view& s, int64_t& t);

template <>
void rdcast(const std::string_view& s, uint64_t& t);

template <>
inline void rdcast(const std::string_view& s, hmap& t) {
	size_t i = 0, j;
	while ((j = s.find(';', i)) != std::string_view::npos) {

		size_t l = s.find('=', i);
		auto k = s.substr(i, l - i);
		auto v = s.substr(l + 1, j - l - 1);
		t[std::string(k)] = std::string(v);
		i = j+1;
	}
}

template <>
void rdcast(const std::string_view& s, cls_gen::RecalculationPeriodType& t);
template <>
void rdcast(const std::string_view& s, cls_gen::CounterSearchFlags& t);

template <typename T>
std::string to_rdstr(const T& t) {
	//return std::to_string(t);
	throw std::invalid_argument(typeid(T).name());// TODO compile-time error
}

template <>
inline std::string to_rdstr(const std::string& s) {
	return s;
}

template <>
inline std::string to_rdstr(const bool& t) {
	return t ? "1" : "0";
}

template <>
inline std::string to_rdstr(const uint8_t& t) {
	return std::to_string(t);
}

template <>
inline std::string to_rdstr(const int16_t& t) {
	return std::to_string(t);
}

template <>
inline std::string to_rdstr(const uint16_t& t) {
	return std::to_string(t);
}

template <>
inline std::string to_rdstr(const int32_t& t) {
	return std::to_string(t);
}

template <>
inline std::string to_rdstr(const uint32_t& t) {
	return std::to_string(t);
}

template <>
inline std::string to_rdstr(const int64_t& t) {
	return std::to_string(t);
}

template <>
inline std::string to_rdstr(const uint64_t& t) {
	return std::to_string(t);
}

template <>
inline std::string to_rdstr(const hmap& t) {
	std::stringstream ss;
	for (auto it = t.begin(); it != t.end(); ++it) {
		ss << it->first << '=' << it->second << ';';
	}
	return ss.str();
}

//Deserialize ordinal properties
#define RDFLD(m,f) rdfld(m, #f, f)
template <typename T>
void rdfld(const hmap& m, const std::string& name, T& t) {
	try
	{
		if (auto it = m.find(name); it != m.cend()) {
			rdcast<>(it->second, t);
		}
		else{
			t = T();//?
		}
	}
	catch(std::exception& e){
		//LOG_ERROR << std::format ( "Exception in counter read field:<> (method: rdfld(const hmap& m, const std::string& name, T& t))", name );
		throw;
	}
}

// Deserialize vector-like properties
#define RDFLDV(m,f) rdfldv(m, #f, f)
template <typename T>
inline void rdfldv(const hmap& m, const std::string& name, std::vector<T>& v) {
	try
	{
		if (auto it = m.find(name); it != m.cend()) {
			const std::string& s = it->second;
			size_t i = 0, j;
			while ((j = s.find(';', i)) != std::string::npos) {
				T t;
				rdcast(s.substr(i, j - i), t);
				v.push_back(t);
				i = j + 1;
			}
		}
	}
	catch(std::exception& e){
		//LOG_ERROR << std::format ( "Exception in counter read field:<{}> (method: rdfld(const hmap& m, const std::string& name, std::vector<T>& v))", name );
		throw;
	}	
}

typedef uint32_t ruleid_t;
typedef uint32_t tmplid_t;
typedef int64_t cntid_t;
typedef int64_t cntval_t;
typedef uint32_t profileid_t;
typedef uint32_t actcode_t;
typedef uint32_t billprdid_t;



/*enum WeekDays {
	Sunday = 0, Monday = 1, Tuesday = 3, Wednesday = 3, Thursday = 4, Friday = 5, Saturday = 6
};*/

enum Scale {
	One = 0,
	Ten = 1,
	Hundred = 2,
	Thousand = 3,
	TenThousand = 4,
	HundredThousand = 5,
	Mill = 6
};
template <>
inline void rdcast(const std::string_view& s, Scale& t) {
	int16_t x;
	rdcast (s, x);
	t = static_cast<Scale>(x);
}

class Decimal {
private:
	int64_t value;
	Scale scale;
public:
	Decimal() : value(0), scale(One) {}
	Decimal(int64_t value) : value(value), scale(One) {}
	Decimal(int64_t value, Scale scale) : value(value), scale(scale) {}
};

template <typename C, typename E>
bool contains(const C& c, const E& e) {
	return std::find(c.cbegin(), c.cend(), e) != c.cend();
}

class svtok {
private:
	std::string_view s;
	size_t i;
	char sep;
public:
	svtok(const std::string_view& s, char sep = ',') : s(s), i(0), sep(sep) {}

	bool hasNext() {
		return i < s.size();
	}

	std::string_view next() {
		size_t j = s.find(sep, i);
		auto r = s.substr(i, j - i);
		i = j + 1;
		return r;
	}
};


// Datetime representation
class clsdate {
	time_t unixTime;
public:

	clsdate() : unixTime(0) {}
	clsdate(const clsdate& d) = default;
	clsdate(time_t time) : unixTime(time) {}
	constexpr time_t ct() const { return unixTime; }	
	operator time_t () { return unixTime; }

	void FromRd(std::string s) {
		rdcast(s, unixTime);
	}

	std::string ToRd() const { return to_rdstr(unixTime); 	}
	friend bool operator <(const clsdate& d1, const clsdate& d2);
	friend bool operator <=(const clsdate& d1, const clsdate& d2);
};

template <>
std::string to_rdstr(const clsdate& t);

template <>
inline void rdcast(const std::string_view& s, clsdate& t) {
	tm tm = {};
	std::stringstream ss ( s.data() );
	switch (s.size() ){
		case 1: [[fallthrough]];
		case 2: [[fallthrough]];
		case 3: [[fallthrough]];
		case 4: [[fallthrough]];
		case 5: [[fallthrough]];
		case 6: [[fallthrough]];
 		case 7: [[fallthrough]];
		case 8: [[fallthrough]];
 		case 9: [[fallthrough]];
		[[likely]] case 10:
		{
			time_t tt;
			rdcast(s, tt);
			t = tt;
			return;
		}
		case 14:
			ss >> std::get_time(&tm, "%Y%m%d%H%M%S");
			break;
		case 19:
			ss >> std::get_time(&tm, "%Y-%m-%d  %H:%M:%S");
			break;
		default:
			throw std::invalid_argument(fmt::format( "clsdate::rdcast wrong date len:{} and value:<{}>", s.size(), s ) );
	}
	std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
	auto ts = std::chrono::duration_cast<std::chrono::seconds>( tp.time_since_epoch() );
	auto fin = ts + std::chrono::current_zone()->get_info(tp).offset; 
	t = fin.count();
}

inline bool operator <(const clsdate& d1, const clsdate& d2) {
	return d1.unixTime < d2.unixTime;
}
inline bool operator <=(const clsdate& d1, const clsdate& d2) {
	return d1.unixTime <= d2.unixTime;
}
inline std::ostream& operator << (std::ostream& s, const clsdate& d) {
	return s << d.ToRd();
}

enum PeriodType {
	V15 = 15,
};

template <class T>
inline std::string toSS(T const& t){
	if (t.empty()) return "";
	std::stringstream ret;
	for (auto x: t){
		ret << x << ", ";
	}
	return std::move(ret.str());
}


#endif // !__TYPES_H__