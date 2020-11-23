# aprs2rrd-se
A piece of software to fetch, parse and store data from APRS weather stations into a RRD &amp; MySQL database and on generated HTML website. Currently software support communication with APRS-IS servers using standard text-based protocol. It support any APRS and CWOP wheather station, until it send measuremenet data in APRS wx frames. The program uses RRD round robin database as a main storage for data gathered from APRS-IS, but an user need to generate target RRD files manually basing individul needs (mostly intervals between wx frames). Optionally storing data in MySQL could be enabled. For further information please refer to documentation and example configuration file which consist comments describing most of options

The most recent release version of APRS2RRD is 1.3, which should be coosen from git repository using command 'git checkout 1.3'. Please be aware that master branch is used to daily development and 'mainline' source coud even not compile in certain situations. 

APRS2RRD is intended to be used in Linux environment and it is developed and tested using this operating system. APRS2RRD uses Boost C++ library instead of platform specific calls hence compilation under Windows environment should be theoretically possible, unless apropriate binary version of Boost C++ will be avaliable. 

System requiremenets:
1) g++ and make 
2) libboost and libboost-dev, version 1.58 or newer (pakages: asio, thread, date-time, regex, system)
3) libconfig++ and libconfig++-dev, version 1.5
4) libmysql++ and libmysql++-dev, version 3.2.2
5) libxerces-c and libxerces-c-dev
6) libcurl and libcurl-dev
7) screen
8) rrdtool

If You have linking errors like that

	/usr/bin/ld: skipping incompatible /usr/lib/i386-linux-gnu/libgcc_s.so.1 when searching for libgcc_s.so.1
	/usr/bin/ld: skipping incompatible /usr/lib/i386-linux-gnu/libgcc_s.so.1 when searching for libgcc_s.so.1

You should install libc6-dev-i386


Mateusz Lubecki
email: sp8ebc@interia.pl

Bielsko-Biała 2019, 2020
