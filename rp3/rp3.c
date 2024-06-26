#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include "../gpio.c"
#include "../socket.c"
#include "ultrasonic.c"
#include "rp3.h"

void *__rp1_thread(void *args)
{
    printf("[__rp1_thread] Start\n");
    rp3_thread_args *__args = (rp3_thread_args *)args;
    uint8_t buffer;
    int __result;
    while (1)
    {
        __result = read(__args->sockfd_rp1, &buffer, 1);
        /* EOF. wait for new data */
        if (__result == 0)
        {
            usleep(SOCKET_INTERVAL);
            continue;
        }
        /* Error */
        else if (__result == -1)
        {
            perror("[__rp1_thread] read");
            return NULL;
        }
        /* 뭐라도 받았으면 철!!!!!컹!!!!!!!!! */
        pthread_t tid;
        pthread_create(&tid, NULL, __execute_thread, args);
        pthread_detach(tid);
    }
}

void *__rp2_thread(void *args)
{
    printf("[__rp2_thread] Start\n");
    rp3_thread_args *__args = (rp3_thread_args *)args;
    struct weatherResult buffer;
    int __result;
    while (1)
    {
        __result = read(__args->sockfd_rp2, &buffer, sizeof(struct weatherResult));
        /* EOF. wait for new data */
        if (__result == 0)
        {
            usleep(SOCKET_INTERVAL);
            continue;
        }
        /* Error */
        if (__result == -1)
        {
            perror("[__rp2_thread] read");
            return NULL;
        }
        /* Data received; update */
        pthread_mutex_lock(&__args->lock_rp2_data);
        __args->rp2_data = buffer;
        pthread_mutex_unlock(&__args->lock_rp2_data);
    }
}

void *monitor_thread(void *args)
{
    printf("[monitor_thread] Start\n");
    rp3_thread_args *__args = (rp3_thread_args *)args;
    double us_data_speed, threshold;
    struct weatherResult rp2_data;
    while (1)
    {
        /* Fetch data */
        pthread_mutex_lock(&__args->us_data->lock);
        us_data_speed = __args->us_data->speed;
        pthread_mutex_unlock(&__args->us_data->lock);
        pthread_mutex_lock(&__args->lock_rp2_data);
        rp2_data = __args->rp2_data;
        pthread_mutex_unlock(&__args->lock_rp2_data);
        /* Manipulate threshold */
        threshold = SPEED_THRESHOLD_DEFAULT;
        if (rp2_data.rain == WET)
        {
            threshold *= SPEED_THRESHOLD_WET_DECREASE_RATIO;
        }
        if (rp2_data.light < LIGHT_THRESHOLD || rp2_data.DHT.humi > HUMID_THRESHOLD)
        {
            threshold *= SPEED_THRESHOLD_FOG_DECREASE_RATIO;
        }
        printf("[monitor] 속도 %4.0lf 비 %d 조도 %d 온도 %d 습도 %d\n",
                us_data_speed, rp2_data.rain, rp2_data.light, rp2_data.DHT.temp, rp2_data.DHT.humi);
        /* Check 철컹 condition */
        if (threshold < us_data_speed)
        {
            /* 철!!!!!컹!!!!!!!!! */
            pthread_t tid;
            pthread_create(&tid, NULL, __execute_thread, args);
            pthread_detach(tid);
        }
        usleep(MONITOR_INTERVAL);
    }
}

void __execute_thread_finalize(void *args)
{
    rp3_thread_args *__args = (rp3_thread_args *)args;
    /* Send rp4 썸띵 only if the cancellation call is not from the another thread */
    pthread_mutex_lock(&__args->lock_tid_executing);
    if (__args->tid_executing == 0)
    {
        uint8_t byte = 0x01;
        write(__args->sockfd_rp4, &byte, 1);
    }
    /* Mark there is no other thread in `__execute_thread` */
    __args->tid_executing = 0;
    pthread_mutex_unlock(&__args->lock_tid_executing);
    /* Allow another thread proceed `__execute_thread` */
    pthread_mutex_unlock(&__args->lock_motor);
}

void *__execute_thread(void *args)
{
    rp3_thread_args *__args = (rp3_thread_args *)args;
    /* Protect the critical section and GPIOs from `pthread_cancel` */
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    /* If there is another thread in `__execute_thread` */
    pthread_mutex_lock(&__args->lock_tid_executing);
    if (__args->tid_executing > 0)
    {
        /* Cancel it */
        pthread_cancel(__args->tid_executing);
    }
    /* 스레드 없으면 */
    else
    {
        /* Send rp4 썸띵 */
        uint8_t byte = 0x01;
        write(__args->sockfd_rp4, &byte, 1);
    }
    pthread_mutex_unlock(&__args->lock_tid_executing);
    /* And wait for cancellation complete */
    pthread_mutex_lock(&__args->lock_motor);
    /* Mark `__execute_thread` is mine! */
    pthread_mutex_lock(&__args->lock_tid_executing);
    __args->tid_executing = pthread_self();
    pthread_mutex_unlock(&__args->lock_tid_executing);
    /* Init GPIO */
    gpio_write(__args->cheolkeong_pin, LOW);
    gpio_write(__args->motor_pin, LOW);
    /* Cancelable from here */
    pthread_cleanup_push(__execute_thread_finalize, args);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    /* 철컹! */
    gpio_write(__args->cheolkeong_pin, HIGH);
    int __wait_length = CHOELKEONG_DURATION * CLOCKS_PER_SEC / TEST_CANCEL_INTERVAL;
    for (int i = 0; i < __wait_length; i++)
    {
        usleep(TEST_CANCEL_INTERVAL);
    }
    /* 도로 원상복구 */
    gpio_write(__args->cheolkeong_pin, LOW);
    gpio_write(__args->motor_pin, HIGH);
    __wait_length = MOTOR_DURATION * CLOCKS_PER_SEC / TEST_CANCEL_INTERVAL;
    for (int i = 0; i < __wait_length; i++)
    {
        usleep(TEST_CANCEL_INTERVAL);
    }
    gpio_write(__args->motor_pin, LOW);
    /* 끗! */
    pthread_cleanup_pop(1);
}

int main(int argc, char *argv[])
{
    rp3_thread_args *args = (rp3_thread_args *)malloc(sizeof(rp3_thread_args));
    args->us_data = (us_thread_args *)malloc(sizeof(us_thread_args));
    args->cheolkeong_pin = 17;
    args->motor_pin = 27;
    args->us_data->echo = 5;
    args->us_data->trig = 6;
    args->sockfd_rp1 = socket_server(8883);
    args->sockfd_rp2 = socket_server(8882);
    args->sockfd_rp4 = socket_server(8884);
    pthread_mutex_init(&args->lock_tid_executing, NULL);
    pthread_mutex_init(&args->lock_motor, NULL);
    pthread_mutex_init(&args->lock_rp2_data, NULL);
    pthread_mutex_init(&args->us_data->lock, NULL);
    gpio_export(args->cheolkeong_pin);
    gpio_export(args->motor_pin);
    gpio_set_direction(args->cheolkeong_pin, OUT);
    gpio_set_direction(args->motor_pin, OUT);
    /* Create theads */
    pthread_t tid_rp1, tid_rp2, tid_monitor, tid_us;
    pthread_create(&tid_rp1, NULL, __rp1_thread, args);
    pthread_create(&tid_rp2, NULL, __rp2_thread, args);
    pthread_create(&tid_us, NULL, us_thread, args->us_data);
    /* 기다렸다가 모니터링시작해 안그럼 segfault 남 */
    sleep(1);
    pthread_create(&tid_monitor, NULL, monitor_thread, args);
    pthread_join(tid_monitor, NULL);
    /* Finalize */
    gpio_unexport(args->cheolkeong_pin);
    gpio_unexport(args->motor_pin);
    pthread_mutex_destroy(&args->lock_rp2_data);
    pthread_mutex_destroy(&args->lock_tid_executing);
    pthread_mutex_destroy(&args->lock_motor);
    pthread_mutex_destroy(&args->us_data->lock);
    close(args->sockfd_rp1);
    close(args->sockfd_rp2);
    close(args->sockfd_rp4);
    free(args->us_data);
    free(args);
    return 0;
}