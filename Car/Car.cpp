#include<iostream>
#include<conio.h>
#include<thread>
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
	double consumption;
	double consumption_per_second;
	bool is_started;

public:
	double get_consumption()const
	{
		return consumption;
	}
	double get_consumption_per_second()const
	{
		return consumption_per_second;
	}
	double set_consumption(double consumption)
	{
		this->consumption = consumption;
	}
	void set_consumption_per_second(int consumption)
	{
		consumption_per_second = consumption*3e-5;
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

	Engine(double consumption) 
		: DEFAULT_CONSUMPTION
		(
		consumption < MIN_ENGINE_CONSUMPTION ? MIN_ENGINE_CONSUMPTION :
		consumption > MAX_ENGINE_CONSUMPTION ? MAX_ENGINE_CONSUMPTION :
		consumption
		)
	{
		this->consumption = DEFAULT_CONSUMPTION;
		set_consumption_per_second(consumption);
		
		is_started = false;
		std::cout << "Engine is ready:\t" << this << std::endl;
	}
	~Engine()
	{
		std::cout << "Engine is over:\t" << this << std::endl;
	}

	void info()const
	{
		std::cout << "Consumption:\t" << consumption << " liters per 100km.\n";
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
	const int MAX_SPEED;
	bool driver_inside;
	bool parking;
	struct Threads
	{
		std::thread panel_thread;
		std::thread engine_idle_thread;
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
	bool get_parking()const
	{
		return parking;
	}

	Car(double consumption, int volume, int max_speed):engin(consumption), tank(volume), speed(0),
		MAX_SPEED
		(
			max_speed < MAX_SPPED_LOWER_LEVEL ? MAX_SPPED_LOWER_LEVEL:
			max_speed > MAX_SPPED_UPPER_LEVEL ? MAX_SPPED_UPPER_LEVEL:
			max_speed
		)
	{
		parking = false;
		driver_inside = false;
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
		speed = 0;
	}

	void drive()
	{
		parking = true;
	}

	void park()
	{
		parking = false;
		speed = 0;
	}

	void speed_increase()
	{
		speed += 10;
		if (speed > MAX_SPEED) speed = MAX_SPEED;
	}
	void speed_decrease()
	{
		speed -= 10;
		if (speed < 0) speed = 0;
	}

	double drive_consumption(int speed)
	{
		if (speed == 0) return engin.get_consumption_per_second();
		if (speed >= 1 && speed < 60) return 200e-5;
		if (speed > 61 && speed < 100) return 140e-5;
		if (speed > 101 && speed < 140) return 200e-5;
		if (speed > 141 && speed < 200) return 250e-5;
		if (speed > 201 && speed <= 250) return 300e-5;
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
				if (driver_inside) get_out();
				else get_in();			
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

			case 'P': case 'p':
				if (!get_parking() && engin.started()) drive();
				else park();
				break;

			case 'W': case 'w':
				if (get_parking() && engin.started()) speed_increase();
				else park();
				break;

			case 'S': case 's':
				if (get_parking() && engin.started()) speed_decrease();
				break;

			case Escape:
				get_out();
				break;
			}

			if (tank.get_fuel_level() == 0) stop();
		} while (key != Escape);
	}

	void engine_idle()
	{
		while (engin.started() && tank.give_fuel(drive_consumption(speed)))
		{
			std::this_thread::sleep_for(1s);
		}
	}

	void panel()
	{	
		while (driver_inside)
		{
			system("CLS");
			std::cout << "Speed:\t" << get_speed() << " km/h\n";
			std::cout << "Fuel level:\t" << tank.get_fuel_level() << " liters\n";
			std::cout << "LOW FUEL!" << std::endl;
			std::cout << (get_parking() ? "Not parked" : "Parked") << std::endl;
			std::cout << "Engine is " << (engin.started() ? "started" : "stoped") << std::endl;
			std::this_thread::sleep_for(1s);
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

	Car bmw(10, 40, 250);
	bmw.control();

}