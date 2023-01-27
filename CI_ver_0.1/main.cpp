// New_mind_beta.1: create map, add functionality for moving around the map.

#define GAMEBOSS false

#include <windows.h>
#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;
using std::cout;

// Модель локации игры
struct Room {
	string short_desc = {};			// Краткое описание локации
	string long_desc = {};			// Полное описание локации
	Room* north = nullptr;			// Указатель локации на севере
	Room* south = nullptr;			// Указатель локации на юге
	Room* west = nullptr;			// Указатель локации на западе
	Room* east = nullptr;			// Указатель локации на востоке
	Room* down = nullptr;			// Указатель локации внизу
	Room* up = nullptr;				// Указатель локации вверху
	Room* north_west = nullptr;		// Указатель локации на северо-западе
	Room* south_east = nullptr;		// Указатель локации на юго-востоке
};

// Модель роботизированного револьвера
struct Weapon {
	std::string name;						// Наименование оружия
	int total_bullets = NULL;				// Общее кол-во патронов
	int damage = NULL;						// Уроний револьвера
	int billets_in_cylinder = NULL;			// Кол-во патронов в револьверном барабане
	int max_bullets_in_cylinder = NULL;		// Максимально допустимое кол-во патронов в барабане револьвера
};

// Модель главного героя
struct Hero {
	int HP = NULL;					// Кол-во текущего HP ГГ
	int score = NULL;				// Игровые очки
	int energy_crystals = NULL;		// Кол-во текущей энергии ГГ
	int microchip = NULL;			// Кол-во собраных микрочипов
	Room* location = nullptr;		// Локация нахождения ГГ
	Weapon revolver = {};			// Экипированное оружие ГГ
};

// Список типов противника
enum class MonsterType {
	Shooter,		// Стрелок
	Stormtrooper,	// Штурмовик
	Firetrooper,	// Огневик
	LevelBoss,		// Финальный Босс Игры
};

enum class Mark { Red, Green, Blue, Black, Orange, White, Yellow, Brown, Grey, Purple, Gold, Silver };

// Модель юнита монстра
struct Monsters {
	int HP = NULL;							// Запас HP монстра
	int score = NULL;						// Количество очков за победу над монстром
	int gives_energy_crystals = NULL;		// Кол-во кристалов, получаемых после победы над монстром
	MonsterType type = {};					// Тип монстра
	Room* room = nullptr;					// Локация расположения монстра
	Mark mark = {};							// Отличительная метка монстра
};

// Список входных параметров файла locations.txt
enum class Locations {
	ID,					// ID локации
	ShortDescription,	// Короткое описание локации
	LongDescription,	// Полное описание локации
	NumShooters,		// Кол-во стрелков
	NumStormtroopers,	// Кол-во штурмовиков
	NumFiretroopers,	// Кол-во огневиков
	Lootbox				// Лутбокс: аптечка или патроны, или микрочип
};

// Список входных параметров файла connections.txt
enum class Connections {
	IDfrom,		// ID локации "из"
	IDto,		// ID локации "в"
	Direction	// Направление перемещения
};

// Список описания типов содержимого лутбокса
enum class LootboxType { medicine = 1, ammo, microchip };

// Список описания состояний лутбокса
enum class Status { open, closed };

// Модель лутбокса
struct Lootboxes {
	LootboxType type;
	Status status;
	Room* location = nullptr;
};

// Обобщающая модель игры
struct Map {
	int game_turn_counter = NULL;			// Счётчик ходов игры
	int total_rooms_on_map = NULL;			// Общее количество локаций в игре
	int total_monsters_on_map = NULL;		// Общее количество монстров в игре
	bool game_boss_existence = true;		// Наличие Боса в конце игры // <== Маркер наличия Боса в конце игры.
	Hero hero = {};							// Сводная модель описания ГГ
	Room* rooms = nullptr;					// Сводная модель описания всех локаций игры
	Room* game_start_room = nullptr;		// Локация начала игры ГГ
	Room* game_end_room = nullptr;			// Локация окончания игры ГГ
	Monsters* monsters = nullptr;			// Сводная модель описания всех монстров в игре
	Lootboxes* lootboxes = nullptr;			// Сводная модель описания разбросаных аптечек и патронов
};

// Проверка доступности исходных файлов "locations.txt" и "connections.txt".
void fileOpenCheck(ifstream& file_locations, ifstream& file_connections) {
	if (!file_locations.is_open()) {
		throw std::exception("File \"locations.txt\" was not found!");
	}
	if (!file_connections.is_open()) {
		throw std::exception("File \"connections.txt\" was not found!");
	}
	return;
}

// Функция возвращает каретку в начало читаемого файла, пропуская заголовочную строку
void setCarriageAtBegining(ifstream& file) {
	file.clear();
	file.seekg(0, ios_base::beg);
	std::string line;
	getline(file, line);
	return;
}

// Прикручиваем ввод/вывод на русском
#ifdef WIN32
#include <locale.h>
#include <vector>
#include <Windows.h>
std::string utf8_to_cp1251(std::string const& utf8)
{
	if (!utf8.empty())
	{
		int wchlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), NULL, 0);
		if (wchlen > 0 && wchlen != 0xFFFD)
		{
			std::vector<wchar_t> wbuf(wchlen);
			int result_u = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), &wbuf[0], wchlen);
			if (!result_u) {
				throw std::runtime_error("utf8_to_cp1251 cannot convert MultiByteToWideChar!");
			}
			std::vector<char> buf(wchlen);
			int result_c = WideCharToMultiByte(1251, 0, &wbuf[0], wchlen, &buf[0], wchlen, 0, 0);
			if (!result_c) {
				throw std::runtime_error("utf8_to_cp1251 cannot convert WideCharToMultiByte!");
			}

			return std::string(&buf[0], wchlen);
		}
	}
	return std::string();
}

std::string cp1251_to_utf8(const std::string& cp1251) {
	std::string res;
	int result_u, result_c;
	enum { CP1251 = 1251 };
	result_u = MultiByteToWideChar(CP1251, 0, cp1251.c_str(), -1, 0, 0);
	if (!result_u) {
		throw std::runtime_error("cp1251_to_utf8 cannot convert MultiByteToWideChar!");
	}
	wchar_t* ures = new wchar_t[result_u];
	if (!MultiByteToWideChar(CP1251, 0, cp1251.c_str(), -1, ures, result_u)) {
		delete[] ures;
		throw std::runtime_error("cp1251_to_utf8 cannot convert MultiByteToWideChar 2!");
	}
	result_c = WideCharToMultiByte(CP_UTF8, 0, ures, -1, 0, 0, 0, 0);
	if (!result_c) {
		delete[] ures;
		throw std::runtime_error("cp1251_to_utf8 cannot convert WideCharToMultiByte!");
	}
	char* cres = new char[result_c];
	if (!WideCharToMultiByte(CP_UTF8, 0, ures, -1, cres, result_c, 0, 0)) {
		delete[] cres;
		throw std::runtime_error("cp1251_to_utf8 cannot convert WideCharToMultiByte 2!");
	}
	delete[] ures;
	res.append(cres);
	delete[] cres;
	return res;
}

#endif
void printOutput(const std::string& data)
{
	std::string output = data;
#ifdef WIN32
	output = utf8_to_cp1251(output);
#endif
	std::cout << output;
	std::cout.flush();
}
std::string readInput()
{
	std::string input;
	std::getline(std::cin, input);
#ifdef WIN32
	input = cp1251_to_utf8(input);
#endif
	return input;
}

// Считывание файла "locations.txt", чобы узнать общее кол-во локаций на карте (строка = локация).
const int countRooms(ifstream& file_locations) {
	setCarriageAtBegining(file_locations);

	std::string line;
	int total_rooms_on_map = NULL;
	while (!file_locations.eof()) {
		getline(file_locations, line);
		if (!line.empty()) {
			++total_rooms_on_map;
		}
	}
	return total_rooms_on_map;
}

// Считывание файла "locations.txt", чобы узнать общее кол-во монстров.
const int countMonsters(ifstream& file_locations) {
	const int FIELDS_IN_LINE = 7;					// A line in the file Locations.txt.
	int total_monsters_on_map = NULL;
	std::string line, box;
	vector<string> temp_line;

	setCarriageAtBegining(file_locations);
	while (!file_locations.eof()) {
		getline(file_locations, line);
		if (!line.empty()) {
			stringstream inflow(line);
			while (getline(inflow, box, ';')) {
				temp_line.push_back(box);
			}
			if (temp_line.size() == FIELDS_IN_LINE) {
				total_monsters_on_map += (stoi(temp_line[(int)Locations::NumShooters]) +
										  stoi(temp_line[(int)Locations::NumStormtroopers]) +
										  stoi(temp_line[(int)Locations::NumFiretroopers]));
			}
			temp_line.clear();
		}
	}
	if (GAMEBOSS) {
		return (total_monsters_on_map + 1);
	} else {
		return total_monsters_on_map;
	}
}

// Генерация игровых локаций.
Room* modelRooms(Map& map, ifstream& file_locations, ifstream& file_connections) {
	map.rooms = new Room[map.total_rooms_on_map];
	vector<string> temp_line;
	string line, box;
	// Записываем Short, Long Descriptions
	setCarriageAtBegining(file_locations);
	while (!file_locations.eof()) {
		getline(file_locations, line);
		stringstream inflow(line);
		while (getline(inflow, box, ';')) {
			temp_line.push_back(box);
		}
		map.rooms[stoi(temp_line[(int)Locations::ID])].short_desc = temp_line[(int)Locations::ShortDescription];
		map.rooms[stoi(temp_line[(int)Locations::ID])].long_desc = temp_line[(int)Locations::LongDescription];
		temp_line.clear();
	}
	// Записываем Directions
	setCarriageAtBegining(file_connections);
	while (!file_connections.eof()) {
		getline(file_connections, line);
		stringstream inflow(line);
		while (getline(inflow, box, ';')) {
			temp_line.push_back(box);
		}
		if (temp_line[(int)Connections::Direction] == "n") {
			map.rooms[stoi(temp_line[(int)Connections::IDfrom])].north
				= &map.rooms[stoi(temp_line[(int)Connections::IDto])];
		}
		else if (temp_line[(int)Connections::Direction] == "s") {
			map.rooms[stoi(temp_line[(int)Connections::IDfrom])].south
				= &map.rooms[stoi(temp_line[(int)Connections::IDto])];
		}
		else if (temp_line[(int)Connections::Direction] == "w") {
			map.rooms[stoi(temp_line[(int)Connections::IDfrom])].west
				= &map.rooms[stoi(temp_line[(int)Connections::IDto])];
		}
		else if (temp_line[(int)Connections::Direction] == "e") {
			map.rooms[stoi(temp_line[(int)Connections::IDfrom])].east
				= &map.rooms[stoi(temp_line[(int)Connections::IDto])];
		}
		else if (temp_line[(int)Connections::Direction] == "u") {
			map.rooms[stoi(temp_line[(int)Connections::IDfrom])].up
				= &map.rooms[stoi(temp_line[(int)Connections::IDto])];
		}
		else if (temp_line[(int)Connections::Direction] == "d") {
			map.rooms[stoi(temp_line[(int)Connections::IDfrom])].down
				= &map.rooms[stoi(temp_line[(int)Connections::IDto])];
		}
		else if (temp_line[(int)Connections::Direction] == "nw") {
			map.rooms[stoi(temp_line[(int)Connections::IDfrom])].north_west
				= &map.rooms[stoi(temp_line[(int)Connections::IDto])];
		}
		else if (temp_line[(int)Connections::Direction] == "se") {
			map.rooms[stoi(temp_line[(int)Connections::IDfrom])].south_east
				= &map.rooms[stoi(temp_line[(int)Connections::IDto])];
		}
		temp_line.clear();
	}
	return map.rooms;
}

// Генерация монстров
Monsters* modelMonsters(Map& map, ifstream& file_locations) {

	map.monsters = new Monsters[map.total_monsters_on_map];
	int overall_monster_counter = 0;
	vector<string> temp_line;
	string line, box;

	setCarriageAtBegining(file_locations);
	while (!file_locations.eof()) {
		getline(file_locations, line);
		stringstream inflow(line);
		while (getline(inflow, box, ';')) {
			temp_line.push_back(box);
		}
		if (stoi(temp_line[(int)Locations::NumShooters]) != 0) {
			const int num_shooters = stoi(temp_line[(int)Locations::NumShooters]);
			for (int k = 0; k < num_shooters; ++k) {
				map.monsters[overall_monster_counter].type = MonsterType::Shooter;
				map.monsters[overall_monster_counter].HP = 10;
				map.monsters[overall_monster_counter].score = 1;
				map.monsters[overall_monster_counter].gives_energy_crystals = 4;
				map.monsters[overall_monster_counter].room = &map.rooms[stoi(temp_line[(int)Locations::ID])];
				map.monsters[overall_monster_counter].mark = static_cast<Mark>(k);
				++overall_monster_counter;
			}
		}
		if (stoi(temp_line[(int)Locations::NumStormtroopers]) != 0) {
			const int num_stormtrooper = stoi(temp_line[(int)Locations::NumStormtroopers]);
			for (int k = 0; k < num_stormtrooper; ++k) {
				map.monsters[overall_monster_counter].type = MonsterType::Stormtrooper;
				map.monsters[overall_monster_counter].HP = 30;
				map.monsters[overall_monster_counter].score = 1;
				map.monsters[overall_monster_counter].gives_energy_crystals = 6;
				map.monsters[overall_monster_counter].room = &map.rooms[stoi(temp_line[(int)Locations::ID])];
				map.monsters[overall_monster_counter].mark = static_cast<Mark>(k);
				++overall_monster_counter;
			}
		}
		if (stoi(temp_line[(int)Locations::NumFiretroopers]) != 0) {
			const int num_firetroopers = stoi(temp_line[(int)Locations::NumFiretroopers]);
			for (int k = 0; k < num_firetroopers; ++k) {
				map.monsters[overall_monster_counter].type = MonsterType::Firetrooper;// WARNING, WHY ???
				map.monsters[overall_monster_counter].HP = 10;
				map.monsters[overall_monster_counter].score = 1;
				map.monsters[overall_monster_counter].gives_energy_crystals = 5;
				map.monsters[overall_monster_counter].room = &map.rooms[stoi(temp_line[(int)Locations::ID])];
				map.monsters[overall_monster_counter].mark = static_cast<Mark>(k);
				++overall_monster_counter;
			}
		}
		temp_line.clear();
	}
	// Добавляем Боса Игры
	if (GAMEBOSS) {
		map.monsters[overall_monster_counter].type = MonsterType::LevelBoss;
		map.monsters[overall_monster_counter].HP = 50;
		map.monsters[overall_monster_counter].score = 35;
		map.monsters[overall_monster_counter].gives_energy_crystals = 35;
		map.monsters[overall_monster_counter].room = map.game_end_room;
		map.monsters[overall_monster_counter].mark = Mark::Black;
	}
	return map.monsters;
}

// Генерация лутбоксов
Lootboxes* modelLootboxes(Map& map, ifstream& file_locations) {
	map.lootboxes = new Lootboxes[map.total_rooms_on_map];
	int lootboxes_overall_counter = 0;
	vector<string> temp_line;
	string line, box;

	setCarriageAtBegining(file_locations);
	while (!file_locations.eof()) {
		getline(file_locations, line);
		stringstream inflow(line);
		while (getline(inflow, box, ';')) {
			temp_line.push_back(box);
		}
		if (stoi(temp_line[(int)Locations::Lootbox]) == 1) {
			map.lootboxes[lootboxes_overall_counter].type = LootboxType::medicine;
			map.lootboxes[lootboxes_overall_counter].status = Status::closed;
			map.lootboxes[lootboxes_overall_counter].location = &map.rooms[stoi(temp_line[(int)Locations::ID])];
			++lootboxes_overall_counter;
		} else if (stoi(temp_line[(int)Locations::Lootbox]) == 2) {
			map.lootboxes[lootboxes_overall_counter].type = LootboxType::ammo;
			map.lootboxes[lootboxes_overall_counter].status = Status::closed;
			map.lootboxes[lootboxes_overall_counter].location = &map.rooms[stoi(temp_line[(int)Locations::ID])];
			++lootboxes_overall_counter;
		} else if (stoi(temp_line[(int)Locations::Lootbox]) == 3) {
			map.lootboxes[lootboxes_overall_counter].type = LootboxType::microchip;
			map.lootboxes[lootboxes_overall_counter].status = Status::closed;
			map.lootboxes[lootboxes_overall_counter].location = &map.rooms[stoi(temp_line[(int)Locations::ID])];
			++lootboxes_overall_counter;
		}
		temp_line.clear();
	}
	return map.lootboxes;
}

// Генерация и отслеживание модели Главного Героя.
Hero modelHero(Map& map) {
	map.hero.HP = 100;								// HP ГГ
	map.hero.score = 0;								// кол-во очков ГГ
	map.hero.energy_crystals = 40;					// энергия ГГ
	map.hero.location = map.game_start_room;		// месторасположение ГГ

	map.hero.revolver.name = "RoboticRevolver";		// экипированое оружие ГГ
	map.hero.revolver.total_bullets = 6;			// запас патрон в наличии
	map.hero.revolver.billets_in_cylinder = 6;		// заряжено в барабан револьвера
	map.hero.revolver.max_bullets_in_cylinder = 6;	// максмум патронов, который может быть заряжен в барабан
	map.hero.revolver.damage = 5;					// уроний роботизированного револьвера

	return map.hero;
}

// Генерация и отслеживание всей игровой карты.
Map createGameMap(ifstream& file_locations, ifstream& file_connections) {
	Map map{};

	map.total_rooms_on_map = countRooms(file_locations);
	map.total_monsters_on_map = countMonsters(file_locations);
	map.rooms = modelRooms(map, file_locations, file_connections);
	map.monsters = modelMonsters(map, file_locations);
	map.lootboxes = modelLootboxes(map, file_locations);

	file_locations.close();
	file_connections.close();

	map.game_start_room = &map.rooms[0];
	map.game_end_room = &map.rooms[9];
	map.hero = modelHero(map);

	return map;
}

// Добавление маркеров монстрам
string addMonsterMark(Mark monster_mark) {
	string mark;
	if (monster_mark == Mark::Red) { mark = "красный"; }
	else if (monster_mark == Mark::Green) { mark = "зелёный"; }
	else if (monster_mark == Mark::Blue) { mark = "синий"; }
	else if (monster_mark == Mark::Black) { mark = "чёрный"; }
	else if (monster_mark == Mark::Orange) { mark = "оранжевый"; }
	else if (monster_mark == Mark::White) { mark = "белый"; }
	else if (monster_mark == Mark::Yellow) { mark = "жёлтый"; }
	else if (monster_mark == Mark::Brown) { mark = "коричневый"; }
	else if (monster_mark == Mark::Grey) { mark = "серый"; }
	else if (monster_mark == Mark::Purple) { mark = "фиолетовый"; }
	else if (monster_mark == Mark::Gold) { mark = "золотой"; }
	else if (monster_mark == Mark::Silver) { mark = "серебряный"; }
	return mark;
}

// Списиок доступных команд
enum class Commands {
	down,		// перемещение по карте вниз
	east,		// перемещение по карте на запад
	help,		// получить описание доступных команд
	inspect,	// осмотреть себя/противника/локацию
	north,		// перемещение по карте на север
	northeast,	// перемещение по карте на северо-восток
	northwest,	// перемещение по карте на северо-запад
	shoot,		// выстрелить из роботизированного револьвера
	south,		// перемещение по карте на юг
	southeast,	// перемещение по карте на юго-восток
	southwest,	// перемещение по карте на юго-запад
	unknown,	// получена неизвестная команда
	up,			// перемещение по карте вверх
	wait,		// пропуск хода ГГ
	west,		// перемещение по карте на восток
	reload,		// перезарядка револьвера
	sinthesis,	// синтезирование патронов
	lift		// поднятие аптечки, патронов или чипа
};

std::string printHelp() {
	return ("ПОМОЩЬ:\n"
		" 1. помощь(п) - выводит все доступные команды,\n"
		" 2. север(с) - двигаться на север\n"
		" 3. юг(ю) - двигаться на юг\n"
		" 4. запад(з) - двигаться на запад\n"
		" 5. восток(в) - двигаться на восток\n"
		" 6. северо-запад(сз) - двигаться на северо-запад\n"
		" 7. северо-восток(св) - двигаться на северо-восток\n"
		" 8. юго-запад(юз) - двигаться на юго-запад\n"
		" 9. юго-восток(юв) - двигаться на юго-восток\n"
		"10. вверх(вх) - двигаться вверх\n"
		"11. вниз(вн) - двигаться вниз\n"
		"12. стрелять(стр) - выстрелить из роботизированного револьвера\n"
		"\tПример: \"стрелять в синий\" или \"стрелять в синий стрелок\"\n"
		"13. ждать(ж) - ждать/пропустить ход\n"
		"14. осмотреть(о) - осмотреть себя/противника/локацию/лутбокс\n"
		"\tПример: \"осмотреться\" или \"осмотреть себя/героя\", или \"осмотреть синий стрелок\", или \"осмотреть лутбокс\"\n"
		"15. перезарядка(пр) - перезарядка револьвера\n"
		"16. синтез(снт) - синтезировать патроны или аптечку\n"
		"\tПример: \"синтезировать патроны\" или \"синтезировать аптечка\"\n"
		"17. поднять(пд) - поднять патроны или аптечку\n"
		"\tПример: \"поднять патроны\" или \"поднять аптечка\"\n");
}

// Разбиение команды игрока на составные слова
std::vector<std::string> splitUserInput(std::string& user_input) {
	std::vector<std::string> split_user_input;
	std::string word;

	stringstream inflow(user_input);
	while (getline(inflow, word, ' ')) {
		if (word != "в" || word != "на" || word != "из") {
			split_user_input.push_back(word);
		}
	}
	return split_user_input;
}

// Парсинг команд игрока
pair<Commands, std::vector<std::string>> parceUserInput(std::string& user_input) {

	std::vector<std::string> words = splitUserInput(user_input);

	Commands command{};
	std::vector<std::string> command_object{};

	if (!words.empty()) {
		if (words[0] == "север" || words[0] == "с") {
			command = Commands::north;
		}
		else if (words[0] == "юг" || words[0] == "ю") {
			command = Commands::south;
		}
		else if (words[0] == "запад" || words[0] == "з") {
			command = Commands::west;
		}
		else if (words[0] == "восток" || words[0] == "в") {
			command = Commands::east;
		}
		else if (words[0] == "северо-запад" || words[0] == "сз") {
			command = Commands::northwest;
		}
		else if (words[0] == "северо-восток" || words[0] == "св") {
			command = Commands::northeast;
		}
		else if (words[0] == "юго-запад" || words[0] == "юз") {
			command = Commands::southwest;
		}
		else if (words[0] == "юго-восток" || words[0] == "юв") {
			command = Commands::southeast;
		}
		else if (words[0] == "вверх" || words[0] == "вх") {
			command = Commands::up;
		}
		else if (words[0] == "вниз" || words[0] == "вн") {
			command = Commands::down;
		}
		else if (words[0] == "перезарядка" || words[0] == "пр") {
			command = Commands::reload;
		}
		else if (words[0] == "синтезировать" || words[0] == "снт") {
			command = Commands::sinthesis;
			if (words.size() > 1) {
				command_object = { words[1] };
			} else if (words.size() == 1) {
				command_object = {};
			}
		}
		else if (words[0] == "стрелять" || words[0] == "стр") {
			command = Commands::shoot;
			if (words.size() >= 3) {
				command_object = { words[1], words[2] };
			}
			else if (words.size() == 2) {
				command_object = { words[1] };
			}
			else if (words.size() == 1) {
				command_object = {};
			}
		}
		else if (words[0] == "ждать" || words[0] == "ж") {
			command = Commands::wait;
		}
		else if (words[0] == "осмотреть" || words[0] == "о") {
			command = Commands::inspect;
			if (words.size() >= 2) {
				command_object = { words[1] };
			}
			else if (words.size() == 1) {
				command_object = {};
			}
		}
		else if (words[0] == "помощь" || words[0] == "п") {
			command = Commands::help;
		}
		else if (words[0] == "поднять" || words[0] == "пд") {
			command = Commands::lift;
			if (words.size() >= 2) {
				command_object = { words[1] };
			}
			else if (words.size() == 1) {
				command_object = {};
			}
		}
		else {
			command = Commands::unknown;
		}
	}
	else {
		command = Commands::unknown;
	}
	return { command, command_object };
}

// Получение массива монстров в одной локации с ГГ
vector<Monsters*> getMonstersOnLocation(Map& map) {
	vector<Monsters*> monsters_in_room;
	for (size_t i = 0; i < map.total_rooms_on_map; ++i) {
		if (map.monsters[i].room == map.hero.location) {
			monsters_in_room.push_back(&map.monsters[i]);
		}
	}
	return monsters_in_room;
}

// Определяем находится ли лутбокс в локации
Lootboxes* getLootboxesOnLocation(Map& map) {
	for (size_t i = 0; i < map.total_rooms_on_map; ++i) {
		if (map.lootboxes[i].location == map.hero.location) {
			return &map.lootboxes[i];
		}
	}
	return nullptr;
}

// Печать метки монстра
std::string printMonsterMarker(Mark marker) {
	if (marker == Mark::Red) {
		return "красный";
	}
	else if (marker == Mark::Green) {
		return "зелёный";
	}
	else if (marker == Mark::Blue) {
		return "синий";
	}
	else if (marker == Mark::Black) {
		return "чёрный";
	}
	else if (marker == Mark::Orange) {
		return "оранжевый";
	}
	else if (marker == Mark::White) {
		return "белый";
	}
	else if (marker == Mark::Yellow) {
		return "жёлтый";
	}
	else if (marker == Mark::Brown) {
		return "коричневый";
	}
	else if (marker == Mark::Grey) {
		return "серый";
	}
	else if (marker == Mark::Purple) {
		return "фиолетовый";
	}
	else if (marker == Mark::Gold) {
		return "золотой";
	}
	else if (marker == Mark::Silver) {
		return "серебряный";
	}
	else {
		return "non color";
	}
}

// Печать типа монстра
std::string printMonsterType(MonsterType type) {
	if (type == MonsterType::Shooter) {
		return "стрелок";
	}
	else if (type == MonsterType::Stormtrooper) {
		return "штурмовик";
	}
	else if (type == MonsterType::Firetrooper) {
		return "огневик";
	}
	else if (type == MonsterType::LevelBoss) {
		return "босс подземелья";
	}
	else {
		return "Возвращаемый тип монстра не определён!";
	}
}

// Печать типа лутбокса
std::string printLootboxType(LootboxType type) {
	if (type == LootboxType::medicine) {
		return "аптечка";
	}
	else if (type == LootboxType::ammo) {
		return "патроны";
	}
	else if (type == LootboxType::microchip) {
		return "микрочип";
	}
	return "Error: LootboxType wrong input.";
}

int receivedDamage(vector<Monsters*> &monsters_in_room) {
	int damage = NULL;
	for (const auto& monster : monsters_in_room) {
		if (monster->type == MonsterType::Shooter) {
			damage += 1;										// Уроний Стрелка
		}
		else if (monster->type == MonsterType::Stormtrooper) {
			damage += 3;										// Уроний Штурмовика
		}
		else if (monster->type == MonsterType::Firetrooper) {
			damage += 5;										// Уроний Огневика
		}
		else if (monster->type == MonsterType::LevelBoss) {
			damage += 10;										// Уроний Финального Боса
		}
	}
	return damage;
}

// Выведение на печать всего пака монстров в локации
string printEncounteredMonsters(vector<Monsters*> monsters_in_room) {
	string monsters_description;
	for (const auto& monster : monsters_in_room) {
		if (monster != monsters_in_room.back()) {
			monsters_description += (printMonsterMarker(monster->mark) + ' ' + printMonsterType(monster->type)) + " - (" + to_string(monster->HP) + " HP)\n";
		}
		if (monster == monsters_in_room.back()) {
			monsters_description += (printMonsterMarker(monster->mark) + ' ' + printMonsterType(monster->type)) + " - (" + to_string(monster->HP) + " HP)";
		}
	}
	return monsters_description;
}

std::string game_loop(std::string& user_inpout, Map& map) {
	std::string location_info;
	std::string hero_info;
	std::string battle_info;

	// Проверяем включён ли боевой режим до получения команд - важно для обработки боевых команд!
	vector<Monsters*> monsters_in_room = getMonstersOnLocation(map);
	bool is_battle_mode = monsters_in_room.size();
	Lootboxes* lootboxes_in_room = getLootboxesOnLocation(map);
	bool is_loot_available = lootboxes_in_room;

	pair<Commands, std::vector<std::string>> command = parceUserInput(user_inpout);

	switch (command.first) {
	case (Commands::help):
		return printHelp();
	case (Commands::unknown):
		return "Неизвестная команда! Используйте команду \"помощь (п)\", чтобы посмотреть все доступные команды!";
	case (Commands::north):
		if (map.hero.location->north) {
			if (is_battle_mode == true) {
				map.hero.HP -= receivedDamage(monsters_in_room);
				hero_info += "\nБыл получен урон от монстров в " + to_string(receivedDamage(monsters_in_room)) + " HP при уклонении от боя.\n";
			}
			map.hero.location = map.hero.location->north;
			location_info += (map.hero.location->short_desc + ' ' +
							  map.hero.location->long_desc + '\n');

			monsters_in_room = getMonstersOnLocation(map);
			is_battle_mode = monsters_in_room.size();

			if (is_battle_mode == true) {
				battle_info += ("ВСТРЕЧЕН ПРОТИВНИК!\n" + printEncounteredMonsters(monsters_in_room));
			}
			
			lootboxes_in_room = getLootboxesOnLocation(map);
			is_loot_available = lootboxes_in_room;

			if (is_loot_available) {
				location_info += "На полу валяется лутбокс.\n";
			}
		}
		else {
			return "Туда не пройти!";
		} break;
	case (Commands::south):
		if (map.hero.location->south) {
			if (is_battle_mode == true) {
				map.hero.HP -= receivedDamage(monsters_in_room);
				hero_info += "\nБыл получен урон от монстров в " + to_string(receivedDamage(monsters_in_room)) + " HP при уклонении от боя.\n";
			}
			map.hero.location = map.hero.location->south;
			location_info += (map.hero.location->short_desc + ' ' + map.hero.location->long_desc) + '\n';

			monsters_in_room = getMonstersOnLocation(map);
			is_battle_mode = monsters_in_room.size();

			if (is_battle_mode == true) {
				battle_info += ("ВСТРЕЧЕН ПРОТИВНИК!\n" + printEncounteredMonsters(monsters_in_room));
			}

			lootboxes_in_room = getLootboxesOnLocation(map);
			is_loot_available = lootboxes_in_room;

			if (is_loot_available) {
				location_info += "На полу валяется лутбокс.\n";
			}
		}
		else {
			return "Туда не пройти!";
		} break;
	case (Commands::east):
		if (map.hero.location->east) {
			if (is_battle_mode == true) {
				map.hero.HP -= receivedDamage(monsters_in_room);
				hero_info += "\nБыл получен урон от монстров в " + to_string(receivedDamage(monsters_in_room)) + " HP при уклонении от боя.\n";
			}
			map.hero.location = map.hero.location->east;
			location_info += (map.hero.location->short_desc + ' ' + map.hero.location->long_desc) + '\n';

			monsters_in_room = getMonstersOnLocation(map);
			is_battle_mode = monsters_in_room.size();

			if (is_battle_mode == true) {
				battle_info += ("ВСТРЕЧЕН ПРОТИВНИК!\n" + printEncounteredMonsters(monsters_in_room));
			}

			lootboxes_in_room = getLootboxesOnLocation(map);
			is_loot_available = lootboxes_in_room;

			if (is_loot_available) {
				location_info += "На полу валяется лутбокс.\n";
			}
		}
		else {
			return "Туда не пройти!";
		} break;
	case (Commands::west):
		if (map.hero.location->west) {
			if (is_battle_mode == true) {
				map.hero.HP -= receivedDamage(monsters_in_room);
				hero_info += "\nБыл получен урон от монстров в " + to_string(receivedDamage(monsters_in_room)) + " HP при уклонении от боя.\n";
			}
			map.hero.location = map.hero.location->west;
			location_info += (map.hero.location->short_desc + ' ' + map.hero.location->long_desc) + '\n';

			monsters_in_room = getMonstersOnLocation(map);
			is_battle_mode = monsters_in_room.size();

			if (is_battle_mode == true) {
				battle_info += ("ВСТРЕЧЕН ПРОТИВНИК!\n" + printEncounteredMonsters(monsters_in_room));
			}

			lootboxes_in_room = getLootboxesOnLocation(map);
			is_loot_available = lootboxes_in_room;

			if (is_loot_available) {
				location_info += "На полу валяется лутбокс.\n";
			}
		}
		else {
			return "Туда не пройти!";
		} break;
	case (Commands::northwest):
		if (map.hero.location->north_west) {
			if (is_battle_mode == true) {
				map.hero.HP -= receivedDamage(monsters_in_room);
				hero_info += "\nБыл получен урон от монстров в " + to_string(receivedDamage(monsters_in_room)) + " HP при уклонении от боя.\n";
			}
			map.hero.location = map.hero.location->north_west;
			location_info += (map.hero.location->short_desc + ' ' + map.hero.location->long_desc) + '\n';

			monsters_in_room = getMonstersOnLocation(map);
			is_battle_mode = monsters_in_room.size();

			if (is_battle_mode == true) {
				battle_info += ("ВСТРЕЧЕН ПРОТИВНИК!\n" + printEncounteredMonsters(monsters_in_room));
			}

			lootboxes_in_room = getLootboxesOnLocation(map);
			is_loot_available = lootboxes_in_room;

			if (is_loot_available) {
				location_info += "На полу валяется лутбокс.\n";
			}
		}
		else {
			return "Туда не пройти!";
		} break;
	case (Commands::southeast):
		if (map.hero.location->south_east) {
			if (is_battle_mode == true) {
				map.hero.HP -= receivedDamage(monsters_in_room);
				hero_info += "\nБыл получен урон от монстров в " + to_string(receivedDamage(monsters_in_room)) + " HP при уклонении от боя.\n";
			}
			map.hero.location = map.hero.location->south_east;
			location_info += (map.hero.location->short_desc + ' ' + map.hero.location->long_desc) + '\n';

			monsters_in_room = getMonstersOnLocation(map);
			is_battle_mode = monsters_in_room.size();

			if (is_battle_mode == true) {
				battle_info += ("ВСТРЕЧЕН ПРОТИВНИК!\n" + printEncounteredMonsters(monsters_in_room));
			}

			lootboxes_in_room = getLootboxesOnLocation(map);
			is_loot_available = lootboxes_in_room;

			if (is_loot_available) {
				location_info += "На полу валяется лутбокс.\n";
			}
		}
		else {
			return "Туда не пройти!";
		} break;
	case (Commands::up):
		if (map.hero.location->up) {
			if (is_battle_mode == true) {
				map.hero.HP -= receivedDamage(monsters_in_room);
				hero_info += "\nБыл получен урон от монстров в " + to_string(receivedDamage(monsters_in_room)) + " HP при уклонении от боя.\n";
			}
			map.hero.location = map.hero.location->up;
			location_info += (map.hero.location->short_desc + ' ' + map.hero.location->long_desc) + '\n';

			monsters_in_room = getMonstersOnLocation(map);
			is_battle_mode = monsters_in_room.size();

			if (is_battle_mode == true) {
				battle_info += ("ВСТРЕЧЕН ПРОТИВНИК!\n" + printEncounteredMonsters(monsters_in_room));
			}

			lootboxes_in_room = getLootboxesOnLocation(map);
			is_loot_available = lootboxes_in_room;

			if (is_loot_available) {
				location_info += "На полу валяется лутбокс.\n";
			}
		}
		else {
			return "Туда не пройти!";
		} break;
	case (Commands::down):
		if (map.hero.location->down) {
			if (is_battle_mode == true) {
				map.hero.HP -= receivedDamage(monsters_in_room);
				hero_info += "\nБыл получен урон от монстров в " + to_string(receivedDamage(monsters_in_room)) + " HP при уклонении от боя.\n";
			}
			map.hero.location = map.hero.location->down;
			location_info += (map.hero.location->short_desc + ' ' + map.hero.location->long_desc) + '\n';

			monsters_in_room = getMonstersOnLocation(map);
			is_battle_mode = monsters_in_room.size();

			if (is_battle_mode == true) {
				battle_info += ("ВСТРЕЧЕН ПРОТИВНИК!\n" + printEncounteredMonsters(monsters_in_room));
			}

			lootboxes_in_room = getLootboxesOnLocation(map);
			is_loot_available = lootboxes_in_room;

			if (is_loot_available) {
				location_info += "На полу валяется лутбокс.\n";
			}
		}
		else {
			return "Туда не пройти!";
		} break;
	case (Commands::reload):
		if (map.hero.revolver.billets_in_cylinder == map.hero.revolver.max_bullets_in_cylinder) {
			return "Барабан уже заряжен полностью.";
		}
		else if (map.hero.revolver.total_bullets == 0) {
			return "Нет доступных патрон.";
		}
		else if ((map.hero.revolver.billets_in_cylinder < map.hero.revolver.max_bullets_in_cylinder) && map.hero.revolver.total_bullets > 0) {
			while ((map.hero.revolver.billets_in_cylinder < 6) && (map.hero.revolver.total_bullets > 0)) {
				++map.hero.revolver.billets_in_cylinder;
				--map.hero.revolver.total_bullets;
			}
			hero_info += "Револьвер перезаряжен! Осталось " + to_string(map.hero.revolver.total_bullets) + " патронов.";
		}
		break;
	case (Commands::sinthesis):
		if (command.second.empty()) {
			return "Отсутствует объект команды \"синтезировать\".";
		}
		else {
			// синтез патронов: 10 кристалов - 6 патрон, 15 кристалов - 1 аптечка (50 HP)
			if (command.second[0] == "патроны") {
				if (map.hero.energy_crystals < 10) {
					return "Недостаточно ресурсов для производства. В наличии " + to_string(map.hero.energy_crystals) + " кристалов.";
				} else {
					map.hero.energy_crystals -= 10;
					map.hero.revolver.total_bullets += 6;
					hero_info += "Патроны синтезированы, теперь " + to_string(map.hero.revolver.total_bullets) + " шт. "
								+ "Осталось " + to_string(map.hero.energy_crystals)+ " кристалов.";
				}
			}
			else if (command.second[0] == "аптечка") {
				if (map.hero.energy_crystals < 15) {
					return "Недостаточно ресурсов для производства. В наличии " + to_string(map.hero.energy_crystals) + " кристалов.";
				}
				else {
					map.hero.energy_crystals -= 15;
					map.hero.HP += 50; // <==================== значение лечилки надо вынести в переменную
					if (map.hero.HP >= 100) {
						map.hero.HP = 100;
					}
					hero_info += "Аптечка синтезирована и применена. Текущее здоровье " + to_string(map.hero.HP) + " HP. "
						+ "Осталось " + to_string(map.hero.energy_crystals) + " кристалов.";
				}
			}
			else {
				return "Нераспознан объект команды \"синтезировать\".";
			}
		}
		break;
	case (Commands::inspect):
		if (!command.second.empty()) {
			if (command.second[0] == "себя" || command.second[0] == "героя") {
				return ("HP " + to_string(map.hero.HP)
						+ "\nКристалов " + to_string(map.hero.energy_crystals)
						+ "\nДоступно " + to_string(map.hero.revolver.total_bullets) + " патронов."
						+ "\nЗаряжено в револьвере " + to_string(map.hero.revolver.billets_in_cylinder) + " патронов."
						+ "\nСобрано чипов " + to_string(map.hero.microchip));
			}
			else if (is_loot_available == true) {
				if (command.second[0] == "лутбокс") {
					lootboxes_in_room->status = Status::open;
					location_info += "Результат осмотра: " + printLootboxType(lootboxes_in_room->type) + ".\n";
				}
				else {
					return "Ошибка ввода цели - в списке нет подходящей.";
				}
			}
			else if (is_battle_mode == true) {
				bool check_of_presence = false;
				for (const auto& monster : monsters_in_room) {
					if (command.second[0] == printMonsterMarker(monster->mark)) {
						return "Проводим беглый осмотр: " + to_string(monster->HP) + " HP";
						check_of_presence = true;
					}
				}
				if (check_of_presence == false) {
					return "Ошибка ввода цели - в списке нет подходящей.";
				}
			}
		}
		else {
			location_info = map.hero.location->short_desc + ' ' + map.hero.location->long_desc;
			if (is_loot_available) {
				if (lootboxes_in_room->status == Status::open) {
					location_info += "\nНа полу валяется " + printLootboxType(lootboxes_in_room->type) + ".\n";
				}
				else { location_info += "\nНа полу валяется лутбокс!\n"; }
			}
			if (is_battle_mode) {
				battle_info = "В локации присутствуют монстры:\n" + printEncounteredMonsters(monsters_in_room);
			}
			return (location_info + battle_info);
		}
		break;
	case (Commands::shoot):
		if (map.hero.revolver.billets_in_cylinder == 0) {
			return "Револьвер разряжен.";
		}
		if (is_battle_mode) {
			if (command.second.empty()) {
				return "Цель не указана. Выберите противника.";
			} else {
				vector<Monsters*> appropriate_aims;
				//for (const auto& monster : monsters_in_room) {
				//	if (command.second.size() == 1) {
				//		if (command.second[0] == printMonsterMarker(monster->mark) || command.second[0] == printMonsterType(monster->type)) {
				//			++counter;
				//		}
				//	}
				//	if (counter == 1) {
				//		if (map.hero.revolver.billets_in_cylinder == 0) {
				//			return "Револьвер разряжен.";
				//		}
				//		else {
				//			location_info = "Стреляем из роботизированного револьвера!";
				//			monster->HP -= map.hero.revolver.damage;
				//			--map.hero.revolver.billets_in_cylinder;
				//			if (monster->HP <= NULL) {
				//				monster->room = nullptr;
				//				battle_info += " Monster defeated!";
				//				map.hero.score += monster->score;
				//				map.hero.energy_crystals += monster->gives_energy_crystals;
				//			}
				//		}
				//		check_of_presence = true;
				//	} else if (counter > 1) {
				//		return "too many targets!";
				//	}
				//	if (command.second.size() == 2) {
				//		if (map.hero.revolver.billets_in_cylinder == 0) {
				//			return "Револьвер разряжен.";
				//		}
				//		else {
				//			if (map.hero.revolver.billets_in_cylinder > 0) {
				//				if ((command.second[0] + ' ' + command.second[1]) == (printMonsterMarker(monster->mark) + ' ' + printMonsterType(monster->type))) {
				//					battle_info += "Стреляем из роботизированного револьвера!";
				//					monster->HP -= map.hero.revolver.damage;
				//					--map.hero.revolver.billets_in_cylinder;
				//					if (monster->HP <= NULL) {
				//						monster->room = nullptr;
				//						battle_info += " Monster defeated!";
				//						map.hero.score += monster->score;
				//						map.hero.energy_crystals += monster->gives_energy_crystals;
				//					}
				//				}
				//			}
				//		}
				//		check_of_presence = true;
				//	}
				//}
				//if (check_of_presence == false) {
				//	return "Ошибка ввода цели - в списке нет подходящей.";
				//}
				string object;
				for (const string& word : command.second) {
					object += word;
				}

				for (const auto& monster : monsters_in_room) {
					if (object == printMonsterType(monster->type) + printMonsterMarker(monster->mark) ||
						object == printMonsterMarker(monster->mark) + printMonsterType(monster->type) ||
						object == printMonsterType(monster->type) ||
						object == printMonsterMarker(monster->mark))
					{
						appropriate_aims.push_back(monster);
					}
				}

				if (appropriate_aims.empty()) {
					return "Ошибка ввода цели - в списке нет подходящей.";
				}
				else if (appropriate_aims.size() > 1) {
					return "Подходит больше чем одна цель. Требуется уточнение.";
				}
				else {
					battle_info = "Стреляем из роботизированного револьвера!\n";
					Monsters* monster = appropriate_aims.front();
					monster->HP -= map.hero.revolver.damage;
					--map.hero.revolver.billets_in_cylinder;
					if (monster->HP <= NULL) {
						monster->room = nullptr;
						battle_info += " Monster defeated!\n";
						map.hero.score += monster->score;
						map.hero.energy_crystals += monster->gives_energy_crystals;
					}
				}
			}
		}else {
			return "Поблизости нет врагов.";
		}
		break;
	case (Commands::wait):
		if (is_battle_mode == true) {
			battle_info += "Инициирован ждущий режим в бою.\n";
		} else {
			hero_info += "Инициирован ждущий режим.\n";
		}
		break;
	case (Commands::lift):
		if (command.second.empty()) {
			return "Не определён объект команды.\n";
		}
		else if (is_loot_available == false) {
			return "Вы не видите ничего полезного.\n";
		}
		else if (lootboxes_in_room->status == Status::closed) {
			return "Лутбокс ещё не осмотрен.\n";
		}
		else {
			if (command.second[0] == printLootboxType(LootboxType::medicine)) {
				map.hero.HP += 50;
				if (map.hero.HP > 100) { map.hero.HP = 100; }
				hero_info += "Аптечка подобрана и применена. Здоровье " + to_string(map.hero.HP) + ".\n";
			}
			else if (command.second[0] == printLootboxType(LootboxType::ammo)) {
				map.hero.revolver.total_bullets += 6;
				hero_info += "Подобраны патроны! Теперь доступно " + to_string(map.hero.revolver.total_bullets) + ".\n";
			}
			else if (command.second[0] == printLootboxType(LootboxType::microchip)) {
				++map.hero.microchip;
				hero_info += "Подобран микрочип! Теперь " + to_string(map.hero.microchip) + ".\n";
			}
		}
		break;
	}

	// Обработка боя. Если ГГ в конце хода остался в одной локации с монстром или ведёт бой, то он получает от них урон.
	if (is_battle_mode == true && (command.first == Commands::shoot ||
								   command.first == Commands::wait ||
								   command.first == Commands::sinthesis ||
								  (command.first == Commands::inspect && command.second[0] == "лутбокс") ||
								   command.first == Commands::lift)) {
		monsters_in_room = getMonstersOnLocation(map);
		map.hero.HP -= receivedDamage(monsters_in_room);
		battle_info += "Герой! получает урон " + to_string(receivedDamage(monsters_in_room)) + " HP от монстров, находящихся в одной локации с ним!";
	}

	std::string output =  location_info + hero_info + battle_info + "\nХод: " + to_string(++map.game_turn_counter);


	return output;
}

int main(int argc, char** argv) {
#ifdef WIN32
	setlocale(LC_ALL, "RUS");
	system("chcp 1251");
#endif
	printOutput("Привет!\n");
	ifstream file_locations("locations.txt");
	ifstream file_connections("connections.txt");
	try {
		fileOpenCheck(file_locations, file_connections);
	}
	catch (const exception& error) {
		cerr << "Error: " << error.what() << endl;
		return EXIT_FAILURE;
	}

	Map map = createGameMap(file_locations, file_connections);
	
	printOutput(map.game_start_room->short_desc + " " + map.game_start_room->long_desc + "\n");

	std::string user_inpout;
	std::string game_output;
	bool game_continue = true;

	while (game_continue) {
		printOutput("> ");
		user_inpout = readInput();
		game_output = game_loop(user_inpout, map);
		printOutput(game_output + "\n");

		if (map.hero.HP <= 0) {
			game_continue = false;
			printOutput("ГГ погиб!\n");
			printOutput("Счёт игры: " + to_string(map.hero.score));
		}
		if (map.hero.location == map.game_end_room &&
			map.hero.microchip == 0 &&
			getMonstersOnLocation(map).empty())
		{
			game_continue = false;
			printOutput("Спасаемся на вертолёте!\n");
			printOutput("Счёт игры: " + to_string(map.hero.score));
		}
	}
	cout << "\n=== G A M E  O V E R ===\n" << endl;

	delete[] map.rooms;
	delete[] map.monsters;
	delete[] map.lootboxes;

	return EXIT_SUCCESS;
}