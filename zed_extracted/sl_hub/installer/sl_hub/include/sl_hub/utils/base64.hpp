#include <string>
#include <vector>
#pragma once
namespace sliot{
std::string base64_encode(unsigned char const*, unsigned int len);
std::vector<unsigned char> base64_decode(std::string const& encoded_string);
}
