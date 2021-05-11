#define STX_BYTE_SIZE 8
//#define DELAY_MICROSEC 60000
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>


int main(int argc, char *argv[]){
	long DELAY_MICROSEC = 0;
	char STX[] = {0x53,0x54,0x58,0x01,0xE1,0x00,0x00,0xB0};
	FILE *bin;
	int vfdDevice = open("/dev/ttyAS1",O_RDWR);
	if(vfdDevice < 0){
		printf("VFD Device open error\n");
		return -1;
	}
	if(argc < 3){
		printf("Argument Error\n");
		return -1;
	}
	bin = fopen(argv[1],"r");
	DELAY_MICROSEC = strtol(argv[2],NULL,10);
	if(bin == NULL){
		printf("Failed to open file\n");
		return -1;
	}


	int count=0;
	fread(&count, sizeof(int),1,bin);
	printf("Setted FPS : %.2f\n", 1000000.0 / (double)DELAY_MICROSEC);
	printf("Scene Count: %d\n",count);
	int sceneSizes[count];
	for(int i=0; i<count; i++){
		fread(&sceneSizes[i],sizeof(int),1,bin);
		//printf("Scene[%d] size: %d\n",i,sceneSizes[i]);
	}

	for(int i=0; i<count; i++){
		char scene[sceneSizes[i]];
		fread(scene,sizeof(char),sceneSizes[i],bin);
		char deviceFinalbuffer[sceneSizes[i] + STX_BYTE_SIZE];
		for(int k=0; k < STX_BYTE_SIZE; k++){
			deviceFinalbuffer[k] = STX[k];
		}
		for(int k=STX_BYTE_SIZE; k < (sceneSizes[i] + STX_BYTE_SIZE); k++){
			deviceFinalbuffer[k] = scene[k - STX_BYTE_SIZE];
		}
		printf("Writing VFD Scene[%d]",i);
		write(vfdDevice,deviceFinalbuffer,sceneSizes[i]+STX_BYTE_SIZE);
		usleep(DELAY_MICROSEC);
		printf("\r"); //Erase line
	}
	printf("\n\n");
	printf("End\n");
	close(vfdDevice);
	return 0;
}
