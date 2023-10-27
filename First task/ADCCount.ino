#include <EEPROM.h>


const int adc_pin = 5;

unsigned long previous_millis = 0;
unsigned long seconds = 0;

const unsigned int adc_interval = 1;
const unsigned int flash_write_interval = 5;

int cur_addr = 0;
int addrs_count = 0;
int from_flash;
const int addr_step = sizeof(int);
const int max_addrs_amount = 10;


void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  unsigned long current_millis = millis();
  if (current_millis - previous_millis >= adc_interval * 1000) 
  {
    previous_millis = current_millis;
    
    // ADC calculation
    for (int i = 0; i < 1024; i++) 
    {
      int val = analogRead(adc_pin);
    }
    unsigned long time_end = millis();
    int adc_val = time_end - current_millis;

    Serial.print("ADC = ");
    Serial.println(adc_val);
    seconds++;

    if (seconds >= flash_write_interval) 
    {
      EEPROM.put(cur_addr, adc_val);
      Serial.print("EEPROM data: ");
      for (int addr = 0; addr <= cur_addr; addr += addr_step) 
      {
        EEPROM.get(addr, from_flash);
        Serial.print(from_flash);
        if (addr != cur_addr) 
        {
          Serial.print(", ");
        }
        else 
        {
          Serial.println();
        }
      }
      cur_addr += addr_step;
      seconds = 0;
      if (cur_addr >= max_addrs_amount * addr_step) 
      {
        // Clean eeprom data
        for (int addr = 0; addr <= cur_addr; addr++) 
        {
          eeprom_write_byte((uint8_t*)addr, 0xFF);
        }
        Serial.println("EEPROM data cleared!");
        cur_addr = 0;
      }
    }
  }
}
