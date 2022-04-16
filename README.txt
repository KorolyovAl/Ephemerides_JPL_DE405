Файл ascp2000.405 состоит из блоков по 341 строк в каждом. Блок начинается со строки вида:

Цитата
1   1018 
                                                                 
Где первое число – номер блока, второе число – количество коэффициентов в блоке (неизменно).
Каждый блок содержит ряд коэффициентов для аппроксимации экваториальных прямоугольных координат (XYZ) планет Солнечной Системы  внутри 32-дневного интервала. 
Блок состоит из 340 строк по 3 коэффициента в строке, итого 1020 волшебных чисел, но два первых не используются.
Файл ascp2000.405 начинается не с 1 января 2000 г, как можно было бы подумать, а с 24.12.1999 0.0 UT.
Файл header.405 содержит в себе такую важную группу чисел:

Цитата
GROUP   1050
     3   171   231   309   342   366   387   405   423   441   753   819   899
    14    10    13    11     8     7     6     6     6    13    11    10    10
     4     2     2     1     1     1     1     1     1     8     2     4     4
     
Первая вертикальная тройка чисел (3, 14, 4) предназначается для Меркурия. Коэффициенты для Меркурия начинаются с 3-го коэффициента в блоке (и до 170, 
как можно догадаться), всего коэффициентов для каждой координаты 14, а блочный интервал  в 32 дня разбит на 4 подинтервала, то есть для каждого 8-суточного 
подинтервала свой набор коэффициентов.
Соответственно, номера коэффициентов для Венеры в блоке - с 171 по 230, по 10 коэффициентов на координату, подинтервалов 2 (каждый подинтервал = 16 дней).
И т.д.
Коэффициенты для тел располагаются в следующем порядке:
 Mercury,  Venus,  Earth-Moon barycenter,  Mars ,  Jupiter ,  Saturn,  Uranus,  Neptune,  Pluto,  Moon (geocentric),  Sun.
 
 
Очевидно, что первым делом нужно вычислить номер нужного блока:
Пусть
MD0 = MJD(1999,12,24) – начальная юлианская дата файла ascp2000.405
MD – нужная дата, на которую необходимо вычислить эфемериды.
Interval = 32 (длина интервала в сутках, во время которого действителен отдельный блок коэффициентов), константа
Номер блока:
NumerBlock=  Int((MD - md0) / Interval) + 1
Начальная дата нужного блока:
MD1 = MD0 + (NumerBlock - 1) * Interval

Затем возникает необходимость вычислить длину и номер подинтервала:
subInterval = Interval / Nset_Body     длина подинтервала в сутках
где Nset_Body – количество подинтервалов для конкретного тела (третья строка группы 1050 в header.405)
Количество полных подинтервалов в блоке до нужной даты:
podint = Int((MD - MD1) / subInterval) 
Начальная дата нужного подинтервала:
Mdat = MD1 + podint * subInterval
Сдвиг начала чтения коэффициентов с учетом подинтервала:
IndBegin = 3 * Nkoef_Body * podint
Где Nkoef_Body – количество коэффициентов для данного тела (вторая строка группы 1050 в header.405).

Для примера возьмем Землю и захотим вычислить ее барицентрические координаты на 03.01. 2016  5:30 UT.
MD0 =  51536
MD =  57390.2299558333 (это заданная дата + 32.184 сек + TAI_UTC )
NumerBlock  =  183
MD1 =  57360  (183-ий блок начинается с этой даты)
subinterval  =   16 (для Земли Nset_Body = 2)
podint  =  1 (столько подинтервалов нужно пропустить)
Mdat  =   57376 (нужны коэффициенты 2-го подинтервала, который начинается с этой даты)
Из 183-го блока читаем коэффициенты с 231 по 308 (положение коэффициентов для Земли, первая строка группы 1050 в header.405), всего их 78, 
они принадлежат двум подинтервалам по 16 дней каждый. Нам нужны коэффициенты второго подинтервала.
IndBegin  =    39
Первые 39 коэффициентов пропускаем, и читаем оставшиеся:  kX(от 1 до 13) для X, kY(от 1 до 13) для Y, kZ(от 1 до 13) для Z.

Получили три комплекта чисел по 13 штук в комплекте. Теперь дело за полиномами Чебышева.

Аргументом в полиномах Чебышева является нормированное время (от -1 до 1) внутри подинтервала:
tau = 2 * (MD - Mdat) / subInterval - 1
Для рассматриваемого примера  tau=  0.778744479166562

Коэффициентов 13 (для Земли), нужно 13 полиномов Чебышева.
p(1) = 1
 p(2) = tau
 p(i) = 2 * tau * p(i - 1) - p(i - 2)
И заодно 13 производных от полиномов Чебышева:
pt(1) = 0
 pt(2) = 1
 pt(i) = 2 * (p(i - 1) + tau * pt(i - 1)) - pt(i - 2)

Вычисляем координаты и скорости:
X = kX(1)*p(1) + … + kX(13)*p(13)
X’ = kX(1)*pt(1) +…  + kX(13)*pt(13)
Y =  kY(1)*p(1) + … + kY(13)*p(13)
…
В результате мы имеем X,Y,Z барицентрические  в километрах. Для перехода к гелиоцентрической системе координат нужно аналогично вычислить барицентрический 
вектор Солнца и взять разницу векторов. Для перехода в эклиптическую систему координату вектор необходимо соответственно развернуть. Для Луны вектор вычисляется 
геоцентрический.

Вычисленный вектор скорости нужно разделить на коэффициент koefVelocity = subInterval / 2 * 86400 (количество секунд в половине подинтервала), чтобы получить 
размерность км/сек.

Для рассматриваемого примера:
Барицентрический вектор положения  (-30121319.6,  132198969.6,  57283461.0)
Барицентрический вектор скорости ( -29.617,  -5.792,  -2.511)
Гелиоцентрический вектор положения (-30680782.2,  131994773.2,  57221236.6)
Гелиоцентрический вектор скорости  (-29.619,  -5.803,  -2.516)


------------------------------------------------------------------------------------------------------------------------------------------------------------

INTERNAL FORMAT OF THE EPHEMERIS FILES
--------------------------------------

On the first record of an export binary file or in the "GROUP 1050 of the ascii
"header", there are 3 sets of 15 integers each.  (Older versions have only the first 13
integers in each set)

The 15 triplets give information about the location, order and time-coverage of
the chebychev polynomials corresponding to the following 15 items:

       Mercury
       Venus
       Earth-Moon barycenter
       Mars 
       Jupiter 
       Saturn
       Uranus
       Neptune
       Pluto
       Moon (geocentric)
       Sun
       Earth Nutations in longitude and obliquity (IAU 1980 model)
       Lunar mantle libration
       Lunar mantle angular velocity
       TT-TDB (at geocenter)

Word (1,i) is the starting location in each data record of the chebychev 
coefficients belonging to the ith item.  Word (2,i) is the number of chebychev 
coefficients per component of the ith item, and Word (3,i) is the number of 
complete sets of coefficients in each data record for the ith item.

Items not stored on the ascii files have 0 coefficients [Word (3,i)].

Data Records ("GROUP 1070")

These records contain the actual ephemeris data in the form of chebychev 
polynomials.

The first two double precision words in each data record contain

         Julian date of earliest data in record.
         Julian date of latest data in record.

The remaining data are chebychev position coefficients for each component of 
each body on the tape.  The chebychev coefficients for the planets represent 
the solar system barycentric positions of the centers of the planetary systems.

There are three Cartesian components (x, y, z), for each of the items #1-11; 
there are two components for the 12th item, nutations : d(psi) and d(epsilon);
there are three components for the 13th item, librations : phi, theta, psi;
there are three components for the 14th item, mantle omega_x,omega_y,omega_z;
there is one component for the 17th item, TT-TDB.

Planetary positions are stored in units of kilometers (TDB-compatible).
The nutations and librations are stored in units of radians.
The mantle angular velocities are stored in radians/day.
TT-TDB is stored in seconds.


