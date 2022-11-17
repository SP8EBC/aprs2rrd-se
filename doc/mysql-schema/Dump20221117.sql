-- MySQL dump 10.13  Distrib 8.0.31, for Linux (x86_64)
--
-- Host: localhost    Database: aprs2rrd
-- ------------------------------------------------------
-- Server version	8.0.31-0ubuntu0.20.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `data_diff`
--

DROP TABLE IF EXISTS `data_diff`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `data_diff` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `epoch` bigint NOT NULL,
  `datetime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `station` varchar(32) NOT NULL,
  `temperaturefrom` varchar(10) NOT NULL,
  `temperaturesubtract` varchar(10) NOT NULL,
  `windfrom` varchar(10) NOT NULL,
  `windsubtract` varchar(10) NOT NULL,
  `temperature` float NOT NULL,
  `winddir` smallint NOT NULL,
  `windspeed` float NOT NULL,
  `windgusts` float NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=319158 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `data_locale`
--

DROP TABLE IF EXISTS `data_locale`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `data_locale` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `stationGroupId` int unsigned NOT NULL,
  `language` varchar(45) COLLATE utf8mb4_general_ci NOT NULL,
  `text` varchar(64) COLLATE utf8mb4_general_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `data_parameteo_measurements`
--

DROP TABLE IF EXISTS `data_parameteo_measurements`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `data_parameteo_measurements` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `epoch` bigint NOT NULL,
  `datetime` datetime DEFAULT CURRENT_TIMESTAMP,
  `stationname` text NOT NULL,
  `temperaturedallas` smallint NOT NULL,
  `temperaturept` smallint NOT NULL,
  `temperatureinternal` smallint NOT NULL,
  `temperaturemodbus` smallint NOT NULL,
  `pressure` smallint NOT NULL,
  `humidity` smallint NOT NULL,
  `winddirection` smallint NOT NULL,
  `windaverage` smallint NOT NULL,
  `windgust` smallint NOT NULL,
  `windcurrent` smallint NOT NULL,
  `windminimal` smallint NOT NULL,
  `pm2a5` smallint NOT NULL,
  `pm10` smallint NOT NULL,
  `ssid` tinyint DEFAULT '0',
  `mastertime` int NOT NULL,
  `callsign` text,
  `apimac` text NOT NULL,
  PRIMARY KEY (`id`),
  KEY `epoch_idx` (`epoch`)
) ENGINE=InnoDB AUTO_INCREMENT=103 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `data_parameteo_status`
--

DROP TABLE IF EXISTS `data_parameteo_status`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `data_parameteo_status` (
  `id` int NOT NULL AUTO_INCREMENT,
  `station` text NOT NULL,
  `timestamp` bigint NOT NULL,
  `mainconfigdatabasiccallsign` text NOT NULL,
  `mainconfigdatabasicssid` int NOT NULL,
  `mastertime` bigint NOT NULL,
  `maincpuload` int NOT NULL,
  `rx10m` int NOT NULL,
  `tx10m` int NOT NULL,
  `digi10m` int NOT NULL,
  `digidrop10m` int NOT NULL,
  `kiss10m` int NOT NULL,
  `rxtotal` int NOT NULL,
  `txtotal` int NOT NULL,
  `rtemainaveragebatteryvoltage` int NOT NULL,
  `rtemainwakeupcount` int NOT NULL,
  `rtemaingoingsleepcount` int NOT NULL,
  `rtemainlastsleepmastertime` int NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `data_station`
--

DROP TABLE IF EXISTS `data_station`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `data_station` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `epoch` bigint NOT NULL,
  `datetime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP,
  `station` varchar(32) NOT NULL,
  `temperature` float NOT NULL,
  `humidity` tinyint unsigned NOT NULL,
  `pressure` float NOT NULL,
  `winddir` smallint unsigned NOT NULL,
  `windspeed` float NOT NULL,
  `windgusts` float NOT NULL,
  `tsource` varchar(10) NOT NULL,
  `wsource` varchar(10) NOT NULL,
  `psource` varchar(10) NOT NULL,
  `hsource` varchar(10) NOT NULL,
  `rsource` varchar(10) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `epoch_idx` (`epoch`),
  KEY `station_idx` (`station`)
) ENGINE=InnoDB AUTO_INCREMENT=3610085 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`mateusz`@`localhost`*/ /*!50003 TRIGGER `data_station_AFTER_INSERT` AFTER INSERT ON `data_station` FOR EACH ROW BEGIN
CALL aprs2rrd.calculate_summary(NEW.station);
END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `data_station_archive`
--

DROP TABLE IF EXISTS `data_station_archive`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `data_station_archive` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `epoch` bigint NOT NULL,
  `datetime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP,
  `station` varchar(32) NOT NULL,
  `temperature` float NOT NULL,
  `humidity` tinyint unsigned NOT NULL,
  `pressure` float NOT NULL,
  `winddir` smallint unsigned NOT NULL,
  `windspeed` float NOT NULL,
  `windgusts` float NOT NULL,
  `tsource` varchar(10) NOT NULL,
  `wsource` varchar(10) NOT NULL,
  `psource` varchar(10) NOT NULL,
  `hsource` varchar(10) NOT NULL,
  `rsource` varchar(10) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `data_station_def`
--

DROP TABLE IF EXISTS `data_station_def`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `data_station_def` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(32) NOT NULL,
  `enabled` tinyint(1) NOT NULL DEFAULT '1',
  `callsign` varchar(6) NOT NULL,
  `ssid` tinyint NOT NULL,
  `displayedname` varchar(32) NOT NULL,
  `displayedlocation` varchar(48) NOT NULL,
  `sponsorurl` varchar(48) DEFAULT NULL,
  `backgroundjpg` varchar(64) DEFAULT NULL,
  `backgroundjpgalign` tinyint NOT NULL DEFAULT '0',
  `stationnametextcolour` int NOT NULL DEFAULT '-16777216',
  `moreinfo` varchar(255) DEFAULT NULL,
  `lat` float NOT NULL,
  `lon` float NOT NULL,
  `timezone` varchar(64) NOT NULL,
  `haswind` tinyint(1) NOT NULL,
  `hastemperature` tinyint(1) NOT NULL,
  `hasqnh` tinyint(1) NOT NULL,
  `hashumidity` tinyint(1) NOT NULL,
  `hasrain` tinyint(1) NOT NULL,
  `telemetryversion` tinyint NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=17 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `data_station_groups`
--

DROP TABLE IF EXISTS `data_station_groups`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `data_station_groups` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `category` int NOT NULL,
  `enabled` tinyint NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `data_station_groups_bondings`
--

DROP TABLE IF EXISTS `data_station_groups_bondings`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `data_station_groups_bondings` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `stationid` int NOT NULL,
  `groupid` int NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `data_summary`
--

DROP TABLE IF EXISTS `data_summary`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `data_summary` (
  `id` int NOT NULL AUTO_INCREMENT,
  `lasttimestamp` int DEFAULT NULL,
  `numberofmeasurements` int DEFAULT NULL,
  `temperature` float DEFAULT NULL,
  `qnh` int DEFAULT NULL,
  `humidity` int DEFAULT NULL,
  `direction` int DEFAULT NULL,
  `average` float DEFAULT NULL,
  `gusts` float DEFAULT NULL,
  `hourgusts` float DEFAULT NULL,
  `hourmaxavg` float DEFAULT NULL,
  `hourminavg` float DEFAULT NULL,
  `station` varchar(45) COLLATE utf8mb4_general_ci DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `station_name` (`station`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `data_telemetry`
--

DROP TABLE IF EXISTS `data_telemetry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `data_telemetry` (
  `id` int NOT NULL AUTO_INCREMENT,
  `epoch` bigint NOT NULL,
  `datetime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `station` varchar(48) NOT NULL,
  `callsign` varchar(32) NOT NULL,
  `number` smallint unsigned NOT NULL,
  `first` float NOT NULL,
  `second` float NOT NULL,
  `third` float NOT NULL,
  `fourth` float NOT NULL,
  `fifth` float NOT NULL,
  `digital` smallint NOT NULL,
  `rawfirst` tinyint unsigned NOT NULL,
  `rawsecond` tinyint unsigned NOT NULL,
  `rawthird` tinyint unsigned NOT NULL,
  `rawfourth` tinyint unsigned NOT NULL,
  `rawfifth` tinyint unsigned NOT NULL,
  `frame` varchar(90) NOT NULL DEFAULT 'none',
  `type` tinyint NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=828230 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping events for database 'aprs2rrd'
--

--
-- Dumping routines for database 'aprs2rrd'
--
/*!50003 DROP PROCEDURE IF EXISTS `calculate_summary` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`mateusz`@`localhost` PROCEDURE `calculate_summary`(IN stationname text)
BEGIN
	DECLARE last_temperature, avg_windspeed, avg_temperature, gusts, hour_gusts, min_avg_windspeed, max_avg_windspeed FLOAT;
    DECLARE measurements_num, last_timestamp, last_qnh INT;
    DECLARE last_humidity TINYINT UNSIGNED;
    DECLARE last_direction SMALLINT UNSIGNED;
    
SELECT 
    `humidity`,
    `windspeed`,
    `windgusts`,
    `temperature`,
    `epoch`,
    `pressure`,
    `winddir`
INTO last_humidity , avg_windspeed , gusts , last_temperature , last_timestamp , last_qnh , last_direction FROM
    `data_station`
WHERE
    `station` COLLATE utf8mb4_general_ci = stationname
        AND `datetime` > SUBTIME(NOW(), '14:00:00.00')
ORDER BY `id` DESC
LIMIT 1;
    
SELECT 
    MAX(`windgusts`),
    MAX(`windspeed`),
    MIN(`windspeed`),
    COUNT(`datetime`)
INTO hour_gusts , max_avg_windspeed , min_avg_windspeed , measurements_num FROM
    `data_station`
WHERE
    `station` COLLATE utf8mb4_general_ci = stationname
        AND `datetime` > SUBTIME(NOW(), '14:00:00.00');
     
REPLACE INTO `data_summary`
	(`lasttimestamp`, `numberofmeasurements`, `temperature`, `qnh`, `humidity`, `direction`, `average`, `gusts`, `hourgusts`, `hourmaxavg`, `hourminavg`, `station`)
    VALUES
    (last_timestamp, measurements_num, last_temperature, last_qnh, last_humidity, last_direction,avg_windspeed, gusts, hour_gusts, max_avg_windspeed, min_avg_windspeed, stationname);
    
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2022-11-17 21:28:50
