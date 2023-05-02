-- MySQL dump 10.13  Distrib 5.7.41, for Win32 (AMD64)
--
-- Host: localhost    Database: sokoban
-- ------------------------------------------------------
-- Server version	5.7.41-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `tb_cax`
--

DROP TABLE IF EXISTS `tb_cax`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tb_cax` (
  `cod_cax` int(11) NOT NULL AUTO_INCREMENT,
  `cod_map` int(11) NOT NULL,
  `quant` int(11) NOT NULL,
  `bpi` varchar(500) NOT NULL,
  `bpj` varchar(500) NOT NULL,
  `pi` varchar(500) DEFAULT NULL,
  `pj` varchar(500) DEFAULT NULL,
  `soma` int(11) DEFAULT NULL,
  PRIMARY KEY (`cod_cax`),
  KEY `fk_map` (`cod_map`),
  CONSTRAINT `fk_map` FOREIGN KEY (`cod_map`) REFERENCES `tb_map` (`cod_map`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tb_cax`
--

LOCK TABLES `tb_cax` WRITE;
/*!40000 ALTER TABLE `tb_cax` DISABLE KEYS */;
INSERT INTO `tb_cax` VALUES (1,1,4,'150,150,200,250','150,250,150,200','1,3,4,6','3,6,1,4',18),(2,2,3,'100,100,150','100,150,100','3,4,5','7,7,7',12),(3,3,4,'100,150,150,200','100,200,350,300','4,4,5,5','2,3,2,3',18),(4,4,6,'100,150,200,200,350,350','250,350,250,350,100,250','6,6,7,7,8,8','16,17,16,17,16,17',33);
/*!40000 ALTER TABLE `tb_cax` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tb_map`
--

DROP TABLE IF EXISTS `tb_map`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tb_map` (
  `cod_map` int(11) NOT NULL AUTO_INCREMENT,
  `tami` int(11) NOT NULL,
  `tamj` int(11) NOT NULL,
  `pi` int(11) NOT NULL,
  `pj` int(11) NOT NULL,
  `larg_tl` int(11) NOT NULL,
  `alt_tl` int(11) NOT NULL,
  `bit_pi` int(11) NOT NULL,
  `bit_pj` int(11) NOT NULL,
  `mapa` varchar(3000) NOT NULL,
  `larg_mini` int(11) DEFAULT NULL,
  PRIMARY KEY (`cod_map`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tb_map`
--

LOCK TABLES `tb_map` WRITE;
/*!40000 ALTER TABLE `tb_map` DISABLE KEYS */;
INSERT INTO `tb_map` VALUES (1,8,8,4,4,400,400,200,200,'0011100000101000001011111113040110052111111161000001010000011100',120),(2,9,9,1,3,450,450,50,150,'111110000100210000103410111105010101111011101011000001010001001010001111011111000',135),(3,7,10,3,2,500,350,150,100,'0111111100010000011111311100011020400501100010601111001000100111111110',150),(4,11,19,8,11,950,550,400,550,'00001111100000000000000100010000000000000013001000000000000111004110000000000010050601000000000111010110100011111110001011011111000011070080000000000001111110111012110000100001000001111111110000111111100000000',285);
/*!40000 ALTER TABLE `tb_map` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tb_movs`
--

DROP TABLE IF EXISTS `tb_movs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tb_movs` (
  `cod_movs` int(11) NOT NULL AUTO_INCREMENT,
  `cod_map` int(11) NOT NULL,
  `cod_user` int(11) NOT NULL,
  `movimentos` int(11) NOT NULL,
  PRIMARY KEY (`cod_movs`),
  KEY `fk_mapmovs` (`cod_map`),
  KEY `fk_user` (`cod_user`),
  CONSTRAINT `fk_mapmovs` FOREIGN KEY (`cod_map`) REFERENCES `tb_map` (`cod_map`),
  CONSTRAINT `fk_user` FOREIGN KEY (`cod_user`) REFERENCES `tb_user` (`cod_user`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tb_movs`
--

LOCK TABLES `tb_movs` WRITE;
/*!40000 ALTER TABLE `tb_movs` DISABLE KEYS */;
INSERT INTO `tb_movs` VALUES (1,1,1,10),(2,2,1,91),(3,3,1,120),(4,4,1,256);
/*!40000 ALTER TABLE `tb_movs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tb_user`
--

DROP TABLE IF EXISTS `tb_user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tb_user` (
  `cod_user` int(11) NOT NULL AUTO_INCREMENT,
  `nome` varchar(20) NOT NULL,
  PRIMARY KEY (`cod_user`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tb_user`
--

LOCK TABLES `tb_user` WRITE;
/*!40000 ALTER TABLE `tb_user` DISABLE KEYS */;
INSERT INTO `tb_user` VALUES (1,'daniel');
/*!40000 ALTER TABLE `tb_user` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2023-05-02 19:52:03
