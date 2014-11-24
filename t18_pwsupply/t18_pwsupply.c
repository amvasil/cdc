/*
 * t18_pwsupply.c
 *
 * Created: 16.11.2013 13:10:21
 *  Author: 
 вторая нога к приемнику USB из МК
 */ 


#define POWER_SUPPLY_4 //definition of supply number for this version of firmware


#define F_CPU 16000000L
#define BAUD 9600
#define pwm OCR1B
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
typedef unsigned char byte;

byte data[800];  //array for measurement data, 10 bits consequently 650
byte write_data; //data to be send by uart
unsigned int data_it = 0;  //iterator in data[]
byte shift = 0;  //internal variable responsible for 10bit adc data to 8bit array conversion
byte rx_state = 0; //state of uart receiver
byte tx_state = 0; //state of uart transmitter
unsigned int measurements = 1024; //number of measurements
unsigned int goal_voltage = 512;  //pwm code
unsigned int voltage = 512; //voltage 10bit code sent from BBB
int voltage_error = 0; //internal calibration variable, stands for (Ugoal-Ureal)->10bit_code
unsigned int max_current = 1024; //fuse value, 10bit code
unsigned int adc_data = 0; //adc 10bit data code
byte adc_flag = 1; //flag is set when adc is ready
byte usart_data = 0; //internal rx variable for received data
byte general_state = 0; //state of power supply
byte to_send=0; //flag for tx to send custom message
unsigned int n_block_send = 0; //internal variable of transmitter, number of blocks 5*8bit sent
byte n_byte_send = 0; //internal variable of transmitter, number of byte sent in the block
unsigned int bytes_to_write = 1280; //amount of bytes which are gonna be written in measuremets
byte ready = 0; //reply to BBB
byte on_init = 1; //flag is only set at initialization of supply
byte prescaler=1; //prescaler value, set by BBB
byte count_scale=0; //variable for prescaling, increments after every adc cycle
byte voltage_code_1 = 0; //high byte of voltage sent by BBB
byte voltage_code_2 = 0; //low byte of voltage sent by BBB
byte fuse_code_1 = 0; //high byte of fuse sent by BBB
byte fuse_code_2 = 0; //low byte of fuse sent by BBB
byte calibration_state = 0; //internal state of calibration algorithm
unsigned int own_current = 0; //zero-current of power supply
byte calibr_integr_count = 0; //iterator for 16 measurement mean value, calibration
unsigned int calibr_sum = 0; //sum of measures, calibration
byte led_blink=0; //state of led in calibration
byte measure_finished=0; //flag indicating measures are finished and ready to send
unsigned int max_adc_value=0; //maximum value measured
byte is_twenty = 0;
unsigned int own_current_working = 0; //own current, divided by 10 if needed(2 ohms on)
byte address_state=0; //0 if waiting for address, 1 if address is this supply

/*
*  timer which starts adc interrupt vector
*/
ISR(TIMER2_COMP_vect)
{
	ADCSRA |= (1<<ADSC);
}

/*
* UART RX interrupt vector
*/
ISR(USART_RXC_vect)
{
	usart_data = UDR;
	if(address_state == 0)
	{
		#ifdef POWER_SUPPLY_1
			if(usart_data == 0x01 || usart_data == 0xFF)
			{
				address_state=1;
			}
		#endif
		#ifdef POWER_SUPPLY_2
		if(usart_data == 0x02 || usart_data == 0xFF)
		{
			address_state=1;
		}
		#endif
		#ifdef POWER_SUPPLY_3
		if(usart_data == 0x03 || usart_data == 0xFF)
		{
			address_state=1;
		}
		#endif
		#ifdef POWER_SUPPLY_4
		if(usart_data == 0x04 || usart_data == 0xFF)
		{
			address_state=1;
		}
		#endif
		return;
	}
	
	switch(rx_state)
	{ 
		case 0:  //default state
			switch(usart_data)
			{
			case 0x11: //to send goal voltage
				rx_state = 1;
				ready &= ~(1<<3);
				break;
			case 0x12: //to send current limit
				rx_state = 3;
				break;
			case 0x15: //to send measurements count
				rx_state = 5;
				break;
			case 0x1C: //auto calibrate
				
				if(ready >= 7)
				{
					PORTB &= ~(1<<1); //turn off red led
					PORTB &= ~(1<<0);  //turn off relay and green led
					calibration_state = 0;
					general_state = 10;	
					PORTC |= (1<<4); //turn on 20 Ohms
					_delay_ms(10);
				}	
				address_state = 0;			
				break;
			case 0x14: //to send prescaler
				if(general_state != 3)
				{
					rx_state = 7;
				}					
				break;
			case 0x1F: //power on with 2 Ohm
				to_send = ready;
				tx_state = 5;
				if(ready == 15)
					{		
						    is_twenty = 0;	
							own_current_working = own_current / 10;		
							general_state = 1;
							PORTB &= ~(1<<1); //turn off red led
							PORTB |= (1<<0); //turn relay on
							_delay_ms(10);
							pwm = goal_voltage;
							TCCR1B |= (1<<CS10);
							TCNT2 = 0;
							TCCR2 |= (1<<CS21);
									
					}	
				address_state=0;		
				break;
			case 0x1B: //power on with 20 Ohm
			to_send = ready;
			tx_state = 5;
			if(ready == 15)
			{
				
				is_twenty = 1;
				own_current_working = own_current;
				PORTC |= (1<<4);
				PORTB &= ~(1<<1); //turn off red led
				PORTB |= (1<<0); //turn relay on
				_delay_ms(10);
				general_state = 1;
				pwm = goal_voltage;
				TCCR1B |= (1<<CS10);
				TCNT2 = 0;
				TCCR2 |= (1<<CS21);
						
				
			}
			address_state = 0;
			break;
		    case 0x20: //measure
				if(general_state == 1)
				{
					general_state = 3;
					max_adc_value = 0;
					data[0] = 0;
					TCNT2 = 0;
					measure_finished = 0;
				}
				else
				{
					tx_state = 5;
					to_send = 0xFF;
				}
				address_state = 0;
				break;
			case 0x21: //transmit
				if(measure_finished)
				{
					tx_state = 6;
					UCSRB |= (1<<UDRIE);
				}
				address_state = 0;
				break;
			case 0x3A: //get max measured level result
				if(general_state==2)
				{
					tx_state=5;
					to_send = 0x01;
				}
				else
				if(!measure_finished || !(general_state==1))
				{
					tx_state=5;
					to_send = 0x0F;
				}
				else
				{
					if(max_adc_value < 102)
					{
						to_send = 0xCB;
					}
					else
					{
						to_send = 0xCA;
					}			
				tx_state = 5;
				//UDR = to_send;
				//to_send = 0;
				//tx_state = 1;
				}
				address_state = 0;
				break;
			case 0xF0: //power off
				general_state = 0;
				pwm = 0;
				PORTB &= ~(1<<1); //turn off red led
				PORTB &= ~(1<<0); //turn off relay
				PORTC &= ~(1<<4); //turn off 20 Ohms
				address_state = 0;
				break;
			default:
				address_state = 0;
				break;
			}
			
			break;
		case 1:  //waiting for 1 byte of goal voltage
			voltage_code_1 = usart_data;
			rx_state = 2;
			break;
		case 2: //waiting for 2 byte of goal voltage
			voltage_code_2 = usart_data;
			
			#ifdef POWER_SUPPLY_1
				voltage = (unsigned int)(((double)voltage_code_1+((double)voltage_code_2)/100.0)*112.4013652-2.352265703); //for ps C1
				goal_voltage = (unsigned int)(((double)voltage_code_1+((double)voltage_code_2)/100.0))*103.7775074-134.5245956; //for ps 1
			#endif
			#ifdef POWER_SUPPLY_2
				voltage = (unsigned int)(((double)voltage_code_1+((double)voltage_code_2)/100.0)*112.7102795-4.38328478); //for ps D2
				goal_voltage = (unsigned int)(((double)voltage_code_1+((double)voltage_code_2)/100.0))*104.3838888-135.6199428; //for ps 2
            #endif
			#ifdef POWER_SUPPLY_3
				voltage = (unsigned int)(((double)voltage_code_1+((double)voltage_code_2)/100.0)*50.22845631+1.335773598); //for ps A3
				goal_voltage = (unsigned int)(((double)voltage_code_1+((double)voltage_code_2)/100.0))*94.47206362-871.3258653; //for ps 3
			#endif
			#ifdef POWER_SUPPLY_4
				voltage = (unsigned int)(((double)voltage_code_1+((double)voltage_code_2)/100.0)*49.77125437+2.027702415); //for ps B4
				goal_voltage = (unsigned int)(((double)voltage_code_1+((double)voltage_code_2)/100.0))*93.19883278-861.5091168; //for ps 4
			#endif
			
			ready |= 1;
			rx_state = 0;
			address_state = 0;
			break;
		case 3: //waiting for 1 byte of current limit
			fuse_code_1 = usart_data;
			rx_state = 4;
			break;
		case 4: //waiting for 2 byte of current limit
			fuse_code_2 = usart_data;
			max_current = (((unsigned int)fuse_code_1)<<8)|fuse_code_2;
			if(max_current>1023)
				max_current = 1023;
			ready |= 2;
			rx_state = 0;
			address_state = 0;
			break;
		case 5: //waiting for 1 byte of measurements count
			measurements = ((unsigned int)usart_data)<<8;
			rx_state = 6;
			break;
		case 6: //waiting for 2 byte of measurements count
			measurements |= usart_data;
			if(measurements>640)
				measurements = 640;
			if(measurements % 4 == 0)
			{
				bytes_to_write = measurements/4*5;
			}
			else
			{
				bytes_to_write = (measurements+1)/4*5; 
			}
			rx_state = 0;
			address_state = 0;
			ready |= 4;
			break;
		case 7: //prescaler setting
			prescaler = usart_data;
			rx_state = 0;
			address_state = 0;
			break;
		default:
			break;
	}
}

/*
* UART TX ready interrupt vector
*/
ISR(USART_UDRE_vect)
{
	switch(tx_state)
	{
		case 0:
			UDR = 0x10;
			tx_state = 1;
			break;
		case 1:
			tx_state = 2;
			UCSRB &= ~(1<<UDRIE);
			break;
		case 3:
			UDR = (unsigned char)(goal_voltage>>2);
			tx_state = 4;
			break;
		case 4:
			UDR = (unsigned char)(goal_voltage % 4);
			tx_state = 1;
			break;
		case 6: //start current 5 measurement transmission
			if((n_block_send+1)*5>=bytes_to_write)
				UDR = 0xCF;
			else
				UDR = 0xCC;
			tx_state = 7;
			break;
		case 7:		//transmitting a block
			UDR = data[n_block_send*5+(n_byte_send++)];
			if(n_byte_send == 5)
			{
				n_byte_send = 0;
				n_block_send++;
				tx_state = 6;
			}
			if(n_block_send*5+n_byte_send>=bytes_to_write)
			{
				tx_state = 1;
				n_byte_send = 0;
				n_block_send = 0;
			}
			break;
		case 9:
			if(n_byte_send!=0)
			{
				UDR = data[n_block_send*5+(n_byte_send++)];
			}
			else
			{
				UDR = 0x01;
				tx_state = 1;
			}
			break;
		case 10:
			UDR = 0x01;
			tx_state = 1;
			break;
		default:
			break;			
	}
}

/*
*ADC interrupt vector
*/
ISR(ADC_vect)
{
	cli();
	adc_data = ((unsigned int)ADCL);
	adc_data |= ((unsigned int)ADCH)<<8;
	adc_flag = 0;	
	sei();
}

/*
* function to init uart
*/
void USART_Init( unsigned int ubrr)
{
	/* Set baud rate */
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSRB = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);
	/* Set frame format: 8data, 2 stop bit */
	UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
}

//main loop
void loop(void)
{

while(1)
{
	//if has custom message to send via uart, sends
	if(tx_state == 5 && to_send != 0)
	{
		UCSRB |= (1<<UDRIE);
		UDR = to_send;
		to_send = 0;
		tx_state = 1;
	}
	
	//on init turn on tx and send 0x10
	if(general_state == 0)
	{
		if(tx_state == 0 && on_init)
		{
			UCSRB |= (1<<UDRIE);
		}
		if(tx_state == 2 && on_init){
			UCSRB &= ~(1<<UDRIE);
			on_init = 0;
		}
		
	}
	
	//if calibration
	if(general_state == 10)
	{
		//init calibration
		if(calibration_state == 0)
		{
			ADCSRA &= ~(1<<ADIF);
			ADMUX &= ~(1<<MUX1);
			calibr_integr_count = 0;
			calibration_state = 1;
			calibr_sum = 0;
			if(goal_voltage > voltage)
			{
				pwm = 0;
				_delay_ms(200);
				_delay_ms(200);
			}
			pwm = 0;
			TCCR1B |= (1<<CS10);
			ADCSRA |= (1<<ADEN)  | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); //prescaler 128, free run
			_delay_ms(10);	
			//ADCSRA |= (1<<ADSC);
					
		}
		//calibrate voltage
		if(calibration_state == 1)
		{
			pwm = goal_voltage;
			for(byte i=0;i<6;i++)
			{
				PORTB |= (1<<1); //blink red
				_delay_ms(100);
				PORTB &= ~(1<<1);
				_delay_ms(100);
			}
			while(1)
			{
				calibr_sum = 0;
				for(byte i=0;i<16;i++)
				{
					ADCSRA |= (1<<ADSC);
					while(ADCSRA &(1<<ADSC));
					PORTB |= (1<<1);
					adc_data = ((unsigned int)ADCL);
					adc_data |= ((unsigned int)ADCH)<<8;
					calibr_sum+= adc_data;
				}
				unsigned int result_voltage = calibr_sum>>4;
				if(result_voltage == voltage)
				{
					calibration_state = 2;
					break;
				}
				else if (result_voltage > voltage)
					goal_voltage--;
				else
					goal_voltage++;
				pwm = goal_voltage;
				PORTB |= (1<<1); //blink red
				_delay_ms(100);
				PORTB &= ~(1<<1);
				_delay_ms(100);
			}			
		}
		
		//measure own current
		if(calibration_state == 2)
		{
			ADMUX |= (1<<MUX1);
			calibr_integr_count = 0;
			calibr_sum = 0;
			calibration_state = 3;
			_delay_ms(10);
		}
		if(calibration_state == 3)
		{
			ADCSRA |= (1<<ADSC);
			while(ADCSRA &(1<<ADSC));
			adc_data = ((unsigned int)ADCL);
			adc_data |= ((unsigned int)ADCH)<<8;
			calibr_sum += adc_data;
			calibr_integr_count++;
			
			if(calibr_integr_count == 16)
			{
				
				own_current = calibr_sum>>4; // equal to /16
				own_current = 1023-own_current;
				ready |= (1<<3);
				calibration_state = 0;
				general_state = 0;
				ADCSRA = (1<<ADEN) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); //prescaler 128
				pwm = 0;
				PORTC &= ~(1<<4); //turn off 20 Ohms
				_delay_ms(10);	
				//to_send = 0xFC;
				//tx_state = 5;
				PORTB &= ~(1<<1); //turn off red led after blinking	
				
									
			}
		}
		
	}
	
	//compute current in "turn on" state and measuring state
	if(general_state == 1 || general_state == 3)
	{
		unsigned char x = 0;
		while(adc_flag)
		{
			x= 1-x;
			_delay_us(1);		
		}
		if(x>2)
			x=2;
		adc_flag = 1;
		adc_data = 1023 - adc_data;	
		if(is_twenty)
		{
			if(adc_data >= 1023)
			{
				//operate fuse out
				pwm = 0;
				PORTB |= (1<<1);    //turn on red led
				PORTB &= ~(1<<0);	//turn off green led and relay
				PORTC &= ~(1<<4);   //turn off 20 ohms
				general_state = 2;
				/*if(tx_state == 6 || tx_state == 7)
				{
					tx_state = 9;
				}
				else
				{
					tx_state = 10;
				}
				UCSRB |= (1<<UDRIE);*/
				
			}
		}
		else			
		if(adc_data>=max_current && general_state != 2)
		{
			//operate fuse out
			pwm = 0;	
			PORTB |= (1<<1);    //turn on red led
			PORTB &= ~(1<<0);	//turn off green led and relay	
			PORTC &= ~(1<<4);
			general_state = 2;
			/*if(tx_state == 6 || tx_state == 7)
			{
				tx_state = 9;
			}
			else
			{
				tx_state = 10;
			}
			UCSRB |= (1<<UDRIE);*/
		}
	}	
	if(general_state == 3)
	{
		count_scale++;
		if(count_scale == prescaler)
		{
			count_scale = 0;
		}	
		else
		{
			continue;
		}
		
		if(adc_data > own_current_working)
			adc_data -= own_current_working; //compensate current
		else
			adc_data = 0;
	    
		if(adc_data>max_adc_value) //update maximum
		{
			max_adc_value = adc_data;
		}
		data[data_it++] |= (unsigned char)(adc_data>>(shift+2));
		data[data_it] = (unsigned char)(adc_data<<(8-shift-2));
		shift+=2;
		if(shift == 8)
		{
			shift = 0;
			data_it++;
			data[data_it] = 0;
		}
		if(data_it >= bytes_to_write)
		{
			data_it = 0;
			shift = 0;
			general_state = 1;
			if(tx_state == 2)
			{
				/*if(max_adc_value < 102)
				{
					to_send = 0xCB;
				}
				else
				{
					to_send = 0xCA;
				}			
				tx_state = 5;*/
				measure_finished = 1;
			}
		}
	}
		
}

}

int main(void)
{
	//ports init
	DDRB =  (1<<PINB2)|(1<<PINB1)|(1<<PINB0);
	//shunt relay port init
	DDRC |= (1<<PINC4);
	PORTC &= ~(1<<PINC4);
	//adc init
	ADMUX |= (0<<REFS1) | (0<<REFS0) | (0<<ADLAR) | (0<<MUX0); //left adjust, ref at avcc
	ADCSRA |= (1<<ADEN) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); //prescaler 128
	
	//pwm init
	TCCR1A = (1<<COM1B1) | (1<<WGM11) | (1<<WGM10); //10bit fast pwm
	pwm = (unsigned int)0;
	TCCR1B = (1<<WGM12);
	
	//adc timer init
	OCR2 = 240;
	TCNT2 = 0;
	TCCR2 = (1<<WGM21) | (0<<WGM20);
	TIMSK |= (1<<OCIE2);
	
	
	//usart init
	USART_Init(F_CPU/16/BAUD-1);
	
	sei();
    loop();
}