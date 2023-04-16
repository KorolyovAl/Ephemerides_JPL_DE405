<a id="start"></a>
A program for calculating the coordinates and velocities of the planets of the solar system from NASA ephemeris JPL [DE405](http://ssd.jpl.nasa.gov/ftp/eph/planets/ascii/de405). All coordinates are calculated relative to the barycenter of the solar system.\
For the program to work, ephemeris files should be placed in the files folder at the root of the project (the path to the files is set in initialization.cpp and reading_ascp.cpp ). The results are saved to the results folder (if there is no such folder, it is necessary to create it).

---

[Initial data](#ch1)\
[Ephemeris File Format](#ch2)\
[Calculation of coordinates and velocities](#ch3)\
[Results](#ch4)

---
<a id="ch1"></a>
## Initial data  

The initial data is set in the file "files/initial_data.txt ".

Based on the file "header.405" a list of objects for which coordinates are calculated is formed in the file "files/initial_data.txt ". A true/false flag is written in front of each object, which indicates whether coordinates will be calculated for this object.

Next, the initial data sets the date range in which the coordinates will be calculated. The date range is formed based on the presence of ephemeris files. Each file contains data for a 20-year period of time.

[up](#start)

---
<a id="ch2"></a>
## Ephemeris file format  

_1. Header file header.405_

The header.405 file is divided into several blocks, which are marked with the keyword "GROUP".

"GROUP 1030" contains 3 numbers: the date of the beginning and end of the epoch in Julian format and the duration in days for each block of coefficients in ascp files.

Some additional parameters for objects are given in "GROUP 1040", and their numerical values are recorded in "GROUP 1041".

The "GROUP 1050" contains such a group of numbers:

     3   171   231   309   342   366   387   405   423   441   753   819   899
    14    10    13    11     8     7     6     6     6    13    11    10    10
     4     2     2     1     1     1     1     1     1     8     2     4     4

In this group, the numbers are divided into vertical triples for each body, respectively.

Consider an example:\
The first vertical triple of numbers (3, 14, 4) is intended for Mercury. The coefficients for Mercury start from the 3rd coefficient in the block (and up to 170), there are 14 coefficients for each coordinate, and the block interval of 32 days is divided into 4 subintervals, that is, for each 8-day
subinterval there is a set of coefficients.\
Accordingly, the coefficient numbers for Venus in the block are from 171 to 230, 10 coefficients per coordinate, 2 subintervals (each subinterval = 16 days).\
Etc.\
Coefficients for bodies are arranged in the following order:\
Mercury, Venus, Earth-Moon barycenter, Mars , Jupiter , Saturn, Uranus, Neptune, Pluto, Moon (geocentric), Sun.

_2. ascp coefficient files_

For example, the ascp2000.405 file consists of blocks of 341 lines each. The block starts with a line like:

1   1018 

Where the first number is the block number, the second number is the number of coefficients in the block (unchanged).\
Each block contains a number of coefficients for approximating the equatorial rectangular coordinates (XYZ) of the planets of the Solar System within a 32-day interval. \
The block consists of 340 lines of 3 coefficients per line, a total of 1020 numbers, but the first two are not used.\
The ascp2000.405 file does not start from January 1, 2000, as one might think, but from 12/24/1999 0:00 UTC.

[up](#start)

---
<a id="ch3"></a>
## Calculation of coordinates and velocities

Algorithm for determining the coordinates of the body:\
Calculate the number of the desired block:\
Let\
MD0 = MJD(1999,12,24) – the initial Julian date of the ascp2000.405 file  
MD – the desired date for which the coordinates must be calculated.\
Interval = 32 (the length of the interval in days during which a separate block of coefficients is valid) is set in the header file.405 in "GROUP 1030".  
Block number:\
NumerBlock =  Int((MD - md0) / Interval) + 1\
The start date of the required block:\
MD1 = MD0 + (NumerBlock - 1) * Interval

We determine the length and number of the subinterval:\
subInterval = Interval / Nset_Body - the length of the subinterval in days\
where Nset_Body is the number of subintervals for a specific body (the third line of the 1050 group in header.405)\
The number of full subintervals in the block before the desired date:\
podint = Int((MD - MD1) / subInterval)  
The start date of the desired subinterval:\
Mdat = MD1 + podint * subInterval\
Shift of the beginning of reading coefficients taking into account the subinterval:\
IndBegin = 3 * Nkoef_Body * podint\
Where Nkoef_Body is the number of coefficients for this body (the second line of the 1050 group in header.405).

For example, let's define the barycentric coordinates of Mars on 04/14/2023 0:00 UTC:\
MD0 = 58832.5\
MD = 60049\
NumerBlock = 39\
MD1 = 60048,5 - (the 39th block starts from this date)\
subinterval = 32 - (for Mars Nset_Body = 1)\
podint = 0 - (so many subintervals need to be skipped)\
Mdat = 60048,5 - (the coefficients of the 1st subinterval, which starts from this date, are needed)\
From the 39th block, we read the coefficients from 309 to 341 (the position of the coefficients for Mars, the first line of the group header in 1050.405)\
IndBegin = 0

Since IndBegin = 0, we read the coefficients from the very beginning: kX(from 1 to 11) for X, kY(from 1 to 11) for Y, kZ(from 1 to 11) for Z.

We received three sets of numbers of 11 pieces in a set. Now it's up to Chebyshev polynomials.

The argument in Chebyshev polynomials is the normalized time (from -1 to 1) inside the subinterval:\
tau = 2 * (MD - Mdat) / subInterval - 1\
For the example in question, tau = -0.96875

There are 11 coefficients (for Mars), 11 Chebyshev polynomials are needed.\
p(1) = 1\
p(2) = tau\
p(i) = 2 * tau*p(i - 1) - p(i - 2)\
And at the same time 11 derivatives of Chebyshev polynomials: \
pt(1) = 0\
pt(2) = 1\
pt(i) = 2 * (p(i - 1) + tau * pt(i - 1)) - pt(i - 2)

Calculate coordinates and speeds:\
X = kX(1)*p(1) + … + kX(11)*p(11)\
X’ = kX(1)*pt(1) +…  + kX(11)*pt(11)\
Y =  kY(1)*p(1) + … + kY(11)*p(11)

As a result, we have X,Y,Z barycentric in kilometers. To switch to the heliocentric coordinate system, you need to similarly calculate the barycentric
vector of the Sun and take the difference of the vectors. To switch to the ecliptic coordinate system, the vector must be expanded accordingly. For the moon , the vector is calculated 
geocentric.

The calculated velocity vector must be divided by the coefficient koefVelocity = subInterval / 2 * 86400 (we get the number of seconds in half of the subinterval) to get the dimension of km/sec.

For the example in question:\
Barycentric position vector (-178770124.524, 155018292.718, 75929096.804)\
Barycentric velocity vector ( -15.98, -14.08, -6.03)  

[up](#start)

---
<a id="ch4"></a>
## Results  

The results are saved to the results directory. A separate file is formed for each body, in which dates in Julian and Gregorian format, barycentric coordinates of the body and its velocity are recorded.

Example:\
julian_date = 2.42961650e+06 gregorian_date = 18.12.1939 X = 1.797734653534874618e+08   Y = 1.073376592823612988e+08   Z = 4.436277296654191613e+07   R = 2.140278663415088654e+08   VX = -1.219732e+01   VY = 2.025101e+01    VZ = 9.619936e+00

[up](#start)

---