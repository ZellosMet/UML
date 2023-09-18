#include<iostream>
#include<thread> //Заголовок для работы с потоками
using namespace std::chrono_literals; //попеременный перехвад потоков

bool finish = false;

void Plus()
{
	while (!finish)
	{
		std::cout << "+ ";
		std::this_thread::sleep_for(1s);
	}
}
void Minus()
{
	while (!finish)
	{
		std::cout << "- ";
		std::this_thread::sleep_for(1s);
	}
}


void main()
{
	setlocale(LC_ALL, "");
	//Plus();
	//Minus();
	std::thread plus_thread = std::thread(Plus);
	std::thread minus_thread = std::thread(Minus);
	std::cin.get(); //Функция get() ожидает нажатие Enter
	finish = true;
	if(minus_thread.joinable()) minus_thread.join(); // Синхронизация потоков и проверка на возможность синхронизации
	if(plus_thread.joinable()) plus_thread.join(); 
	
}