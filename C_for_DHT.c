/* blink.c
 *
 * Raspberry Pi GPIO example using sysfs interface.
 * Guillermo A. Amaral B. <g@maral.me>
 *
 */
 
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h> 
 
#define IN  0
#define OUT 1
 
#define LOW  0
#define HIGH 1

#define MAX_TIME 85
 
#define PIN  4 /* P1-18 */

int dht11_val[5]={0,0,0,0,0}; 

void dht11_read_val();

static int
GPIOExport(int pin)
{
#define BUFFER_MAX 3
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;
 
	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open export for writing!\n");
		return(-1);
	}
 	printf("Pin is %d\n", pin); 
	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}
 
static int
GPIOUnexport(int pin)
{
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;
 
	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open unexport for writing!\n");
		return(-1);
	}
 
	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}
 
static void
GPIODirection(int pin, int dir)
{
	static const char s_directions_str[]  = "in\0out";
 
#define DIRECTION_MAX 35
	char path[DIRECTION_MAX];
	int fd;
 
	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio direction for writing!\n");
		exit(EXIT_FAILURE);
	}
 
	if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
		fprintf(stderr, "Failed to set direction!\n");
		exit(EXIT_FAILURE);
	}
 
	close(fd);

}

static int
GPIORead(int pin)
{
#define VALUE_MAX 30
	char path[VALUE_MAX];
	char value_str[3];
	int fd;
 
	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio value for reading!\n");
		return(-1);
	}
 
	if (-1 == read(fd, value_str, 3)) {
		fprintf(stderr, "Failed to read value!\n");
		return(-1);
	}
 
	close(fd);
 	//printf("value is %s", value_str);
	return(atoi(value_str));
}
 
static void
GPIOWrite(int pin, int value)
{
	static const char s_values_str[] = "01";
 
	char path[VALUE_MAX];
	int fd;
 
	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio value for writing!\n");
		exit(EXIT_FAILURE);
	}
 
	if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
		fprintf(stderr, "Failed to write value!\n");
		exit(EXIT_FAILURE);
	}
 
	close(fd);
}
 
int main()
{	
	/*
	 * Enable GPIO pins
	 */
	if (-1 == GPIOExport(PIN))
		return(1);
	
	dht11_read_val();
	
	/*
	 * Disable GPIO pins
	 */
	if (-1 == GPIOUnexport(PIN))
		return(4);
 
	return(0);
}
  
void dht11_read_val()  
{  
  uint8_t lststate=1;  
  uint8_t counter=0;  
  uint8_t j=0,i;
  int val;  
  float farenheit;  
  for(i=0;i<5;i++)  
     dht11_val[i]=0;
  GPIODirection(PIN, OUT); 
  GPIOWrite(PIN,LOW);  
  usleep(18*1000);  
  GPIOWrite(PIN,HIGH);  
  usleep(40);  
  GPIODirection(PIN, IN); 
  printf("Reached!!\n");  
  for(i=0;i<MAX_TIME;i++)  
  {  
    counter=0;
  val = GPIORead(PIN);
    while(val ==lststate){
      counter++;  
      usleep(1);  
      if(counter==255) {
  printf("counter is %d\n",counter);
        break;  
}
val = GPIORead(PIN);

    }  
    lststate=GPIORead(PIN);
    printf("lststate is %d and i is %d and j is %d\n", lststate,i,j);  
    if(counter==255)  
       break;  
    // top 3 transistions are ignored  
    if((i>=4)&&(i%2==0)){  
      dht11_val[j/8]<<=1;  
      if(counter>16)  
        dht11_val[j/8]|=1;  
      j++;  
    }  
  }  
  printf("J is %d",j);
  // verify cheksum and print the verified data  
  //if((j>=40)&&(dht11_val[4]==((dht11_val[0]+dht11_val[1]+dht11_val[2]+dht11_val[3])& 0xFF)))  
  //{  
    farenheit=dht11_val[2]*9./5.+32;  
    printf("Humidity = %d.%d %% Temperature = %d.%d *C (%.1f *F)\n",dht11_val[0],dht11_val[1],dht11_val[2],dht11_val[3],farenheit);  
  //}  
  //else  
    //printf("Invalid Data!!\n");  
}  
