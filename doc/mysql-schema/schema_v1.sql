

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";



CREATE TABLE IF NOT EXISTS `dzialy` (
  `Id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `TimestampEpoch` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `TimestampDate` datetime NOT NULL,
  `Temp` int(11) NOT NULL,
  `WindSpeed` float NOT NULL,
  `WindGusts` float NOT NULL,
  `WindDir` smallint(5) unsigned NOT NULL,
  `QNH` int(10) unsigned NOT NULL,
  `Humidity` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_polish_ci AUTO_INCREMENT=1 ;