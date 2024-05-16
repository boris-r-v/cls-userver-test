#include <string>
#include "types.h"

#include <charconv>

template <>
void rdcast(const std::string_view& s, bool& t) {
	if (s.empty() || s == "0")
		t = false;
	else if (s == "1")
		t = true;
	else
		throw std::runtime_error("Invalid bool value");
}

template <>
void rdcast(const std::string_view& s, int16_t& t) {
	std::from_chars_result r = std::from_chars(s.data(), s.data() + s.size(), t);
	if (r.ec != std::errc{})
		throw std::invalid_argument("Invalid int16 conversation");
	//t = std::stol(s);//TODO
}

template <>
void rdcast(const std::string_view& s, uint16_t& t) {
	std::from_chars_result r = std::from_chars(s.data(), s.data() + s.size(), t);
	if (r.ec != std::errc{})
		throw std::invalid_argument("Invalid uint16 conversation");
	//t = std::stoul(s);//TODO
}

template <>
void rdcast(const std::string_view& s, int32_t& t) {
	std::from_chars_result r = std::from_chars(s.data(), s.data() + s.size(), t);
	if (r.ec != std::errc{})
		throw std::invalid_argument("Invalid int32 conversation");
	//t = std::stol(s);
}

template <>
void rdcast(const std::string_view& s, uint32_t& t) {
	std::from_chars_result r = std::from_chars(s.data(), s.data() + s.size(), t);
	if (r.ec != std::errc{})
		throw std::invalid_argument("Invalid uint32 conversation");
	//t = std::stoul(s);
}

template <>
void rdcast(const std::string_view& s, int64_t& t) {
	std::from_chars_result r = std::from_chars(s.data(), s.data() + s.size(), t);
	if (r.ec != std::errc{})
		throw std::invalid_argument("Invalid int64 conversation");
	//t = std::stoll(s);
}

template <>
void rdcast(const std::string_view& s, uint64_t& t) {
	std::from_chars_result r = std::from_chars(s.data(), s.data() + s.size(), t);
	if (r.ec != std::errc{})
		throw std::invalid_argument("Invalid uint64 conversation");
	//t = std::stoull(s);
}

template <>
void rdcast(const std::string_view& s, cls_gen::RecalculationPeriodType& t){
	int16_t x;
	rdcast (s, x);
	t = static_cast<cls_gen::RecalculationPeriodType>(x);
}

template <>
void rdcast(const std::string_view& s, cls_gen::CounterSearchFlags& t){
	int16_t x;
	rdcast (s, x);
	t = static_cast<cls_gen::CounterSearchFlags>(x);
}

template <>
std::string to_rdstr(const clsdate& t) {
	return t.ToRd();
}

template <>
std::string to_rdstrv(const std::vector<uint32_t>& v) {
	std::stringstream ss;
	for (auto it = v.cbegin(); it != v.cend(); ++it) {
		ss << *it;
		ss << ';';
	}
	return ss.str();
}
