#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include "structures.h"

void Read_init_data(Ephemerides& tmp);
void Read_header_ascp(Ephemerides& tmp);
void Calculate_number_of_elements(Ephemerides& eph);
void Initialization(Ephemerides& eph);