// Copyright 2022 Daniel Liu

#ifndef _CPPSERVER_URL_H_
#define _CPPSERVER_URL_H_

#include <regex>
#include <string>

namespace cppserver {

std::string QuoteUrl(const std::string& str_in);

std::string UnquoteUrl(const std::string& str_in);

}

#endif
