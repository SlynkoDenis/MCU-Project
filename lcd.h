#include <avr/io.h>
#include <util/delay.h>

// RS = PC0
// E = PC2

void lcd_com(unsigned char lcd) {
	unsigned char temp;

	temp = (lcd & ~(0x01)); // RS = 0 – это команда
	PORTC = temp; // Выводим на PORTC старшую тетраду команды
	PORTC |= 0x04;
	asm("nop"); // Небольшая задержка в 1 такт МК, для стабилизации
	PORTC &= ~(0x04); // Сигнал записи команды

	_delay_loop_2(1000);

	temp = ((lcd << 4) & ~(0x01)); // RS = 0 – это команда
	PORTC = temp; // Выводим на PORTC младшую тетраду команды, сигналы RS, E
	PORTC |= 0x04;
	asm("nop"); // Небольшая задержка в 1 такт МК, для стабилизации
	PORTC &= ~(0x04); // Сигнал записи команды

	_delay_loop_2(1000); // Пауза для выполнения команды
}

void lcd_dat(unsigned char lcd) {
	unsigned char temp;

	temp = (lcd | 0x01); // RS = 1 – это данные
	PORTC = temp; // Выводим на PORTC старшую тетраду данных, сигналы RS, E
	PORTC |= 0x04;
	asm("nop"); // Небольшая задержка в 1 такт МК, для стабилизации
	PORTC &= ~(0x04); // Сигнал записи данных

	_delay_loop_2(1000);

	temp = ((lcd << 4) | 0x01); // RS = 1 – это данные
	PORTC = temp;	// Выводим на PORTC младшую тетраду данных, сигналы RS, E
	PORTC |= 0x04;
   	asm("nop");	// Небольшая задержка в 1 такт МК, для стабилизации
  	PORTC &= ~(0x04);	// Сигнал записи данных

  	_delay_loop_2(1000);	// Пауза для вывода данных
}

void lcd_init(void) {
	lcd_com(0b00100100);	// 4 разряда шины, одна строка, матрица 5 х 10
	_delay_loop_2(10000);

 	lcd_com(0x0c);		// Показать изображение, курсор не показывать
 	_delay_loop_2(10000);
}

void write_temperature(unsigned int t) {
	unsigned char dec = t / 10;

	lcd_com(0x01);

	lcd_dat(0x30 + dec);

	lcd_dat(0x30 + (t - 10 * dec));
}
