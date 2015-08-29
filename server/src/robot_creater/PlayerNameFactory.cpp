#include "PlayerNameFactory.h"
#include "utils.h"
#include <fstream>
using std::ifstream;

#define FILE_FirstName_Male   "../conf/name_maleFirstName.txt"
#define FILE_FirstName_Female "../conf/name_femaleFirstName.txt"
#define FILE_LastName		  "../conf/name_lastName.txt"


PlayerNameFactory* PlayerNameFactory::_playerNameFactory = NULL;


PlayerNameFactory::PlayerNameFactory()
{
	srand(time(NULL));
    const int max_buf = 256;
	char temp[max_buf];
	std::ifstream iFile;

	//male first name
	iFile.open(FILE_FirstName_Male);
	if (iFile.good()) {
		while (iFile.getline(temp, max_buf)) {
			_maleFirstName.push_back(temp);
		}
		iFile.close();
	}

	//female first name
	iFile.open(FILE_FirstName_Female);
	if (iFile.good()) {
		while (iFile.getline(temp, max_buf)) {
			_femaleFirstName.push_back(temp);
		}
		iFile.close();
	}

	//last name
	iFile.open(FILE_LastName);
	if (iFile.good()) {
		while (iFile.getline(temp, max_buf)) {
			_lastName.push_back(temp);
		}
		iFile.close();
	}
}


PlayerNameFactory::~PlayerNameFactory()
{
	this->destroyFactory();
}


PlayerNameFactory* PlayerNameFactory::getFactory()
{
	if (! _playerNameFactory) 
        _playerNameFactory = new PlayerNameFactory;
	return _playerNameFactory;
}


void PlayerNameFactory::destroyFactory()
{
	if (_playerNameFactory){
		delete _playerNameFactory;
		_playerNameFactory = NULL;
	}
}


string PlayerNameFactory::getRandomName(bool male)
{
	string name;

	//last name
	int lastIdx = Utl::auto_random() % _lastName.size();
	name = _lastName.at(lastIdx);
    
	//first name
	if (male) {
		int firstIdx = Utl::auto_random() % _maleFirstName.size();
		name += _maleFirstName.at(firstIdx);
	} else {
		int firstIdx = Utl::auto_random() % _femaleFirstName.size();
		name += _femaleFirstName.at(firstIdx);
	}

	return name;
}


void PlayerNameFactory::addOneRepeatName(string name)
{
	_repeatName.push_back(name);
}
