DROP TABLE IF EXISTS "Canale";
CREATE TABLE "Canale"(
	"CanID" INTEGER PRIMARY KEY  NOT NULL  UNIQUE,
	"Nome" VARCHAR NOT NULL CHECK(TRIM(Nome) <> ''),
	"CodTipoDato" INTEGER DEFAULT 1,
	"Dimensione" INTEGER DEFAULT 1,
        "CodTipoIO" INTEGER DEFAULT 1,
	"CodTipoConversione" INTEGER DEFAULT 0,
	"a" FLOAT DEFAULT 0,
	"b" FLOAT DEFAULT 0,
	"c" FLOAT DEFAULT 0,
	"Descrizione" TEXT,
	FOREIGN KEY(CodTipoDato) REFERENCES TipoDato(IDTipoDato) ON UPDATE CASCADE ON DELETE CASCADE ,
        FOREIGN KEY(CodTipoIO) REFERENCES TipoIO(IDTipoIO) ON UPDATE CASCADE ON DELETE CASCADE ,
	FOREIGN KEY(CodTipoConversione) REFERENCES TipoConversione(IDTipoConversione) ON UPDATE CASCADE ON DELETE CASCADE 
);

DROP TABLE IF EXISTS "CanaleSpeciale";
CREATE TABLE CanaleSpeciale(
    IDRuolo INTEGER NOT NULL UNIQUE PRIMARY KEY,
    Ruolo VARCHAR,
    CodCanale INTEGER NULL,
    FOREIGN KEY(CodCanale) REFERENCES Canale(CanID)
);

INSERT INTO "CanaleSpeciale" VALUES(1,'Latitudine',-1);
INSERT INTO "CanaleSpeciale" VALUES(2,'Longitudine',-1);
INSERT INTO "CanaleSpeciale" VALUES(3,'Giro',-1);
INSERT INTO "CanaleSpeciale" VALUES(4,'Spazio',-1);
INSERT INTO "CanaleSpeciale" VALUES(5,'Tempo',-1);
INSERT INTO "CanaleSpeciale" VALUES(6,'Intermedio',-1);


DROP TABLE IF EXISTS "Circuito";
CREATE TABLE "Circuito"(
	"IDCircuito" INTEGER PRIMARY KEY autoincrement NOT NULL UNIQUE,
	"Nome" VARCHAR NOT NULL CHECK(TRIM(Nome)<>'') ,
	"Modello" VARCHAR,
	"Descrizione" TEXT,
	"CentroLat" FLOAT ,
	"CentroLon" FLOAT ,
	"CodCittà" INTEGER,
	FOREIGN KEY(CodCittà) REFERENCES Città(IDCittà) ON UPDATE CASCADE ON DELETE CASCADE 
);

DROP TABLE IF EXISTS "Città";
CREATE TABLE "Città"(
	"IDCittà" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE ,
	"Nome" VARCHAR NOT NULL UNIQUE,
        "Lat" FLOAT,
        "Lon" FLOAT,
	"CodStato" INTEGER NOT NULL,
	foreign key(CodStato) REFERENCES Stato(IDStato) ON UPDATE CASCADE ON DELETE CASCADE 
);

DROP TABLE IF EXISTS "Finestra";
CREATE TABLE "Finestra" (
	"IDFinestra" INTEGER PRIMARY KEY  NOT NULL  UNIQUE,
	"Nome" VARCHAR UNIQUE NOT NULL CHECK(TRIM(Nome)<>''),
	"Descrizione" TEXT
);

DROP TABLE IF EXISTS "FinestraGrafici";
CREATE TABLE FinestraGrafici(
       "IDFinestraGrafici" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL,
	"CodFinestra" INTEGER,
	"CodCan" INTEGER,
	"Nome" VARCHAR NOT NULL CHECK(TRIM(Nome)<>''),
	"Colore" VARCHAR,
	FOREIGN KEY(CodCan) REFERENCES Canale(CanID) ON UPDATE CASCADE ON DELETE CASCADE ,
	FOREIGN KEY(CodFinestra) REFERENCES Finestra(IDFinestra) ON UPDATE CASCADE ON DELETE CASCADE 
);

DROP TABLE IF EXISTS "GPSCoord";
CREATE TABLE "GPSCoord"(
	"IDCoord" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,
	"Lat" FLOAT NOT NULL,
	"Lon" FLOAT NOT NULL,
        "Raggio" FLOAT NOT NULL DEFAULT 1,
        "Distanza" FLOAT NOT NULL DEFAULT 1,
        "Durata" FLOAT NOT NULL DEFAULT 1,
        "Riferimento" BOOLEAN DEFAULT 0,
        "Ruolo" INT NOT NULL DEFAULT 1,
	"CodCircuito" VARCHAR,
	FOREIGN KEY(CodCircuito) REFERENCES Circuito(IDCircuito) ON UPDATE CASCADE ON DELETE CASCADE 
);

DROP TABLE IF EXISTS "Gara";
CREATE TABLE "Gara" (
	"IDGara" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE , 
	"Nome" VARCHAR NOT NULL , 
	"Data" DATETIME, 
	"OraInizio" DATETIME, 
	"OraFine" DATETIME, 
	"Descrizione" TEXT,
	"CodVeicolo" INTEGER,
	"CodCircuito" INTEGER,
	foreign key(CodVeicolo) REFERENCES Veicolo(IDVeicolo) ON UPDATE CASCADE ON DELETE CASCADE ,
	foreign key(CodCircuito) REFERENCES Circuito(IDCircuito) ON UPDATE CASCADE ON DELETE CASCADE 
);

DROP TABLE IF EXISTS "ImpostazioniGSM";
CREATE TABLE ImpostazioniGSM(
    IDImpostazione INTEGER NOT NULL UNIQUE PRIMARY KEY,
    Nome VARCHAR,
    Valore VARCHAR
);

INSERT INTO "ImpostazioniGSM" VALUES(1,'APN_NAME','');
INSERT INTO "ImpostazioniGSM" VALUES(2,'APN_USER','');
INSERT INTO "ImpostazioniGSM" VALUES(3,'APN_PSW','');
INSERT INTO "ImpostazioniGSM" VALUES(4,'TCP_PORT','5678');
INSERT INTO "ImpostazioniGSM" VALUES(5,'UDP_PORT','5679');
INSERT INTO "ImpostazioniGSM" VALUES(6,'RECEIVER_NUM','');
INSERT INTO "ImpostazioniGSM" VALUES(7,'SENDER_NUM','');
INSERT INTO "ImpostazioniGSM" VALUES(8,'RECEIVER_PIN','');
INSERT INTO "ImpostazioniGSM" VALUES(9,'SENDER_PIN','');
INSERT INTO "ImpostazioniGSM" VALUES(10,'PACKET_PER_SECOND','1');
INSERT INTO "ImpostazioniGSM" VALUES(11,'PACKET_TIMEOUT','30000');
INSERT INTO "ImpostazioniGSM" VALUES(12,'PUSH_TO_TALK_REQUEST','-1');
INSERT INTO "ImpostazioniGSM" VALUES(13,'PUSH_TO_TALK_STATUS','-1');

DROP TABLE IF EXISTS "Stato";
CREATE TABLE "Stato"(
	"IDStato" INTEGER PRIMARY KEY NOT NULL UNIQUE ,
	"Nome" VARCHAR NOT NULL
);
INSERT INTO "Stato" VALUES(1,'Afghanistan');
INSERT INTO "Stato" VALUES(2,'Albania');
INSERT INTO "Stato" VALUES(3,'Algeria');
INSERT INTO "Stato" VALUES(4,'American Samoa');
INSERT INTO "Stato" VALUES(5,'Andorra');
INSERT INTO "Stato" VALUES(6,'Angola');
INSERT INTO "Stato" VALUES(7,'Anguilla');
INSERT INTO "Stato" VALUES(8,'Antigua & Barbuda');
INSERT INTO "Stato" VALUES(9,'Argentina');
INSERT INTO "Stato" VALUES(10,'Armenia');
INSERT INTO "Stato" VALUES(11,'Aruba');
INSERT INTO "Stato" VALUES(12,'Australia');
INSERT INTO "Stato" VALUES(13,'Austria');
INSERT INTO "Stato" VALUES(14,'Azerbaijan');
INSERT INTO "Stato" VALUES(15,'Bahamas, The');
INSERT INTO "Stato" VALUES(16,'Bahrain');
INSERT INTO "Stato" VALUES(17,'Bangladesh');
INSERT INTO "Stato" VALUES(18,'Barbados');
INSERT INTO "Stato" VALUES(19,'Belarus');
INSERT INTO "Stato" VALUES(20,'Belgium');
INSERT INTO "Stato" VALUES(21,'Belize');
INSERT INTO "Stato" VALUES(22,'Benin');
INSERT INTO "Stato" VALUES(23,'Bermuda');
INSERT INTO "Stato" VALUES(24,'Bhutan');
INSERT INTO "Stato" VALUES(25,'Bolivia');
INSERT INTO "Stato" VALUES(26,'Bosnia & Herzegovina');
INSERT INTO "Stato" VALUES(27,'Botswana');
INSERT INTO "Stato" VALUES(28,'Brazil');
INSERT INTO "Stato" VALUES(29,'British Virgin Is.');
INSERT INTO "Stato" VALUES(30,'Brunei');
INSERT INTO "Stato" VALUES(31,'Bulgaria');
INSERT INTO "Stato" VALUES(32,'Burkina Faso');
INSERT INTO "Stato" VALUES(33,'Burma');
INSERT INTO "Stato" VALUES(34,'Burundi');
INSERT INTO "Stato" VALUES(35,'Cambodia');
INSERT INTO "Stato" VALUES(36,'Cameroon');
INSERT INTO "Stato" VALUES(37,'Canada');
INSERT INTO "Stato" VALUES(38,'Cape Verde');
INSERT INTO "Stato" VALUES(39,'Cayman Islands');
INSERT INTO "Stato" VALUES(40,'Central African Rep.');
INSERT INTO "Stato" VALUES(41,'Chad');
INSERT INTO "Stato" VALUES(42,'Chile');
INSERT INTO "Stato" VALUES(43,'China');
INSERT INTO "Stato" VALUES(44,'Colombia');
INSERT INTO "Stato" VALUES(45,'Comoros');
INSERT INTO "Stato" VALUES(46,'Congo, Dem. Rep.');
INSERT INTO "Stato" VALUES(47,'Congo, Repub. of the');
INSERT INTO "Stato" VALUES(48,'Cook Islands');
INSERT INTO "Stato" VALUES(49,'Costa Rica');
INSERT INTO "Stato" VALUES(50,'Cote d''Ivoire');
INSERT INTO "Stato" VALUES(51,'Croatia');
INSERT INTO "Stato" VALUES(52,'Cuba');
INSERT INTO "Stato" VALUES(53,'Cyprus');
INSERT INTO "Stato" VALUES(54,'Czech Republic');
INSERT INTO "Stato" VALUES(55,'Denmark');
INSERT INTO "Stato" VALUES(56,'Djibouti');
INSERT INTO "Stato" VALUES(57,'Dominica');
INSERT INTO "Stato" VALUES(58,'Dominican Republic');
INSERT INTO "Stato" VALUES(59,'East Timor');
INSERT INTO "Stato" VALUES(60,'Ecuador');
INSERT INTO "Stato" VALUES(61,'Egypt');
INSERT INTO "Stato" VALUES(62,'El Salvador');
INSERT INTO "Stato" VALUES(63,'Equatorial Guinea');
INSERT INTO "Stato" VALUES(64,'Eritrea');
INSERT INTO "Stato" VALUES(65,'Estonia');
INSERT INTO "Stato" VALUES(66,'Ethiopia');
INSERT INTO "Stato" VALUES(67,'Faroe Islands');
INSERT INTO "Stato" VALUES(68,'Fiji');
INSERT INTO "Stato" VALUES(69,'Finland');
INSERT INTO "Stato" VALUES(70,'France');
INSERT INTO "Stato" VALUES(71,'French Guiana');
INSERT INTO "Stato" VALUES(72,'French Polynesia');
INSERT INTO "Stato" VALUES(73,'Gabon');
INSERT INTO "Stato" VALUES(74,'Gambia, The');
INSERT INTO "Stato" VALUES(75,'Gaza Strip');
INSERT INTO "Stato" VALUES(76,'Georgia');
INSERT INTO "Stato" VALUES(77,'Germany');
INSERT INTO "Stato" VALUES(78,'Ghana');
INSERT INTO "Stato" VALUES(79,'Gibraltar');
INSERT INTO "Stato" VALUES(80,'Greece');
INSERT INTO "Stato" VALUES(81,'Greenland');
INSERT INTO "Stato" VALUES(82,'Grenada');
INSERT INTO "Stato" VALUES(83,'Guadeloupe');
INSERT INTO "Stato" VALUES(84,'Guam');
INSERT INTO "Stato" VALUES(85,'Guatemala');
INSERT INTO "Stato" VALUES(86,'Guernsey');
INSERT INTO "Stato" VALUES(87,'Guinea');
INSERT INTO "Stato" VALUES(88,'Guinea-Bissau');
INSERT INTO "Stato" VALUES(89,'Guyana');
INSERT INTO "Stato" VALUES(90,'Haiti');
INSERT INTO "Stato" VALUES(91,'Honduras');
INSERT INTO "Stato" VALUES(92,'Hong Kong');
INSERT INTO "Stato" VALUES(93,'Hungary');
INSERT INTO "Stato" VALUES(94,'Iceland');
INSERT INTO "Stato" VALUES(95,'India');
INSERT INTO "Stato" VALUES(96,'Indonesia');
INSERT INTO "Stato" VALUES(97,'Iran');
INSERT INTO "Stato" VALUES(98,'Iraq');
INSERT INTO "Stato" VALUES(99,'Ireland');
INSERT INTO "Stato" VALUES(100,'Isle of Man');
INSERT INTO "Stato" VALUES(101,'Israel');
INSERT INTO "Stato" VALUES(102,'Italy');
INSERT INTO "Stato" VALUES(103,'Jamaica');
INSERT INTO "Stato" VALUES(104,'Japan');
INSERT INTO "Stato" VALUES(105,'Jersey');
INSERT INTO "Stato" VALUES(106,'Jordan');
INSERT INTO "Stato" VALUES(107,'Kazakhstan');
INSERT INTO "Stato" VALUES(108,'Kenya');
INSERT INTO "Stato" VALUES(109,'Kiribati');
INSERT INTO "Stato" VALUES(110,'Korea, North');
INSERT INTO "Stato" VALUES(111,'Korea, South');
INSERT INTO "Stato" VALUES(112,'Kuwait');
INSERT INTO "Stato" VALUES(113,'Kyrgyzstan');
INSERT INTO "Stato" VALUES(114,'Laos');
INSERT INTO "Stato" VALUES(115,'Latvia');
INSERT INTO "Stato" VALUES(116,'Lebanon');
INSERT INTO "Stato" VALUES(117,'Lesotho');
INSERT INTO "Stato" VALUES(118,'Liberia');
INSERT INTO "Stato" VALUES(119,'Libya');
INSERT INTO "Stato" VALUES(120,'Liechtenstein');
INSERT INTO "Stato" VALUES(121,'Lithuania');
INSERT INTO "Stato" VALUES(122,'Luxembourg');
INSERT INTO "Stato" VALUES(123,'Macau');
INSERT INTO "Stato" VALUES(124,'Macedonia');
INSERT INTO "Stato" VALUES(125,'Madagascar');
INSERT INTO "Stato" VALUES(126,'Malawi');
INSERT INTO "Stato" VALUES(127,'Malaysia');
INSERT INTO "Stato" VALUES(128,'Maldives');
INSERT INTO "Stato" VALUES(129,'Mali');
INSERT INTO "Stato" VALUES(130,'Malta');
INSERT INTO "Stato" VALUES(131,'Marshall Islands');
INSERT INTO "Stato" VALUES(132,'Martinique');
INSERT INTO "Stato" VALUES(133,'Mauritania');
INSERT INTO "Stato" VALUES(134,'Mauritius');
INSERT INTO "Stato" VALUES(135,'Mayotte');
INSERT INTO "Stato" VALUES(136,'Mexico');
INSERT INTO "Stato" VALUES(137,'Micronesia, Fed. St.');
INSERT INTO "Stato" VALUES(138,'Moldova');
INSERT INTO "Stato" VALUES(139,'Monaco');
INSERT INTO "Stato" VALUES(140,'Mongolia');
INSERT INTO "Stato" VALUES(141,'Montserrat');
INSERT INTO "Stato" VALUES(142,'Morocco');
INSERT INTO "Stato" VALUES(143,'Mozambique');
INSERT INTO "Stato" VALUES(144,'Namibia');
INSERT INTO "Stato" VALUES(145,'Nauru');
INSERT INTO "Stato" VALUES(146,'Nepal');
INSERT INTO "Stato" VALUES(147,'Netherlands');
INSERT INTO "Stato" VALUES(148,'Netherlands Antilles');
INSERT INTO "Stato" VALUES(149,'New Caledonia');
INSERT INTO "Stato" VALUES(150,'New Zealand');
INSERT INTO "Stato" VALUES(151,'Nicaragua');
INSERT INTO "Stato" VALUES(152,'Niger');
INSERT INTO "Stato" VALUES(153,'Nigeria');
INSERT INTO "Stato" VALUES(154,'N. Mariana Islands');
INSERT INTO "Stato" VALUES(155,'Norway');
INSERT INTO "Stato" VALUES(156,'Oman');
INSERT INTO "Stato" VALUES(157,'Pakistan');
INSERT INTO "Stato" VALUES(158,'Palau');
INSERT INTO "Stato" VALUES(159,'Panama');
INSERT INTO "Stato" VALUES(160,'Papua New Guinea');
INSERT INTO "Stato" VALUES(161,'Paraguay');
INSERT INTO "Stato" VALUES(162,'Peru');
INSERT INTO "Stato" VALUES(163,'Philippines');
INSERT INTO "Stato" VALUES(164,'Poland');
INSERT INTO "Stato" VALUES(165,'Portugal');
INSERT INTO "Stato" VALUES(166,'Puerto Rico');
INSERT INTO "Stato" VALUES(167,'Qatar');
INSERT INTO "Stato" VALUES(168,'Reunion');
INSERT INTO "Stato" VALUES(169,'Romania');
INSERT INTO "Stato" VALUES(170,'Russia');
INSERT INTO "Stato" VALUES(171,'Rwanda');
INSERT INTO "Stato" VALUES(172,'Saint Helena');
INSERT INTO "Stato" VALUES(173,'Saint Kitts & Nevis');
INSERT INTO "Stato" VALUES(174,'Saint Lucia');
INSERT INTO "Stato" VALUES(175,'St Pierre & Miquelon');
INSERT INTO "Stato" VALUES(176,'Saint Vincent and the Grenadines');
INSERT INTO "Stato" VALUES(177,'Samoa');
INSERT INTO "Stato" VALUES(178,'San Marino');
INSERT INTO "Stato" VALUES(179,'Sao Tome & Principe');
INSERT INTO "Stato" VALUES(180,'Saudi Arabia');
INSERT INTO "Stato" VALUES(181,'Senegal');
INSERT INTO "Stato" VALUES(182,'Serbia');
INSERT INTO "Stato" VALUES(183,'Seychelles');
INSERT INTO "Stato" VALUES(184,'Sierra Leone');
INSERT INTO "Stato" VALUES(185,'Singapore');
INSERT INTO "Stato" VALUES(186,'Slovakia');
INSERT INTO "Stato" VALUES(187,'Slovenia');
INSERT INTO "Stato" VALUES(188,'Solomon Islands');
INSERT INTO "Stato" VALUES(189,'Somalia');
INSERT INTO "Stato" VALUES(190,'South Africa');
INSERT INTO "Stato" VALUES(191,'Spain');
INSERT INTO "Stato" VALUES(192,'Sri Lanka');
INSERT INTO "Stato" VALUES(193,'Sudan');
INSERT INTO "Stato" VALUES(194,'Suriname');
INSERT INTO "Stato" VALUES(195,'Swaziland');
INSERT INTO "Stato" VALUES(196,'Sweden');
INSERT INTO "Stato" VALUES(197,'Switzerland');
INSERT INTO "Stato" VALUES(198,'Syria');
INSERT INTO "Stato" VALUES(199,'Taiwan');
INSERT INTO "Stato" VALUES(200,'Tajikistan');
INSERT INTO "Stato" VALUES(201,'Tanzania');
INSERT INTO "Stato" VALUES(202,'Thailand');
INSERT INTO "Stato" VALUES(203,'Togo');
INSERT INTO "Stato" VALUES(204,'Tonga');
INSERT INTO "Stato" VALUES(205,'Trinidad & Tobago');
INSERT INTO "Stato" VALUES(206,'Tunisia');
INSERT INTO "Stato" VALUES(207,'Turkey');
INSERT INTO "Stato" VALUES(208,'Turkmenistan');
INSERT INTO "Stato" VALUES(209,'Turks & Caicos Is');
INSERT INTO "Stato" VALUES(210,'Tuvalu');
INSERT INTO "Stato" VALUES(211,'Uganda');
INSERT INTO "Stato" VALUES(212,'Ukraine');
INSERT INTO "Stato" VALUES(213,'United Arab Emirates');
INSERT INTO "Stato" VALUES(214,'United Kingdom');
INSERT INTO "Stato" VALUES(215,'United States');
INSERT INTO "Stato" VALUES(216,'Uruguay');
INSERT INTO "Stato" VALUES(217,'Uzbekistan');
INSERT INTO "Stato" VALUES(218,'Vanuatu');
INSERT INTO "Stato" VALUES(219,'Venezuela');
INSERT INTO "Stato" VALUES(220,'Vietnam');
INSERT INTO "Stato" VALUES(221,'Virgin Islands');
INSERT INTO "Stato" VALUES(222,'Wallis and Futuna');
INSERT INTO "Stato" VALUES(223,'West Bank');
INSERT INTO "Stato" VALUES(224,'Western Sahara');
INSERT INTO "Stato" VALUES(225,'Yemen');
INSERT INTO "Stato" VALUES(226,'Zambia');
INSERT INTO "Stato" VALUES(227,'Zimbabwe');

DROP TABLE IF EXISTS "Storico";
CREATE TABLE "Storico"(
	"IDGara" INTEGER  NOT NULL,
       "Indice" INTEGER NOT NULL,
	"IDCan" INTEGER  NOT NULL ,
	"OraArrivo" DATETIME  NOT NULL ,
	"Valore" VARCHAR,
 	PRIMARY KEY(IDGara,IDCan,Indice),
	FOREIGN KEY(IDGara) REFERENCES Gara(IDGara) ON UPDATE CASCADE ON DELETE CASCADE ,
	FOREIGN KEY(IDCan) REFERENCES Canale(CanID) ON UPDATE CASCADE ON DELETE CASCADE 
);

DROP TABLE IF EXISTS "Team";
CREATE TABLE Team(
IDP INTEGER PRIMARY KEY AUTOINCREMENT,
Nome VARCHAR(32) NOT NULL,
Cognome VARCHAR(32) NOT NULL,
Ruolo VARCHAR(64) NOT NULL,
Tel VARCHAR(16) NOT NULL 
);


DROP TABLE IF EXISTS "TipoConversione";
CREATE TABLE "TipoConversione"(
	"IDTipoConversione" INTEGER PRIMARY KEY NOT NULL UNIQUE,
	"Tipo" VARCHAR
);

INSERT INTO "TipoConversione" VALUES(0,'x');
INSERT INTO "TipoConversione" VALUES(1,'x+a');
INSERT INTO "TipoConversione" VALUES(2,'x/a');
INSERT INTO "TipoConversione" VALUES(3,'(x+a)/b');
INSERT INTO "TipoConversione" VALUES(4,'(x+a)/b+c');

DROP TABLE IF EXISTS "TipoDato";
CREATE TABLE "TipoDato"(
	"IDTipoDato" INTEGER PRIMARY KEY NOT NULL UNIQUE,
	"Tipo" VARCHAR
);

INSERT INTO "TipoDato" VALUES(0,'BitsFlag');
INSERT INTO "TipoDato" VALUES(1,'Numero con segno');
INSERT INTO "TipoDato" VALUES(2,'Numero senza segno');
INSERT INTO "TipoDato" VALUES(3,'Stringa');

DROP TABLE IF EXISTS "TipoIO";
CREATE TABLE "TipoIO"(
	"IDTipoIO" INTEGER  NOT NULL,
	"Tipo" VARCHAR,
 	PRIMARY KEY(IDTipoIO)
);

INSERT INTO "TipoIO" VALUES(1,'Input');
INSERT INTO "TipoIO" VALUES(2,'Output');
INSERT INTO "TipoIO" VALUES(3,'Input/Output');

DROP TABLE IF EXISTS "Veicolo";
CREATE TABLE "Veicolo"(
	"IDVeicolo" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE ,
	"Nome" VARCHAR NOT NULL CHECK(TRIM(Nome)<>''),
	"Modello" VARCHAR,
	"Foto" VARCHAR,
	"Descrizione" VARCHAR
);


DROP TABLE IF EXISTS "Widget";
CREATE TABLE "Widget" (
	"IDWidget" INTEGER PRIMARY KEY  NOT NULL  UNIQUE,
	"Nome" VARCHAR UNIQUE
);

INSERT INTO "Widget" VALUES(1,'Led');
INSERT INTO "Widget" VALUES(2,'Label');
INSERT INTO "Widget" VALUES(3,'Display 7 segmenti');
INSERT INTO "Widget" VALUES(4,'Barra');
INSERT INTO "Widget" VALUES(5,'Angolometro');
INSERT INTO "Widget" VALUES(6,'Termometro');
INSERT INTO "Widget" VALUES(7,'Allarme');

DROP TABLE IF EXISTS "WidgetCruscotto";
CREATE TABLE WidgetCruscotto(
        "IDWidget" INTEGER PRIMARY KEY AUTOINCREMENT,
	"CodCan" INTEGER,
	"CodWidget" INTEGER,
	"Config" TEXT,
	FOREIGN KEY(CodCan) REFERENCES Canale(CanID) ON UPDATE CASCADE ON DELETE CASCADE ,
	FOREIGN KEY(CodWidget) REFERENCES Widget(IDWidget) ON UPDATE CASCADE ON DELETE CASCADE 
);
