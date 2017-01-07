#include "pintos_thread.h"

struct station {
    // the number of waiting passengers
    int num_pass;
    // the number of available seats in train
    int num_seats;

    // mutexes
    struct lock station_lock;
    struct condition station_cond;
    struct condition board_cond;
};

void
station_init(struct station *station)
{
    station->num_seats = 0;
    station->num_pass = 0;

    lock_init(&station->station_lock);
    cond_init(&station->station_cond);
    cond_init(&station->board_cond);
}

void
station_load_train(struct station *station, int count)
{
    lock_acquire(&station->station_lock);

    station->num_seats += count;
    cond_broadcast(&station->station_cond, &station->station_lock);

    // 'while' clause, because every passengers alerts it to be on board the train.
    while(station->num_seats > 0 && station->num_pass > 0)
        cond_wait(&station->board_cond, &station->station_lock);

    lock_release(&station->station_lock);
}

void
station_wait_for_train(struct station *station)
{
    lock_acquire(&station->station_lock);

    station->num_pass++;

    if(station->num_seats <= 0)
        cond_wait(&station->station_cond, &station->station_lock);

    station->num_seats--;
    cond_signal(&station->board_cond, &station->station_lock);

    lock_release(&station->station_lock);
}

void
station_on_board(struct station *station)
{
    lock_acquire(&station->station_lock);

    station->num_pass--;
    cond_signal(&station->board_cond, &station->station_lock);

    lock_release(&station->station_lock);
}
