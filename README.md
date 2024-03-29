# aprs2rrd-se
A piece of software to fetch, parse and store data from APRS weather stations into RRD files, MySQL database and on generated HTML website. Currently the software supports communication with APRS-IS servers using standard text-based protocol (it can monitor two different stations at once), communication directly with the APRS radio network using KISS TNC modem connected to the serial port and communication with the Holfuy cloud API to download data collected by stations manufactured by this company. 

Value of each monitored parameter could come from different source, so for instance temperature could be fetched from primary APRS-IS station, pressure could come from secondary APRS-IS station and the rest could be collected from Holfuy cloud API. Data processing chain involves such steps:

1. Estabilisihing persistent connection to an APRS-IS server and receiving data sent by configured station
2. Opening RS232 serial port and receiving KISS protocol data from connected TNC.
3. Downloading data from Holfuy cloud API if it is enabled.
4. Downloading data from Davis Weatherlink API if it is enabled.
5. Parsing APRX rf-log file.
6. Merging data from differenct sources into one dataset.
7. Applying the slew limitation to rub out any peaking measurement noise.
8. Applying the wind direction correction if it is enabled, to remove known offset coming from wind sensor misalignment. 
9. Calculating differences for the same parameters between different data sources according to a user configuration.
10. Storing results in RRD files and optionally in MySQL database.
11. Generating set of plots from RRD files with a simple HTML file.
12. Generating PNG image with graphical representation of windspeed and direction (a wind rose with an arrow pointing winddir + big numeric value of average windspeed)

The most recent release version of APRS2RRD is 1.9.1, which should be coosen from git repository using command 'git checkout 1.9.1'. This is valid as for December 20th 2023 and of course this readme may not be updated each release. Please be aware that master branch is used to daily development and 'mainline' source coud even not compile in certain situations. Just use the latest tag available and it will work fine :)

APRS2RRD can be used standalone, but actually it is a part of bigger weather stations system which is formed by:
1. API: https://github.com/SP8EBC/meteo_backend
2. Firmware for APRS weather station controller and it's design: https://github.com/SP8EBC/ParaTNC
3. Android application: https://github.com/SP8EBC/MeteoSystem

APRS2RRD is intended to be used in Linux environment and it is developed and tested using this operating system. APRS2RRD uses Boost C++ library instead of platform specific calls hence compilation under Windows environment should be theoretically possible, unless apropriate binary version of Boost C++ will be avaliable. 

System requiremenets (plus all dependiences of those):
1) g++ and make 
2) libboost and libboost-dev, version 1.58 or newer (pakages: filesystem, asio, thread, date-time, regex, system)
3) libconfig++ and libconfig++-dev, version 1.5 or newer
4) libmysql++ and libmysql++-dev, version 3.2.2 or newer
5) libxerces-c and libxerces-c-dev 3.2
6) libcurl and libcurl-dev
7) libspdlog1 and libspdlog1-fmt8 and libspdlog-dev
8) imagemagick and imagemagick-6-common and imagemagick-6.q16
9) screen
10) rrdtool

Currently old distros, Debian 9 or Ubuntu 16.04/16.10 and anything older is not supported. 'Release' and 'Debug' configurations are not actively maintained and will be removed in near future. Please use Ubuntu 20.xx / Debian 10 or newer (with xerces-c in version 9.2) and build APRS2RRD using Debug_xerces_3_2 or Release_xerces_3_2 configuration. There is also working Visual Studio Code workspace and configuration in this repo. You can open it in VS code and develop it out of the box (assuming all dependiences are installed)

Information about installing in Debian 10:
If You want to upgrade Your Debian 9 installation, or install APRS2RRD in fresh and current installation You will very quickly see that libmysql++ is no longer avaliable in the Debian 10. Thankfully this isn't something which don't have any workaround, but You need to install libmysql++ manually. First at all visit https://tangentsoft.com/mysqlpp/home and download the tarball containing all sources needed like this one: https://tangentsoft.com/mysqlpp/releases/mysql++-3.2.5.tar.gz. 

Then check if You have all libmysql++ dependiences installed. Use aptitude and check 'aptitude show default-libmysqlclient-dev' if this metapackage is in the system and install it if not. Then decompress the tarball and configure sources with './configure --prefix=/usr' When sources are configured w/o errors just type standard make and then make install. If everything is OK You can proceed to APRS2RRD.

Actually the APRS2RRD cannot run in background as a daemon so 'screen' command must be used to start it in background. Please look into 'doc' directory where example rc.aprs2rrd and rc.skrzyczne startup scripts are placed, along with example configuration files. It is strongly advised to run APRS2RRD as a separate non-root user with minial privileges required. Please remember that APRS2RRD executable must have a read-write access to all RRD files and directory where they are stored and also to directory where it will generate HTML and PNG files with plots. PS: Yes, I know what systemd is and how to create new systemd units

APRX rf-log parser implementation has been ported from ParaGADACZ software (https://github.com/SP8EBC/ParaGADACZ) although some small changes had to be made
before it compiled and worked correctly. Classes and methods are still named the same and do almost the same, admittedly the implementation itself is 
sligtly different. As a result parser code is not portable across ParaGADACZ and APRS2RRD

This APRS2RRD incarnation is an evolution of previous codebase, which goes back to 2015 and even 2013 year. If You are brave enough look on the source code
located in './doc/prehistory-please-dont-look-here/' but please be warned. Don't go there if You don't like blood coming from Your eyes, or You are not 
sadistic reviewer taking pleasure from bitching about somebody shamless past.  

Mateusz Lubecki
email: sp8ebc@interia.pl

Bielsko-Biała 2018, 2019, 2020, 2021, 2022, 2023
