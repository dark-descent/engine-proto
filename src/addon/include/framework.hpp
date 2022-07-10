#pragma once

#include <node.h>

#include <array>
#include <map>
#include <unordered_map>
#include <vector>
#include <stack>
#include <utility>
#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <stdexcept>
#include <filesystem>
#include <fstream>

#include "v8_helpers.hpp"

#ifdef __GNUC__
#define PACK(__Declaration__) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK(__Declaration__) __pragma(pack(push, 1)) __Declaration__ __pragma(pack(pop))
#endif