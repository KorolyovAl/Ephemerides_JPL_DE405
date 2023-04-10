#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <cmath>
#include <iomanip>
#include <thread>
#include <future>

#include "structures.h"
#include "functions.h"

void CalculateSingleThread(Ephemerides& eph,int i);

void Run_calculating_coordinates(Ephemerides& eph);