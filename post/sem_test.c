#include <stdio.h>
#include <sys/types.h>/*数据类型定义*/
#include <sys/stat.h>/*open时权限的定义*/
#include <sys/ioctl.h>/*ioctl*/
#include <fcntl.h>/*open，read，write，read，等具体实现*/
#include <unistd.h>/*一些标志位实现，如0，1，2的具体实现等等*/
#define DEV_FILE_NAME "/dev/MSB_SEM"
#define IOCTRL_GPIO_ON 1
#define IOCTRL_GPIO_OFF 0
#define LED1 0
#define LED2 1
#define LED3 2
#define BEEP 3
#define RELAY 4
int main ( )
{
int devfd;

devfd=open(DEV_FILE_NAME,O_RDWR|O_NDELAY);
printf("MAOMAO_sem_start\n");
if(!devfd)
{
	printf("打开失败\n");
}
else if(devfd>=0)
{
	printf("打开成功\n");
}
sleep(10);
printf("设备运行\n");


printf("MAOMAO_sem_end\n");
close(devfd);



return 0;
}
