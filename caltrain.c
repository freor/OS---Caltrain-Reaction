#include "pintos_thread.h"

struct station {
    int num_seat;
    int num_wait;
    int num_board;

    struct lock station_lock;
    struct condition station_cond;
    struct condition board_cond;
};

void
station_init(struct station *station)
{
    station->num_seat = 0;
    station->num_wait = 0;
    station->num_board = 0;

    lock_init(&station->station_lock);
    cond_init(&station->station_cond);
    cond_init(&station->board_cond);
}

void
station_load_train(struct station *station, int count)
{
    lock_acquire(&station->station_lock);

    station->num_seat = count;

    while((station->num_wait > 0 && station->num_seat > 0) || station->num_board > 0){
        cond_broadcast(&station->station_cond, &station->station_lock);
        cond_wait(&station->board_cond, &station->station_lock);
    }

    station->num_seat = 0;
    station->num_board = 0;

    lock_release(&station->station_lock);
}

void
station_wait_for_train(struct station *station)
{
    lock_acquire(&station->station_lock);

    station->num_wait++;

    while(station->num_seat == 0)
        cond_wait(&station->station_cond, &station->station_lock);

    station->num_wait--;
    station->num_seat--;
    station->num_board++;

    lock_release(&station->station_lock);
}

void
station_on_board(struct station *station)
{
    lock_acquire(&station->station_lock);

    station->num_board--;
    cond_signal(&station->board_cond, &station->station_lock);

    lock_release(&station->station_lock);
}
