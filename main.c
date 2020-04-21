#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>

#include "lcd.h"

unsigned int temperature = 18;
unsigned char status = 0;

unsigned int check_status() {
    if (status == 1) {
        if (temperature != 0) {
            --temperature;
            status = 0;

            write_temperature(temperature);

            return 1;
        }
    } else if (status == 2) {
        ++temperature;
        status = 0;

        write_temperature(temperature);

        return 2;
    } else if (status == 3) {
        status = 0;
	
	lcd_com(0x01);
        lcd_dat('S');
	lcd_dat('t');
	lcd_dat('a');
	lcd_dat('r');
	lcd_dat('t');

        return 3;
    }

    return 0;
}

unsigned int ADC_convert() {
    unsigned int r;

    ADCSRA |= (1 << ADSC);

    while ((ADCSRA & _BV(ADIF)) == 0x00)
        ;

    r = (ADCL | ADCH << 8);
    return r;
}

ISR(TIMER0_OVF_vect) {
    if ((PIND & 0x10) == 0x00) {
        _delay_loop_2(200000);
        if ((PIND & 0x10) == 0x00) {
            while ((PIND & 0x10) == 0x00)
                ;
	    status = 1;
        }
    } else if ((PIND & 0x20) == 0x00) {
        _delay_loop_2(200000);
        if ((PIND & 0x20) == 0x00) {
            while ((PIND & 0x20) == 0x00)
                ;
            status = 2;
        }
    } else if ((PIND & 0x40) == 0x00) {
        _delay_loop_2(200000);
        if ((PIND & 0x40) == 0x00) {
            while ((PIND & 0x40) == 0x00)
                ;
            status = 3;
        }
    }

    TCNT0 = 0x00;
    TIFR = 0x00;
    return;
}

unsigned int check_tempterature() {
    float v = (float)ADC_convert() * 5.0f / 1024.0f;
    float t = 1.0f / (log(v / 2.5f) / 4300.0f + 1.0f / 298.0f) - 273.0f;

    if (t + 1.0 <= (float)temperature)
        return 1;
    else
        return 0;
}

unsigned int check_sound() {
    if (status == 3) {
    	status = 0;
	return 1;
    } else
	status = 0;

    return 0;
}

void sound() {
    while (1) {
        unsigned int i;
        for (i = 0; i < 100; ++i) {
            PORTD |= 0x01;
            _delay_loop_2(1500);
            PORTD &= ~(0x01);
            _delay_loop_2(1500);
        }

        cli();

        if (check_sound() == 1) {	// Если кнопку нажали
            sei();
	    break;			// Выключение динамика
	}

        sei();
    }
}

int main() {
    DDRC = 0b11110101;

    _delay_loop_2(2000000);

    PORTC = 0x20;
    PORTC |= 0b00000100;
    asm("nop");
    PORTC &= 0b11111011;

    _delay_loop_2(10000);

    lcd_init();

    DDRD = 0x03;
    PORTD = 0x70;

    ADCSRA = (1 << ADEN) | (1 << ADPS2);
    ADMUX = (0 << MUX0) | (0 << MUX1) | (0 << MUX2);

    TIMSK = (1 << TOIE0);
    TCCR0 = (1 << CS00);  

    while(1) {
	write_temperature(temperature);		// Изначальная температура

    	sei();	

    	while (1) {	// Цикл настройки температуры
		_delay_loop_2(10000);

		cli();

        	unsigned int r = check_status();
        	if (r == 3)
            		break;

        	sei();
    	}

    	sei();

    	PORTD |= 0x02;	// Загорание светодиода при старте 

    	while (1) {	// Цикл вычислений температуры; ожидание достижения порога температуры
		_delay_loop_2(10000);
	    
		cli();

        	unsigned int r = check_tempterature();
        	if (r != 0) {
	    		sei();

            		sound();
            		break;
       		}

		sei();
    	}

    	PORTD = 0x70;	// Выключение светодиода
    }

    return 0;
}
