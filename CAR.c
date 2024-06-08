/* 고쳐야 함!!!!!!!!!!!!!!!
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#include "pwm.c"
#include "pwm.h"
#include "gpio.c"
#include "gpio.h"
#include "socket.c"
#include "socket.h"

#define STOP 0
#define FORWARD 1
#define BACKWARD 2

#define OUTPUT 1
#define INPUT 0

#define HIGH 1
#define LOW 0

#define ENB 18 // GPIO 18 for PWM channel 0

// Changed GPIO pins for IN3 and IN4
#define IN3 23 // GPIO 23 (physical pin 16)
#define IN4 24 // GPIO 24 (physical pin 18)

#define BUTTON 15 // physical 10
// #define SERVER_IP "192.168.13.3" // (?????)

#define VALUE_MAX 40
#define DIRECTION_MAX 40

// socket 파일에 있음
//#define DEFAULT_PORT 8888


void setPinConfig(int pwm_num, int INA, int INB)
{
  pwm_unexport(pwm_num);
  gpio_export(INA);
  gpio_direction(INA, OUTPUT);
  gpio_export(INB);
  gpio_direction(INB, OUTPUT);

  pwm_export(pwm_num);
  pwm_write_period(pwm_num, DEFAULT_PERIOD);
  pwm_enable(pwm_num);
}

void setMotorControl(int pwm_num, int INA, int INB, float speed_ratio, int stat)
{
  pwm_ratio(pwm_num, speed_ratio);

  if (stat == FORWARD)
  {
    gpio_write(INA, HIGH);
    gpio_write(INB, LOW);
  }
  else if (stat == BACKWARD)
  {
    gpio_write(INA, LOW);
    gpio_write(INB, HIGH);
  }
  else if (stat == STOP)
  {
    gpio_write(INA, LOW);
    gpio_write(INB, LOW);
  }
}

void setMotor(float speed_ratio, int stat)
{
  setMotorControl(0, IN3, IN4, speed_ratio, stat);
}

void *motor_control_thread(void *arg)
{
  // pwm_unexport(ENB);
  setPinConfig(0, IN3, IN4);

  setMotor(0.80, FORWARD);
  sleep(1);

  setMotor(0.40, BACKWARD);
  sleep(1);

  setMotor(1.00, BACKWARD);
  sleep(1);

  setMotor(0.80, STOP);

  pwm_disable(0);
  pwm_disable(0);
  gpio_unexport(IN3);
  gpio_unexport(IN4);

  return NULL;
}

void *emergency_signal_thread(void *arg)
{

  gpio_unexport(BUTTON);
  gpio_export(BUTTON);
  gpio_direction(BUTTON, INPUT);

  enum gpio_value button_state;

  
  //문제 상황: 버튼이 눌리지도 않았는데 서버 소켓에 메세지 감 & 버튼 못읽음
  

  while (1)
  {
    if (gpio_read(BUTTON, &button_state) == 0)
    { // 성공적으로 읽었을 때
      if (button_state == HIGH)
      { // 버튼이 눌린 상태 (HIGH)
        printf("button clicked");
        int client_sock = socket_client("192.168.14.4", DEFAULT_PORT);
        if (client_sock != -1)
        {
          char *client_message = "Emergency vehicles are approaching!\n";
          write(client_sock, client_message, strlen(client_message));
          dprintf(client_sock, "%s", client_message);
          close(client_sock); // 소켓 사용 후 닫기
        }
        else
        {
          perror("socket_client failed");
        }
        sleep(1); // 버튼 상태를 너무 자주 확인하지 않도록 대기
        while (gpio_read(BUTTON, &button_state) == 0 && button_state == HIGH)
        {
          usleep(100000); // 100ms 대기
        }
      }
    }
    return NULL;
  }
}

int main(void)
{
  pthread_t motor_thread, emergency_thread;

  if (pthread_create(&motor_thread, NULL, motor_control_thread, NULL) != 0)
  {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&emergency_thread, NULL, emergency_signal_thread, NULL) != 0)
  {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(motor_thread, NULL) != 0)
  {
    perror("pthread_join");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(emergency_thread, NULL) != 0)
  {
    perror("pthread_join");
    exit(EXIT_FAILURE);
  }

  return 0;
}

*/
