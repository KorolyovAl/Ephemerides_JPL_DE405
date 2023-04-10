#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <iomanip>
#include <thread>
#include <chrono>

#include "structures.h"
#include "functions.h"
#include "initialization.h"
#include "reading_ascp.h"
#include "calculating_coordinates.h"

using namespace std::chrono;

/*
	Функция запуска программы чтения эфемерид (пока только эфемериды DE_405).
*/
void Ephemerides_start();