#include<iostream>
#include<conio.h>
#include<thread>
#include<Windows.h>
using namespace  std::chrono_literals;

#define Enter 13
#define Escape 27


#define MIN_TANK_VOLUME 20
#define MAX_TANK_VOLUME 120

class Tank
{
	const int VOLUME;
	double fuel_level;

public:

int get_VOLUME()const
	{
		return VOLUME;
	}
double get_fuel_level()const
	{
		return fuel_level;
	}
void fill(double fuel)
	{
		if (fuel_level<0) return;
		if (fuel_level + fuel < VOLUME) fuel_level += fuel;
		else fuel_level = VOLUME;
	}
double give_fuel(double amount)
{
	fuel_level -= amount;
	if (fuel_level < 0)fuel_level = 0;
	return fuel_level;
}

Tank(int volume)
		: VOLUME
		(
			volume<MIN_TANK_VOLUME?MIN_TANK_VOLUME:
			volume>MAX_TANK_VOLUME? MAX_TANK_VOLUME:
			volume
		)
	{
		this->fuel_level = 0;
		std::cout << "Tank is ready\t" << this << std::endl;
	}
~Tank()
	{
		std::cout << "Tank is over\t" << this << std::endl;
	}

void info()const
	{
		std::cout << "Volume:\t" << VOLUME << " liters\n";
		std::cout << "Fuel level:\t" << get_fuel_level() << " liters\n";
	}
};

#define MIN_ENGINE_CONSUMPTION 3
#define MAX_ENGINE_CONSUMPTION 30

class Engine
{
	const double DEFAULT_CONSUMPTION;
	const double DEFAULT_CONSUMPTION_PER_SECOND;
	double consumption_per_second;
	bool is_started;

public:
	double get_DEFAULT_CONSUMPTION_PER_SECOND()const
	{
		return DEFAULT_CONSUMPTION_PER_SECOND;
	}
	double get_consumption_per_second()const
	{
		return consumption_per_second;
	}
	void set_consumption_per_second(int speed)
	{
		if (speed == 0)consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND;
		else if (speed < 60) consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 20/3;
		else if (speed < 100) consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 14/3;
		else if (speed < 140) consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 20/3;
		else if (speed < 200) consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 25/3;
		else if (speed < 250) consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 10;		
	}

	void start()
	{
		is_started = true;
	}
	void stop()
	{
		is_started = false;
	}
	bool started()const
	{
		return is_started;
	}

	Engine(double consumption) : DEFAULT_CONSUMPTION_PER_SECOND(DEFAULT_CONSUMPTION * 3e-5),
		DEFAULT_CONSUMPTION
		(
		consumption < MIN_ENGINE_CONSUMPTION ? MIN_ENGINE_CONSUMPTION :
		consumption > MAX_ENGINE_CONSUMPTION ? MAX_ENGINE_CONSUMPTION :
		consumption
		)
	{	
		set_consumption_per_second(0);
		is_started = false;
		std::cout << "Engine is ready:\t" << this << std::endl;
	}
	~Engine()
	{
		std::cout << "Engine is over:\t" << this << std::endl;
	}

	void info()const
	{
		std::cout << "Consumption:\t" << DEFAULT_CONSUMPTION << " liters per 100km.\n";
		std::cout << "Consumption:\t" << consumption_per_second << " liters per second.\n";
	}
};

#define MAX_SPPED_LOWER_LEVEL 30
#define MAX_SPPED_UPPER_LEVEL 400

class Car
{
	Engine engin;
	Tank tank;
	int speed;
	int accelleration;
	const int MAX_SPEED;
	bool driver_inside;
	struct Threads
	{
		std::thread panel_thread;
		std::thread engine_idle_thread;
		std::thread free_wheeling_thread;
	}threads;

public:

	int get_speed()const
	{
		return speed;
	}
	int get_MAX_SPEED()const
	{
		return MAX_SPEED;
	}

	Car(double consumption, int volume, int max_speed, int acceleration = 10):engin(consumption), tank(volume),
		MAX_SPEED
		(
			max_speed < MAX_SPPED_LOWER_LEVEL ? MAX_SPPED_LOWER_LEVEL:
			max_speed > MAX_SPPED_UPPER_LEVEL ? MAX_SPPED_UPPER_LEVEL:
			max_speed
		)
	{
		driver_inside = false;
		this->accelleration = acceleration;
		this->speed = 0;
		std::cout << "Your car is ready to go " << this << std::endl;
	}

	~Car()
	{
		std::cout << "Car is over\t" << this << std::endl;
	}

	void get_in()
	{
		driver_inside = true;
		threads.panel_thread = std::thread(&Car::panel, this);
	}

	void get_out()
	{
		driver_inside = false;
		if (threads.panel_thread.joinable()) threads.panel_thread.join();
		system("CLS");
		std::cout << "Outside" << std::endl;
	}

	void start()
	{
		if (driver_inside && tank.get_fuel_level())
		{
			engin.start();
			threads.engine_idle_thread = std::thread(&Car::engine_idle, this);
		}
	}

	void stop()
	{
		engin.stop();
		if (threads.engine_idle_thread.joinable())threads.engine_idle_thread.join();
	}

	void control()
	{
		char key = 0;
		do
		{
			key = 0;
			if(_kbhit()) //Функция _kbhit() возвращает ненулевое значение пока клавиша не нажата
			key = _getch();
			switch(key)
			{
			case Enter:				
				if (driver_inside && speed == 0) get_out();
				else if(!driver_inside && speed == 0) get_in();			
				break;

			case 'F': case 'f':	//Заправить машину
				if (driver_inside)
				{
					std::cout << "Для начала нужно выйти из машины" << std::endl;
					break;
				}
				double fuel;
				std::cout << "Введите объём топлива: "; std::cin >> fuel;
				std::this_thread::sleep_for(2s);
				tank.fill(fuel);
				break;

			case 'I': case 'i': //Завести двигатель
				if (engin.started()) stop();
				else start();
				break;

			case 'W': case 'w':
				accellerate(); 
				break;

			case 'S': case 's':
				slow_down();
				break;

			case Escape:
				speed = 0;
				stop();
				get_out();
				break;
			}
			if (tank.get_fuel_level() == 0) stop();
			if (speed <= 0)engin.set_consumption_per_second (speed = 0);
			if (speed == 0 && threads.free_wheeling_thread.joinable())threads.free_wheeling_thread.join();
		} while (key != Escape);
	}

	void engine_idle()
	{
		while (engin.started() && tank.give_fuel(engin.get_consumption_per_second()))
			std::this_thread::sleep_for(1s);
	}

	void free_wheeling()
	{
		while (--speed > 0)
		{
			std::this_thread::sleep_for(1s);
			engin.set_consumption_per_second(speed);
		}
	}

	void accellerate()
	{
		if (engin.started() && driver_inside)
		{
			speed += accelleration;
			if (speed > MAX_SPEED) speed = MAX_SPEED;
			if (!threads.free_wheeling_thread.joinable()) threads.free_wheeling_thread = std::thread(&Car::free_wheeling, this);
			std::this_thread::sleep_for(1s);
		}
	}

	void slow_down()
	{
		if (driver_inside)
		{
			speed -= accelleration;
			if (speed < 0) speed = 0;
			std::this_thread::sleep_for(1s);
		}
	}

	void panel()
	{	
		while (driver_inside)
		{
			system("CLS");
			for (int i = 0; i < speed/3; i++)
			{
				std::cout << "|";
			}
			std::cout << std::endl;
			std::cout << "Fuel level:\t" << tank.get_fuel_level() << " liters";
			if (tank.get_fuel_level() < 5)
			{
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hConsole, 0xCE);
				std::cout << " LOW FUEL! ";
				SetConsoleTextAttribute(hConsole, 0x07);
			}
			std::cout << "\nEngine is " << (engin.started() ? "started" : "stoped") << std::endl;
			std::cout << "Speed:\t" << speed << " km/h\n";
			std::cout << "Consumption per second:\t" << engin.get_consumption_per_second() << " liters\n";
			std::this_thread::sleep_for(100ms);
		}
	}
};

//#define TANK_CHEK
//#define ENGINE_CHEK

void main()
{
	setlocale(LC_ALL, "");

#ifdef TANK_CHEK

	Tank tank(150);
	tank.info();

	do
	{
		int fuel;
		std::cout << "Введите уровень топлива: "; std::cin >> fuel;
		tank.fill(fuel);
		tank.info();
	} while (true);

#endif 

#ifdef ENGINE_CHEK

	Engine engine(10);
	engine.info();

#endif
	std::cout << "Введите объём бака: ";
	Car bmw(25, 100, 300);
	bmw.control();

}