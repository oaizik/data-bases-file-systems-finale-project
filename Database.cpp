#include "Database.h"
//#include <iostream>
#include <exception>
using namespace std;
void clearCin() {
	string temp;
	getline(cin, temp);
}


Database *Database::instance = 0;

Database::Database() : driver(get_driver_instance()) {
	connection_properties["hostName"] = DB_HOST;
	connection_properties["port"] = DB_PORT;
	connection_properties["userName"] = DB_USER;
	connection_properties["password"] = DB_PASS;
	connection_properties["OPT_RECONNECT"] = true;
}
	

Database & Database::getInstance() {
	if (Database::instance == 0) {
		instance = new Database();
	}
	return *instance;
}

Connection * Database::getConnection() {
	try {
		Connection *con = driver->connect(connection_properties);
		con->setSchema(DB_NAME);
		return con;
	}
	catch (SQLException &e) {
		cout << e.what();
	}
	return 0;
}


//Option 1
void Database::showBetweenTwoDates() {
	string temp1, temp2;
	Connection* con = driver->connect(connection_properties);
	con->setSchema(DB_NAME);
	ResultSet* rset;
	PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM album WHERE start_date >=? AND end_date <= ?;");

	cout << endl <<"Please enter date in this pattern YYYY-MM-DD\n" << endl;
	cout << "Please enter from date: ";
	clearCin();
	getline(cin, temp1);
	pstmt->setString(1, temp1);
	cout << "Please enter to date: ";
	getline(cin, temp2);
	pstmt->setString(2, temp2);
	rset = pstmt->executeQuery();

	if (rset->first()) {
		cout << "Number of recorded albums between " << temp1 << " and " << temp2 << " is: " << rset->rowsCount() << "." << endl;
	}
	else cout << "Invalid Date or No albums That Where recorded Between Dates." << endl;


	delete con;
	delete pstmt;
	delete rset;
}

//Option 2
void Database::showSongsBetweenTwoDates() {

	string temp1, temp2, temp3;
	Connection* con = driver->connect(connection_properties);
	con->setSchema(DB_NAME);
	ResultSet* rset;
	PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM track_musician LEFT JOIN musical_track ON musical_track.id = track_musician.track_id WHERE recording_date between ? AND ? AND person.name LIKE ?;");


	cout << endl << "Please enter date in this pattern YYYY-MM-DD\n" << endl;
	cout << "Please enter from date: ";
	clearCin();
	getline(cin, temp1);
	pstmt->setString(1, temp1);
	cout << "Please enter to date: ";
	getline(cin, temp2);
	pstmt->setString(2, temp2);
	cout << "Please enter musician Name: ";
	getline(cin, temp3);
	pstmt->setString(3, temp3);

	rset = pstmt->executeQuery();

	if (rset->first()) {
		cout << "Number of recorded songs of "<< temp3 << " between " << temp1 << " and " << temp2 << " is: " << rset->rowsCount() << "." << endl;
	}
	else cout << "Invalid Date or No Orders Found That Where Ordered Between Dates." << endl;


	delete con;
	delete pstmt;
	delete rset;
}

//Option 3
void Database::showAlbumsBetweenDatesAndComposer() {
	string temp1, temp2, temp3;
	Connection* con = driver->connect(connection_properties);
	con->setSchema(DB_NAME);
	ResultSet* rset;
	PreparedStatement* pstmt = con->prepareStatement("SELECT DISTINCT album.id, person.name FROM track_musician LEFT JOIN musical_track ON track_musician.track_id=musical_track.id LEFT JOIN person ON track_musician.musician_id=person.id LEFT JOIN tracks_album ON musical_track.id= tracks_album.track_id LEFT JOIN album ON album.id=tracks_album.album_id WHERE start_date >= AND end_date <= ? AND person.name LIKE ?;");


	cout << endl << "Please enter date in this pattern YYYY-MM-DD\n" << endl;
	cout << "Please enter from date: ";
	clearCin();
	getline(cin, temp1);
	pstmt->setString(1, temp1);
	cout << "Please enter to date: ";
	getline(cin, temp2);
	pstmt->setString(2, temp2);
	cout << "Please enter musician Name: ";
	getline(cin, temp3);
	pstmt->setString(3, temp3);

	rset = pstmt->executeQuery();

	if (rset->first()) {
		cout << "Number of recorded albums where included " << temp3 << " between " << temp1 << " and " << temp2 << " is: " << rset->rowsCount() << "." << endl;
	}
	else cout << "Invalid Date or No Orders Found That Where Ordered Between Dates." << endl;


	delete con;
	delete pstmt;
	delete rset;
}

//Option 4
void Database::showMostPopularInstrument() {
	Connection *con = driver->connect(connection_properties);
	con->setSchema(DB_NAME);
	Statement *stmt = con->createStatement();
	ResultSet* rset = stmt->executeQuery("SELECT  instrument_type.description , instrument_id , count(instrument_id) AS ct FROM track_instruments LEFT JOIN instruments_stock ON track_instruments.instrument_id = instruments_stock.id LEFT JOIN instrument_type ON instruments_stock.instrumen_type_id = instrument_type.id GROUP BY instrument_id ORDER BY ct DESC LIMIT 1; ");

	rset->beforeFirst();

	if (rset->first()) {
		cout <<"Most used instrument is: "<< rset->getString("description") <<" appears " << rset->getString("ct") << " times." << endl;
	}

	delete con;
	delete stmt;
	delete rset;
}

//Option 5
void Database::instrumentsInAlbum() {
	string temp1;
	int counter = 1;
	Connection* con = driver->connect(connection_properties);
	con->setSchema(DB_NAME);
	ResultSet* rset;
	PreparedStatement* pstmt = con->prepareStatement("SELECT tracks_album.album_id , album.album_name , tracks_album.track_id,track_instruments.instrument_id,instrument_type.description ,manf.manufacturer_name FROM track_instruments LEFT JOIN tracks_album ON track_instruments.track_id=tracks_album.track_id LEFT JOIN album ON album.id = tracks_album.album_id LEFT JOIN instruments_stock AS ins_stock ON ins_stock.id=track_instruments.instrument_id LEFT JOIN instrument_type ON ins_stock.instrumen_type_id = instrument_type.id LEFT JOIN manufacturers AS manf ON ins_stock.manufacturer_id=manf.id WHERE album.album_name LIKE ?;");

	cout << "Please enter album name: ";
	clearCin();
	getline(cin, temp1);
	pstmt->setString(1, temp1);

	rset = pstmt->executeQuery();

	if (rset->rowsCount() == 0) {
		cout << "No such album found" << endl;
		return;
	}

	rset->beforeFirst();
	cout << "Instruments that album " << temp1 << " includes are:" << endl;

	while (rset->next()) {
		cout << counter << ") " << rset->getString("description") << endl;
		++counter;
	}

	delete con;
	delete pstmt;
	delete rset;
}

//Option 6
void Database::releaseMostNumOfAlbums() {
	string temp1, temp2;
	Connection* con = driver->connect(connection_properties);
	con->setSchema(DB_NAME);
	ResultSet* rset;
	PreparedStatement* pstmt = con->prepareStatement("SELECT album_producer.producer_id ,person.name , count(album_producer.producer_id) as ct FROM album_producer LEFT JOIN album ON album_producer.album_id=album.id LEFT JOIN person ON person.id= album_producer.producer_id WHERE (start_date >=? AND end_date <= ?) GROUP BY album_producer.producer_id ORDER BY ct DESC LIMIT 1;");

	cout << endl << "Please enter date in this pattern YYYY-MM-DD\n" << endl;
	cout << "Please enter from date: ";
	clearCin();
	getline(cin, temp1);
	pstmt->setString(1, temp1);
	cout << "Please enter to date: ";
	getline(cin, temp2);
	pstmt->setString(2, temp2);
	rset = pstmt->executeQuery();

	if (rset->first()) {
		cout << "Most released albums producer is: " << rset->getString("name") << " which released " << rset->getString("ct") << " albums." << endl;
	}
	else cout << "Invalid Date or No Orders Found That Where Ordered Between Dates." << endl;

	delete con;
	delete pstmt;
	delete rset;
}

//Option 7
void Database::mostPopularManufacturer() {
	Connection* con = driver->connect(connection_properties);
	con->setSchema(DB_NAME);
	Statement* stmt = con->createStatement();
	ResultSet* rset = stmt->executeQuery("SELECT * FROM(SELECT instruments_stock.manufacturer_id, count(instruments_stock.manufacturer_id) as ct FROM track_instruments LEFT JOIN instruments_stock ON track_instruments.instrument_id = instruments_stock.id GROUP BY instruments_stock.manufacturer_id ORDER BY ct DESC LIMIT 1) AS manf LEFT JOIN manufacturers on manf.manufacturer_id = manufacturers.id;");
	int counter = 1;
	rset->beforeFirst();
	cout << "Most:" << endl;

	if (rset->first()) {
		cout << "Most popular manufacturer maker is: " << rset->getString("manufacturer_name") << "and his instruments take part in " << rset->getString("ct") << " records." << endl;
	}

	delete con;
	delete stmt;
	delete rset;
	counter = 1;
}

// Option 8
void Database::totalRecordsInMin() {

	Connection* con = driver->connect(connection_properties);
	con->setSchema(DB_NAME);
	Statement* stmt = con->createStatement();
	ResultSet* rset = stmt->executeQuery("SELECT count( DISTINCT (track_musician.musician_id)) AS ct FROM track_musician;");
	rset->beforeFirst();

	if (rset->first()) {
		cout << "the total number of musicians are ever recorded is : " << rset->getString("ct") << " musicians"<< endl;
	}

	delete con;
	delete stmt;
	delete rset;
}

// Option 9
void Database::mostAssistMusician() {

	Connection* con = driver->connect(connection_properties);
	con->setSchema(DB_NAME);
	Statement* stmt = con->createStatement();
	ResultSet* rset = stmt->executeQuery("SELECT musician.musician_name,  COUNT(*) AS count FROM song_with INNER JOIN musician ON song_with.musician_id = musician.musician_id GROUP BY musician.musician_name ORDER BY count DESC LIMIT 1;");
	rset->beforeFirst();

	if (rset->first()) {
		cout << "Most assisted musician : " << rset->getString("musician_name") << " with " << rset->getString("count") << " assists" <<endl;
	}

	delete con;
	delete stmt;
	delete rset;
}

// Option 10
void Database::mostPopularGenre() {
	Connection* con = driver->connect(connection_properties);
	con->setSchema(DB_NAME);
	Statement* stmt = con->createStatement();
	ResultSet* rset = stmt->executeQuery("SELECT genre , count(genre) AS ct FROM musical_track GROUP BY musical_track.genre ORDER BY ct DESC LIMIT 1;");
	rset->beforeFirst();

	if (rset->first()) {
		cout << "Most popular genre : " << rset->getString("genre") << " with " << rset->getString("ct") << " songs" << endl;
	}

	delete con;
	delete stmt;
	delete rset;
}

// Option 11 
void Database::bestCustomerDetails() {

	string temp1, temp2;
	Connection *con = driver->connect(connection_properties);
	con->setSchema(DB_NAME);
	ResultSet *rset;
	PreparedStatement *pstmt = con->prepareStatement("SELECT technician_id, person.name,count(musical_track.technician_id) AS ct FROM musical_track LEFT JOIN `person` ON musical_track.technician_id = person.Id WHERE (recording_date >= ? AND recording_date <= ? ) GROUP BY musical_track.technician_id ORDER BY ct DESC LIMIT 1;");
	cout << "\nPlease enter date in this pattern YYYY-MM-DD\n" << endl;
	cout << "Please Enter from Date:> ";
	clearCin();
	getline(cin, temp1);
	pstmt->setString(1, temp1);
	cout << "Please Enter to Date:> ";
	clearCin();
	getline(cin, temp2);
	pstmt->setString(2, temp2);
	rset = pstmt->executeQuery();

	if (rset->first()) {
		cout <<"the techinician who recorded most of the songs is: "<< rset->getString("name") << " " << ", and he recorded " << rset->getUInt("ct") << "tracks " <<"from:"<< temp1<<" to: "<<temp2 << endl;
	}

	else cout << "Invalid Date or No tracks who recorded in those dates." << endl;


	delete con;
	delete pstmt;
	delete rset;
}

//Option 12
void Database::firstRecAlbum() {
	Connection* con = driver->connect(connection_properties);
	con->setSchema(DB_NAME);
	Statement* stmt = con->createStatement();
	ResultSet* rset = stmt->executeQuery("SELECT * FROM album WHERE end_date =  ( SELECT MIN(end_date) FROM album );");
	rset->beforeFirst();

	if (rset->first()) {
		cout << "First recorded album is: " << rset->getString("album_name") << " at " << rset->getString("end_date") << endl;
	}

	delete con;
	delete stmt;
	delete rset;
}

//Option 13
void Database::songsInTwoOrMoreAlbums() {

	int counter = 1;
	Connection* con = driver->connect(connection_properties);
	con->setSchema(DB_NAME);
	Statement* stmt = con->createStatement();
	ResultSet* rset = stmt->executeQuery("SELECT track_id, track_name , count(track_id) AS ct FROM tracks_album RIGHT JOIN musical_track ON tracks_album.track_id = musical_track.id  GROUP BY tracks_album.track_id  HAVING count(track_id) >1;");

	rset->beforeFirst();

	if (rset->first()) {
		cout << "The songs who includes in two or more albums are: " << endl;
		do {
			cout << counter << ")" << rset->getString("track_name") << " includes in: " << rset->getString("ct") << " albums." << endl;
			++counter;
		} while (rset->next());
	}
	else cout << "No duplicate songs found." << endl;

	delete con;
	delete stmt;
	delete rset;
}

//Option 14
void Database::techniciansInWholeAlbum() {

	int counter = 1;
	Connection* con = driver->connect(connection_properties);
	con->setSchema(DB_NAME);
	Statement* stmt = con->createStatement();
	ResultSet* rset = stmt->executeQuery("SELECT tb1.album_id,tb1.technician_id,person.name FROM (SELECT tb.album_id,tb.technician_id,count(tb.technician_id) AS ct FROM (SELECT track.id , track.track_name , tracks_album.album_id , track.technician_id ,count(technician_id) FROM musical_track AS track LEFT JOIN tracks_album ON track.id = tracks_album.track_id GROUP BY track.technician_id ,tracks_album.album_id ORDER BY tracks_album.album_id ASC) AS tb GROUP BY tb.album_id) AS tb1 LEFT JOIN person ON person.id=tb1.technician_id WHERE tb1.ct =1;");
	rset->beforeFirst();

	if (rset->first()) {
		cout << "the technicians who recorded whole album are: " << endl;
		while (rset->next()) {
			cout<<counter<<") "<<rset->getString("name") << " and the album id is: "<<rset->getString("album_id")<<"."  << endl;
			++counter;
		}
	}

	delete con;
	delete stmt;
	delete rset;
}

//Option 15
void Database::musicianWithMostGenres() {

	Connection* con = driver->connect(connection_properties);
	con->setSchema(DB_NAME);
	Statement* stmt = con->createStatement();
	ResultSet* rset = stmt->executeQuery("SELECT tb.name,count(tb.musician_id) AS ct1 FROM (SELECT  musician_id, genre ,person.name,count(*) AS ct FROM track_musician LEFT JOIN musical_track ON track_musician.track_id =musical_track.id LEFT JOIN person ON person.id = musician_id GROUP BY musician_id, genre) AS tb GROUP BY tb.name ORDER BY ct1 DESC LIMIT 1;");
	rset->beforeFirst();

	if (rset->first()) {
		cout << "Most genres musician: " << rset->getString("name") << " with " << rset->getString("ct1") << " genres."<< endl;
	}

	delete con;
	delete stmt;
	delete rset;

}