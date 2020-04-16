#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include "usiTwiSlave.h"

#define FIRMWARE_VERSION 0x21 //2.1

//------------ peripherals ----------------

#define LED_K PB0 
#define LED_A PB1

void ledOn() {
    DDRB |= _BV(LED_A) | _BV(LED_K); //forward bias the LED
    PORTB &= ~_BV(LED_K);            //flash it to discharge the PN junction capacitance
    PORTB |= _BV(LED_A);  
}

void ledOff() {
    DDRB &= ~(_BV(LED_A) | _BV(LED_K)); //make pins inputs
    PORTB &= ~(_BV(LED_A) | _BV(LED_K));//disable pullups
}


//------------------- initialization/setup-------------------

static inline void inline setupGPIO() {
    PORTA |= _BV(PA0);  //nothing
    PORTA &= ~_BV(PA0);                     
    PORTA |= _BV(PA2);  //nothing
    PORTA &= ~_BV(PA2);                     
    PORTA |= _BV(PA3);  //nothing
    PORTA &= ~_BV(PA3);                     
    DDRA |= _BV(PA7);   //nothing
    PORTA &= ~_BV(PA7);
    
    DDRB |= _BV(PB0);   //nothing
    PORTB &= ~_BV(PB0);
    DDRB |= _BV(PB1);   //nothing
    PORTB &= ~_BV(PB1);
    DDRB |= _BV(PB2);   //sqare wave output
    PORTB &= ~_BV(PB2);
}

//--------------- sleep / wakeup routines --------------

void inline sleepWhileADC() {
    set_sleep_mode(SLEEP_MODE_ADC);
//    while(1) {
        sleep_mode();
//    }
}

ISR(ADC_vect) { 
    //nothing, just wake up
}

// ------------------ capacitance measurement ------------------

static inline void startExcitationSignal() {
    OCR0A = 0;
    TCCR0A = _BV(COM0A0) |  //Toggle OC0A on Compare Match
            _BV(WGM01);     //reset timer to 0 on Compare Match
    TCCR0B = _BV(CS00);
}

static inline void initADC() {
    ADCSRA |= _BV(ADPS2); //adc clock speed = sysclk/16
    ADCSRA |= _BV(ADIE);
    ADCSRA |= _BV(ADEN);
}

static inline uint16_t getADC1() {
    ADMUX = _BV(MUX0); //select ADC1 as input
    
    ADCSRA |= _BV(ADSC); //start conversion
    
    sleepWhileADC();
    
    uint16_t result = ADCL;
    result |= ADCH << 8;
    
    return 1023 - result;
}

uint8_t capMeasurementInProgress = 0;


static inline uint16_t getCapacitance() {
	capMeasurementInProgress = 1;
    getADC1();
    uint16_t result = getADC1();
	capMeasurementInProgress = 0;
	return result;
}

#define isCapacitanceMeasurementInProgress() capMeasurementInProgress 


// ------------------ temperature measurement ------------------

 uint16_t getADC8() {
    ADMUX = 0b10100010; //select ADC8 as input, internal 1.1V Vref
    
    ADCSRA |= _BV(ADSC); //start conversion
    
    sleepWhileADC();
    
    uint16_t result = ADCL;
    result |= ADCH << 8;
    
    return result;
}

uint8_t tempMeasurementInProgress = 0;

static inline uint16_t getTemperature() {
    tempMeasurementInProgress = 1;
    getADC8();          //discard the first measurement
    uint16_t temperature = getADC8();
	tempMeasurementInProgress = 0;
	return temperature;
}

#define isTemperatureMeasurementInProgress() tempMeasurementInProgress

//--------------------- light measurement --------------------

volatile uint16_t light = 0;
volatile uint16_t lightCounter = 0;
volatile uint8_t lightCycleOver = 1;

static inline stopLightMeaseurement() {
    GIMSK &= ~_BV(PCIE1);
    TCCR1B = 0;
    PCMSK1 &= ~_BV(PCINT8);
    TIMSK1 &= ~_BV(TOIE1);

    lightCycleOver = 1;
}

ISR(PCINT1_vect) {
    GIMSK &= ~_BV(PCIE1);//disable pin change interrupts
    TCCR1B = 0;          //stop timer
    lightCounter = TCNT1;
    light = lightCounter;
    
    stopLightMeaseurement();
}

ISR(TIM1_OVF_vect) {
    lightCounter = 65535;
    light = lightCounter;

    stopLightMeaseurement();
}

static inline uint16_t getLight() {
    TIMSK1 |= _BV(TOIE1);               //enable timer overflow interrupt
    
    DDRB |= _BV(LED_A) | _BV(LED_K);    //forward bias the LED
    PORTB &= ~_BV(LED_K);               //flash it to discharge the PN junction capacitance
    PORTB |= _BV(LED_A);

    PORTB |= _BV(LED_K);                //reverse bias LED to charge capacitance in it
    PORTB &= ~_BV(LED_A);
    
    DDRB &= ~_BV(LED_K);                //make Cathode input
    PORTB &= ~(_BV(LED_A) | _BV(LED_K));//disable pullups
    
    TCNT1 = 0;
    TCCR1A = 0;
    TCCR1B = _BV(CS11) | _BV(CS10);                 //start timer1 with prescaler clk/64
    
    PCMSK1 |= _BV(PCINT8);              //enable pin change interrupt on LED_K
    GIMSK |= _BV(PCIE1); 

    lightCycleOver = 0;
}

static inline uint8_t lightMeasurementInProgress() {
    return !lightCycleOver;
}

// ----------------- sensor mode loop ---------------------

#define I2C_GET_CAPACITANCE     0x00
#define I2C_SET_ADDRESS         0x01
#define I2C_GET_ADDRESS         0x02
#define I2C_MEASURE_LIGHT       0x03
#define I2C_GET_LIGHT           0x04
#define I2C_GET_TEMPERATURE     0x05
#define I2C_RESET               0x06
#define I2C_GET_VERSION         0x07
#define I2C_SLEEP               0x08
#define I2C_GET_BUSY         	0x09

#define reset() wdt_enable(WDTO_15MS); while(1) {}

static inline void loopSensorMode() {
    startExcitationSignal();
    uint16_t currCapacitance = 0;
    uint16_t temperature = 0;
    
    while(1) {
    if(usiTwiDataInReceiveBuffer()) {
            uint8_t usiRx = usiTwiReceiveByte();
			
            if(I2C_GET_CAPACITANCE == usiRx) {
				ledOn();
                currCapacitance = getCapacitance();
                usiTwiTransmitByte(currCapacitance >> 8);
                usiTwiTransmitByte(currCapacitance &0x00FF);
			
            } else if(I2C_SET_ADDRESS == usiRx) {
                uint8_t newAddress = usiTwiReceiveByte();
                if(newAddress > 1 && newAddress < 127) {
                    eeprom_write_byte((uint8_t*)0x01, newAddress);
                }

            } else if(I2C_GET_ADDRESS == usiRx) {
                uint8_t newAddress = eeprom_read_byte((uint8_t*) 0x01);
                usiTwiTransmitByte(newAddress);

            } else if(I2C_MEASURE_LIGHT == usiRx) {
                if(!lightMeasurementInProgress()) {
                    getLight();
                }

            } else if(I2C_GET_LIGHT == usiRx) {
                GIMSK &= ~_BV(PCIE1);//disable pin change interrupts
                TCCR1B = 0;          //stop timer
                
                usiTwiTransmitByte(light >> 8);
                usiTwiTransmitByte(light & 0x00FF);

                GIMSK |= _BV(PCIE1); 
                TCCR1B = _BV(CS11) | _BV(CS10);                 //start timer1 with prescaler clk/64
            } else if(I2C_GET_TEMPERATURE == usiRx) {
                temperature = getTemperature();
                usiTwiTransmitByte(temperature >> 8);
                usiTwiTransmitByte(temperature & 0x00FF);
            } else if(I2C_RESET == usiRx) {
                reset();
            } else if(I2C_GET_VERSION == usiRx) {
                usiTwiTransmitByte(FIRMWARE_VERSION);
            } else if(I2C_SLEEP == usiRx) {
                set_sleep_mode(SLEEP_MODE_PWR_DOWN);
                sleep_mode();
			} else if(I2C_GET_BUSY == usiRx) {
				usiTwiTransmitByte(isCapacitanceMeasurementInProgress() || 
                                isTemperatureMeasurementInProgress() || 
                                lightMeasurementInProgress());
			}
			
			
			
        }
    }
}

//-----------------------------------------------------------------

#define I2C_ADDRESS_EEPROM_LOCATION (uint8_t*)0x01
#define I2C_ADDRESS_DEFAULT         0x20

int main (void) {
	MCUSR = 0;
    wdt_disable();
    setupGPIO();
    initADC();
    
    uint8_t address = eeprom_read_byte(I2C_ADDRESS_EEPROM_LOCATION);
    if(0 == address || 255 == address) {
        address = I2C_ADDRESS_DEFAULT;
    }
    usiTwiSlaveInit(address);
	usiTwiOnStartSending(&ledOn);
	usiTwiOnEndSending(&ledOff);
	
    CLKPR = _BV(CLKPCE);
    CLKPR = _BV(CLKPS1); //clock speed = clk/4 = 2Mhz

    sei();
    
    ledOn();
    _delay_ms(10);
    ledOff();

    loopSensorMode();
}
