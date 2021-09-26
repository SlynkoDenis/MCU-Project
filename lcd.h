#include <avr/io.h>
#include <util/delay.h>


// RS = PC0
// E = PC2

void lcd_command(unsigned char lcd) {
	unsigned char temp;

	// Set RS bit to 0 for command
	temp = (lcd & ~(0x01));
	// Move command's high 4 bits to PORTC
	PORTC = temp;
	PORTC |= 0x04;
	// Stabilization delay
	asm("nop");
	// Write signal
	PORTC &= ~(0x04);
	// Delay for command execution
	_delay_loop_2(1000);

	// Set RS bit to 0 means command
	temp = ((lcd << 4) & ~(0x01));
	// Move command's low 4 bits, signals RS and E to PORTC
	PORTC = temp;
	PORTC |= 0x04;
	asm("nop");
	PORTC &= ~(0x04);
	_delay_loop_2(1000);
}

void lcd_data(unsigned char lcd) {
	unsigned char temp;

	// Set RS bit to 1 for data
	temp = (lcd | 0x01);
	// Move command's high 4 bits, signals RS and E to PORTC
	PORTC = temp;
	PORTC |= 0x04;
	// Stabilization delay
	asm("nop");
	// Write signal
	PORTC &= ~(0x04);
	// Delay for command execution
	_delay_loop_2(1000);

	// Set RS bit to 1 means data
	temp = ((lcd << 4) | 0x01);
	// Move command's low 4 bits, signals RS and E to PORTC
	PORTC = temp;
	PORTC |= 0x04;
   	asm("nop");
  	PORTC &= ~(0x04);
  	_delay_loop_2(1000);
}

void lcd_init(void) {
	// 4 bus discharges, one line, matrix 5 x 10
	lcd_command(0b00100100);
	_delay_loop_2(10000);

	// Show image without cursor
 	lcd_command(0x0c);
 	_delay_loop_2(10000);
}

void write_temperature(unsigned int t) {
	unsigned char dec = t / 10;

	lcd_command(0x01);

	lcd_data(0x30 + dec);

	lcd_data(0x30 + (t - 10 * dec));
}
