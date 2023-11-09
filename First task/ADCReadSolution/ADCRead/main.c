#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define MYUBRR F_CPU/16/BAUD_RATE-1
#define ADC_READ_PIN 2
#define FLASH_ADD_PERIOD 3
#define MAX_FLASH_ADDR 5

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

uint16_t seconds = 0;
uint16_t adc_result;

uint8_t add_count = 0;
uint16_t flash[10];

/* UART section */

void uart_init(unsigned int ubrr) {
	// Set baud rate for uart
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	// Enable receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	// Set frame format: 8 data, 1 stop bit
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

unsigned char uart_receive(void) {
	// Wait for data to be received
	while (!(UCSR0A & (1<<RXC0)));
	// Get and return received data from buffer
	return UDR0;
}

void uart_transmit(unsigned char data) {
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1<<UDRE0)));
	// Put data into buffer, sends the data
	UDR0 = data;
}

void uart_sendString(char* string_ptr) {
	while (*string_ptr != 0x00) {
		uart_transmit(*string_ptr);
		string_ptr++;
	}
}

void uart_send_int(uint16_t adc_result) {
	char buffer[10];
	sprintf(buffer, "%u", adc_result);
	uart_sendString(buffer);
}

void newline() {
	uart_sendString((char *)"\r\n");
}

/* UART section */

/* ADC section */

void ADC_init(void) {
	// Select AVcc as the reference voltage
	ADMUX |= (1 << REFS0);
	
	// Set the prescaler to 128 and enable the ADC
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);
}

uint16_t ADC_read(uint8_t channel) {
	// Select the corresponding channel 0-7
	// And ensure the reference voltage remains AVcc
	ADMUX = (ADMUX & 0xF8) | channel;

	// Start the conversion
	ADCSRA |= (1 << ADSC);

	// Wait for the conversion to finish
	while (ADCSRA & (1 << ADSC));

	// Combine the low and high bytes
	uint16_t adc_result;
	adc_result = ADCL;
	adc_result |= (ADCH << 8);
	return adc_result;
	return ADC;  // On the ATmega328P, the ADC data register is a 16-bit register
}

/* ADC section */

/* Flash operations */

void flash_write(uint16_t data, uint8_t* addr) {
	flash[*addr] = data;
	(*addr)++;
}

void flash_clear(uint8_t* addr) {
	for (uint8_t i = 0; i < add_count; i++) {
		flash[i] = 0x00;
	}
	(*addr) = 0;
}

void print_flash_data() {
	uart_sendString((char *)"flash: ");
	for (int i = 0; i < add_count; i++) {
		uart_send_int(flash[i]);
		if (i != add_count - 1) {
			uart_transmit(',');
		}
	}
	newline();
}


/* Flash operations */


/* Timer section */

void timer_init() {
	TCCR1B |= (1 << WGM12); // Mode 4, CTC on OCR1A
	TIMSK1 |= (1 << OCIE1A); //Set interrupt on compare match
	OCR1A = 15624; // Set OCR1A for 1-second interval
	TCCR1B |= (1 << CS12) | (1 << CS10); // Set prescaler to 1024 and start the timer
}

ISR(TIMER1_COMPA_vect) {
	// ?????????? ?????? ???????
	seconds++;
	
	adc_result = ADC_read(ADC_READ_PIN);
	uart_send_int(adc_result);
	newline();
	
	if (seconds == FLASH_ADD_PERIOD) {
		flash_write(adc_result, &add_count);
		print_flash_data();
		seconds = 0;
		if (add_count == MAX_FLASH_ADDR) {
			flash_clear(&add_count);
			uart_sendString((char *)"Flash cleared");
		}
	}
}

/* Timer section */

int main(void) {
	uart_init(MYUBRR);
	ADC_init();
	timer_init();
	sei();
	
	while (1) {
		// Interrupts handling everything
	}
}
