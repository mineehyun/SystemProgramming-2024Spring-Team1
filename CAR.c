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

#define BUTTON_PIN 17 // physical

#define VALUE_MAX 40
#define DIRECTION_MAX 40

void setPinConfig(int pwm_num, int INA, int INB)
{
  pwm_unexport(pwm_num);
  gpio_unexport(INA);
  gpio_unexport(INB);
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
  // 속도 설정
  pwm_ratio(pwm_num, speed_ratio);

  // 정방향
  if (stat == FORWARD)
  {
    gpio_write(INA, HIGH);
    gpio_write(INB, LOW);
  }
  // 역방향
  else if (stat == BACKWARD)
  {
    gpio_write(INA, LOW);
    gpio_write(INB, HIGH);
  }
  // 정지
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
  sleep(15);

  // setMotor(0.40, BACKWARD);
  // sleep(1);

  // setMotor(0.80, STOP);

  // pwm_disable(0);
  pwm_disable(0);
  gpio_unexport(IN3);
  gpio_unexport(IN4);

  return NULL;
}

/*
cntl c 했을때 모터 멈추도록 signal, event 핸들러 등록하기
*/

void *emergency_signal_thread(void *arg)
{
  gpio_unexport(BUTTON_PIN);
  gpio_export(BUTTON_PIN);
  gpio_direction(BUTTON_PIN, INPUT);

  int client_sock;
  // int prev_btn = 1; // 버튼이 눌리지 않은 상태

  enum gpio_value button_state;
  enum gpio_value last_button_state = HIGH;

  while (1)
  {
    gpio_read(BUTTON_PIN, &button_state);

    if (button_state == LOW && last_button_state == HIGH)
    {
      // 버튼이 눌렸을 때 (HIGH -> LOW 전환)
      client_sock = socket_client("192.168.14.4", DEFAULT_PORT);
      if (client_sock != -1)
      {
        char *client_message = "Emergency vehicles are approaching!\n";
        // write(client_sock, client_message, strlen(client_message));
        dprintf(client_sock, "%s", client_message);
        close(client_sock); // 소켓 사용 후 닫기
      }
      else
      {
        perror("socket_client failed");
      }
      //  pthread_detach(client_thread);
    }

    last_button_state = button_state;

    usleep(100000); // 100ms 대기
  }

  gpio_unexport(BUTTON_PIN);
  return NULL;
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
