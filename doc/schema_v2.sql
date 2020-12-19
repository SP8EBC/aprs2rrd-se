-- phpMyAdmin SQL Dump
-- version 4.6.6deb4+deb9u2
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: Dec 19, 2020 at 10:55 AM
-- Server version: 10.1.47-MariaDB-0+deb9u1
-- PHP Version: 7.0.33-0+deb9u10

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `aprs2rrd`
--

-- --------------------------------------------------------

--
-- Table structure for table `data_diff`
--

CREATE TABLE `data_diff` (
  `id` int(10) UNSIGNED NOT NULL,
  `epoch` bigint(20) NOT NULL,
  `datetime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `station` varchar(32) NOT NULL,
  `temperaturefrom` varchar(10) NOT NULL,
  `temperaturesubtract` varchar(10) NOT NULL,
  `windfrom` int(11) NOT NULL,
  `windsubtract` varchar(10) NOT NULL,
  `temperature` float NOT NULL,
  `winddir` smallint(6) NOT NULL,
  `windspeed` float NOT NULL,
  `windgusts` float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Table structure for table `data_station`
--

CREATE TABLE `data_station` (
  `id` int(10) UNSIGNED NOT NULL,
  `epoch` bigint(20) NOT NULL,
  `datetime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP,
  `station` varchar(32) NOT NULL,
  `temperature` float NOT NULL,
  `humidity` tinyint(3) UNSIGNED NOT NULL,
  `pressure` float NOT NULL,
  `winddir` smallint(5) UNSIGNED NOT NULL,
  `windspeed` float NOT NULL,
  `windgusts` float NOT NULL,
  `tsource` varchar(10) NOT NULL,
  `wsource` varchar(10) NOT NULL,
  `psource` varchar(10) NOT NULL,
  `hsource` varchar(10) NOT NULL,
  `rsource` varchar(10) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Table structure for table `data_station_def`
--

CREATE TABLE `data_station_def` (
  `id` int(10) UNSIGNED NOT NULL,
  `name` varchar(32) NOT NULL,
  `enabled` tinyint(1) NOT NULL DEFAULT '1',
  `callsign` varchar(6) NOT NULL,
  `ssid` tinyint(48) NOT NULL,
  `displayedname` varchar(32) NOT NULL,
  `displayedlocation` varchar(48) NOT NULL,
  `sponsorurl` varchar(48) DEFAULT NULL,
  `backgroundjpg` varchar(64) DEFAULT NULL,
  `moreinfo` varchar(255) DEFAULT NULL,
  `lat` float NOT NULL,
  `lon` float NOT NULL,
  `haswind` tinyint(1) NOT NULL,
  `hastemperature` tinyint(1) NOT NULL,
  `hasqnh` tinyint(1) NOT NULL,
  `hashumidity` tinyint(1) NOT NULL,
  `hasrain` tinyint(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Table structure for table `data_telemetry`
--

CREATE TABLE `data_telemetry` (
  `id` int(11) NOT NULL,
  `epoch` bigint(20) NOT NULL,
  `datetime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `station` varchar(32) NOT NULL,
  `number` tinyint(3) UNSIGNED NOT NULL,
  `first` float NOT NULL,
  `second` float NOT NULL,
  `third` float NOT NULL,
  `fourth` float NOT NULL,
  `fifth` float NOT NULL,
  `digital` tinyint(4) NOT NULL,
  `rawfirst` tinyint(3) UNSIGNED NOT NULL,
  `rawsecond` tinyint(3) UNSIGNED NOT NULL,
  `rawthird` tinyint(3) UNSIGNED NOT NULL,
  `rawfourth` tinyint(3) UNSIGNED NOT NULL,
  `rawfifth` tinyint(3) UNSIGNED NOT NULL,
  `frame` varchar(90) NOT NULL,
  `type` tinyint(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `data_diff`
--
ALTER TABLE `data_diff`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `data_station`
--
ALTER TABLE `data_station`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `data_station_def`
--
ALTER TABLE `data_station_def`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `data_telemetry`
--
ALTER TABLE `data_telemetry`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `data_diff`
--
ALTER TABLE `data_diff`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `data_station`
--
ALTER TABLE `data_station`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `data_station_def`
--
ALTER TABLE `data_station_def`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `data_telemetry`
--
ALTER TABLE `data_telemetry`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
