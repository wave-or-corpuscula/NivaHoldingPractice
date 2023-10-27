#include <SPI.h>
#include <SD.h>


unsigned int cs_pin = 10;

unsigned long previous_millis = 0;
char* rand_in_sec = "rand_nums.txt";
char* rand_in_even_sec = "even_rand_nums.txt";
FILE *rand_file;
FILE *even_rand_file;

char buf[255];

int nums_count = 0;


void setup() 
{
  Serial.begin(9600);
  Serial.println("Initializing SD card...");
  if (!SD.begin(cs_pin)) 
  {
    Serial.println("SD initialization failed!");
    return;
  }
  Serial.println("Initializing done!");
}

void loop() 
{
  unsigned long current_millis = millis();
  if (current_millis - previous_millis >= 1000) 
  {
    previous_millis = current_millis;
    int rand_num = rand()
    sprintf(buf, "%d\n", rand_num);

    rand_file = SD.open(rand_in_sec, FILE_WRITE);
    if (rand_file) 
    {
      rand_file.write(buf);
      rand_file.close();
    }
    if (!(millis() / 1000 % 2)) 
    {
      if (nums_count == 10)
      {
        SD.remove(rand_in_even_sec)
        nums_count = 0;
      }
      even_rand_file = SD.open(rand_in_even_sec, FILE_WRITE);
      even_rand_file.write(buf);
      even_rand_file.close();
      nums_count++;
    }
  }
}