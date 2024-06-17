#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#include "pwm.c"
#include "gpio.c"
#include "socket.c"
#include "buzzer.c"

#define STOP -1
#define FORWARD 1
#define BACKWARD 2

#define OUTPUT 1
#define INPUT 0

#define HIGH 1
#define LOW 0

// GPIO pins for motor
#define IN3 23 // GPIO 23
#define IN4 24 // GPIO 24

#define EMERGENCY_BUTTON_PIN 17 // 긴급 차량 버튼
#define MOTOR_PIN 27
#define MOTOR_ACTUATE_PIN 22 // 모터 작동 버튼
#define SIREN 0              // GPIO 18, PWM 0번 채널 사용

// #define VALUE_MAX 40
// #define DIRECTION_MAX 40

void setMotor(int stat)
{

  // 정방향
  if (stat == FORWARD)
  {
    gpio_write(IN3, HIGH);
    gpio_write(IN4, LOW);
  }
  // 역방향
  else if (stat == BACKWARD)
  {
    gpio_write(IN3, LOW);
    gpio_write(IN4, HIGH);
  }
  // 정지
  else if (stat == STOP)
  {
    gpio_write(IN3, LOW);
    gpio_write(IN4, LOW);
  }
}

void *motor_control_thread(void *arg)
{
  gpio_export(MOTOR_ACTUATE_PIN);
  gpio_export(IN3);
  gpio_set_direction(IN3, OUTPUT);
  gpio_export(IN4);
  gpio_set_direction(IN4, OUTPUT);

  gpio_value button1_state = LOW;
  gpio_value prev_button1_state = LOW;

  //gpio_value value;

  while (1)
  {
    gpio_read(MOTOR_ACTUATE_PIN, &button1_state);
    if (button1_state != prev_button1_state)
    {

      setMotor(FORWARD);
      sleep(3);
      // setMotor(BACKWARD);
      // sleep(10);

      setMotor(STOP);
    }

    prev_button1_state = button1_state;

    usleep(100000); // 100ms 대기
  }

  gpio_unexport(IN3);
  gpio_unexport(IN4);
  gpio_unexport(MOTOR_ACTUATE_PIN);

  return NULL;
}

void *emergency_signal_thread(void *__sock_fd) // 긴급 차량 버튼이 눌리면 서버 소켓과 통신 & 긴급 차량 버튼이 눌리면 부저 작동
{
  int *sock_fd = (int *)__sock_fd;
  gpio_export(EMERGENCY_BUTTON_PIN);
  gpio_set_direction(EMERGENCY_BUTTON_PIN, INPUT);
  gpio_value button2_state = LOW;
  gpio_value prev_button2_state;

  while (1)
  {
    if (gpio_read(EMERGENCY_BUTTON_PIN, &button2_state) == -1)
    {
      perror("gpio_read");
    }

    // 버튼이 눌렸을 때
    // if ( (button2_state == LOW && prev_button2_state == HIGH) || button2_state == HIGH && prev_button2_state == LOW)
    if (button2_state == HIGH)
    {

      // (1) 서버 소켓에 메세지 전달
      int buf = 0;
      if (write(*sock_fd, &buf, 1) == -1)
      {
        perror("[write]");
      }

      // 사이렌 작동
      siren_thread_args args =
          {
              .pwm_num = SIREN,
              .freq_max = 1000,
              .freq_min = 400,
          };
      pthread_t siren_thread_on;
      // pthread_create(&tid, NULL, siren_thread, &args);
      if (pthread_create(&siren_thread_on, NULL, siren_thread, &args) != 0)
      {
        perror("pthread_create");
        exit(EXIT_FAILURE);
      }
      if (pthread_join(siren_thread_on, NULL) != 0)
      {
        perror("pthread_join");
        exit(EXIT_FAILURE);
      }
    }

    prev_button2_state = button2_state;

    usleep(100000); // 100ms 대기
  }

  gpio_unexport(EMERGENCY_BUTTON_PIN);

  return NULL;
}

int main(void)
{
  pthread_t motor_thread, emergency_thread;
  int sock_fd = socket_client("192.168.14.10", 8883);

  if (pthread_create(&motor_thread, NULL, motor_control_thread, NULL) != 0)
  {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&emergency_thread, NULL, emergency_signal_thread, &sock_fd) != 0)
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
  close(sock_fd);
  return 0;
}
