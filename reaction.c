#include "pintos_thread.h"

// Forward declaration. This function is implemented in reaction-runner.c,
// but you needn't care what it does. Just be sure it's called when
// appropriate within reaction_o()/reaction_h().
void make_water();

struct reaction {
    int num_h;

    struct lock react_lock;
    struct condition o_cond;
    struct condition h_cond;
};

void
reaction_init(struct reaction *reaction)
{
    reaction->num_h = 0;

    lock_init(&reaction->react_lock);
    cond_init(&reaction->o_cond);
    cond_init(&reaction->h_cond);
}

void
reaction_h(struct reaction *reaction)
{
    lock_acquire(&reaction->react_lock);

    reaction->num_h++;

    if(reaction->num_h > 2)
        cond_signal(&reaction->o_cond, &reaction->react_lock);

    cond_wait(&reaction->h_cond, &reaction->react_lock);

    lock_release(&reaction->react_lock);
}

void
reaction_o(struct reaction *reaction)
{
    lock_acquire(&reaction->react_lock);

    while(reaction->num_h < 2)
        cond_wait(&reaction->o_cond, &reaction->react_lock);

    reaction->num_h -= 2;

    make_water();
    cond_signal(&reaction->h_cond, &reaction->react_lock);
    cond_signal(&reaction->h_cond, &reaction->react_lock);

    lock_release(&reaction->react_lock);
}
