#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include "gpio.c"
#include "pwm.c"
#include "socket.c"
#include "ultrasonic.c"
#include "rp3.h"

void *__rp1_thread(void *args)
{
    rp3_thread_args *__args = (rp3_thread_args *)args;
    char buffer;
    while (1)
    {
        printf("[__rp1_thread] Waiting for data from rp1...");
        if (read(__args->sockfd_rp1, &buffer, 1) < 1)
        {
            perror("[__rp1_thread] read");
            continue;
        }
        if (buffer != 1)
        {
            break;
        }
        printf("[__rp1_thread] Received.");
        pthread_t tid;
        pthread_create(&tid, NULL, execute_thread, args);
    }
}

void *__rp2_thread(void *args)
{
    rp3_thread_args *__args = (rp3_thread_args *)args;
    struct weatherResult buffer;
    while (1)
    {
        if (read(__args->sockfd_rp2, &buffer, sizeof(struct weatherResult)) < 1)
        {
            perror("[__rp2_thread] read");
            continue;
        }
        if (buffer.light == -1 || buffer.rain == -1 || buffer.DHT.humi == -1 || buffer.DHT.temp == -1)
        {
            break;
        }
        pthread_mutex_lock(&__args->lock_rp2);
        __args->rp2_data = buffer;
        pthread_mutex_unlock(&__args->lock_rp2);
    }
}

void *monitor_thread(void *args)
{
    rp3_thread_args *__args = (rp3_thread_args *)args;
    double us_data_speed, threshold = SPEED_THRESHOLD_DEFAULT;
    struct weatherResult rp2_data;
    while (1)
    {
        /* Fetch data */
        pthread_mutex_lock(&__args->us_data->lock);
        us_data_speed = __args->us_data->speed;
        pthread_mutex_unlock(&__args->us_data->lock);
        pthread_mutex_lock(&__args->lock_rp2);
        rp2_data = __args->rp2_data;
        pthread_mutex_unlock(&__args->lock_rp2);
        /* Manipulate threshold */
        if (rp2_data.rain == WET)
        {
            threshold *= SPEED_THRESHOLD_WET_DECREASE_RATIO;
        }
        if (rp2_data.light < LIGHT_THRESHOLD || rp2_data.DHT.humi < HUMID_THRESHOLD)
        {
            threshold *= SPEED_THRESHOLD_FOG_DECREASE_RATIO;
        }
        /* Check 철컹 condition meet */
        if (threshold < us_data_speed)
        {
            pthread_t tid;
            pthread_create(&tid, NULL, execute_thread, args);
            /**
             * No need to wait thread termination.
             * Exception on multiple call of `pthread_create` on `execute_thread`
             * is handled in `execute_thread`.
             */
        }
        usleep(1000000 / __args->polling_rate);
    }
}

void __execute_thread_finalize(void *args)
{
    rp3_thread_args *__args = (rp3_thread_args *)args;
    /* Unexport GPIO */
    gpio_write(__args->motor_pin, LOW);
    gpio_unexport(__args->motor_pin);
    /** 
     * Use `__args->tid_executing` as flag;
     * confirms `execute_thread` terminated
     * so that another `execute_thread` can proceed without cancel.
     */
    pthread_mutex_lock(&__args->lock_execute);
    __args->tid_executing = 0;
    pthread_mutex_unlock(&__args->lock_execute);
}

void *execute_thread(void *args)
{
    rp3_thread_args *__args = (rp3_thread_args *)args;
    /* Temporarily set uncancelable */
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    /**
     * Use `__args->tid_executing` as flag;
     * if there is another thread running `execute_thread`, cancel it.
     * So that overlapped calling of `execute_thread` is allowed.
     */
    pthread_mutex_lock(&__args->lock_execute);
    if (__args->tid_executing > 0)
    {
        pthread_cancel(__args->tid_executing);
        __args->tid_executing = pthread_self();
    }
    pthread_mutex_unlock(&__args->lock_execute);
    /* Restore cancelstate */
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    /* Init Motor GPIO */
    gpio_export(__args->motor_pin);
    pthread_cleanup_push(__execute_thread_finalize, args);
    gpio_set_direction(__args->motor_pin, OUT);
    /* Motor run! */
    gpio_write(__args->motor_pin, HIGH);
    sleep(MOTOR_DURATION);
    pthread_cleanup_pop(1);
    /* gpio_write(__args->motor_pin, LOW) is in cleanup function */
}

int main(int argc, char *argv[])
{
    
}
