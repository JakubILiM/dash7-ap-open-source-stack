/*
 *  Created on: May 9, 2013
 *  Authors:
 *  	maarten.weyn@artesis.be
 *  	glenn.ergeerts@artesis.be
 */

#include "timer.h"

#include "../hal/timer.h"
#include "log.h"

void timer_init()
{
    hal_timer_init();

    queue_init(&event_queue, (uint8_t*)&event_array, sizeof(event_array));
    started = false;
}

bool timer_insert_value_in_queue(timer_event* event)
{
	#ifdef LOG_FWK_ENABLED
	uint16_t current_timer = hal_timer_getvalue();
	log_print_string("timer_insert S - Timer current value: %d", current_timer);
	#endif

    // TODO: substract time already gone
	uint8_t position = 0;
	event->next_event = hal_timer_getvalue() + event->next_event;

	while (position < event_queue.length)
	{
		timer_event *temp_event = (timer_event*) queue_read_value(&event_queue, position, sizeof(timer_event));
		if (temp_event->next_event > event->next_event)
		{
			if (position == 0)
			{
				hal_timer_disable_interrupt();
				started = false;
			}
			queue_insert_value(&event_queue, (void*) event, position, sizeof(timer_event));
			position = 0;
			break;
		}
		position++;
	}

	if (position == event_queue.length)
	{
		if (!queue_push_value(&event_queue, (void*) event, sizeof(timer_event)))
			return false;
	}

	#ifdef LOG_FWK_ENABLED
	position = 0;
	for (;position<event_queue.length; position++)
	{
		timer_event *temp_event = (timer_event*) queue_read_value(&event_queue, position, sizeof(timer_event));
		log_print_string("Queue: %d", temp_event->next_event);
	}
	current_timer = hal_timer_getvalue();
	log_print_string("timer_insert E - Timer current value: %d", current_timer);
	#endif
	/*
	// code when timer was up instead of continous
    int16_t sum_next_event = - hal_timer_getvalue();

    while (position < event_queue.length)
    {
        timer_event *temp_event = (timer_event*) queue_read_value(&event_queue, position, sizeof(timer_event));
        if (event->next_event > sum_next_event + temp_event->next_event)
        {
            sum_next_event += temp_event->next_event;
        } else {
            uint16_t elapsed = 0;
            if (position == 0)
            {
                elapsed = hal_timer_getvalue();
                hal_timer_disable_interrupt();
                started = false;
            } else {
                event->next_event -= sum_next_event;
            }

            queue_insert_value(&event_queue, (void*) event, position, sizeof(timer_event));
            temp_event = (timer_event*) queue_read_value(&event_queue, position+1, sizeof(timer_event));

            temp_event->next_event -= (event->next_event + elapsed);
            return true;
        }
        position++;
    }

    if (position == event_queue.length)
    {
        if (started) event->next_event -= sum_next_event;
        return queue_push_value(&event_queue, (void*) event, sizeof(timer_event));
    }
    */

    return true;
}

bool timer_add_event(timer_event* event)
{
	#ifdef LOG_FWK_ENABLED
	uint16_t current_timer = hal_timer_getvalue();
	log_print_string("timer_add_event: %d", event->next_event);
	log_print_string("timer_add_event timer S : %d", current_timer);
	#endif

    if (event->next_event == 0)
    {
        event->f(&event->argument);
        return true;
    }

    timer_event new_event;
    new_event.f = event->f;
    new_event.next_event = event->next_event;
    new_event.argument = event->argument;

    if (timer_insert_value_in_queue(&new_event))
    {
        if (!started)
        {
            hal_timer_enable_interrupt();
            started = true;
            hal_timer_setvalue(new_event.next_event);
        }
        uint16_t diff = hal_timer_getvalue() - new_event.next_event;
        if (diff < 1000)
		{
			#ifdef LOG_FWK_ENABLED
        	current_timer = hal_timer_getvalue();
        	log_print_string("timer_add_event M timer overrun : %d", current_timer);
        	log_print_string("timer_add_event M: %d", new_event.next_event);
			#endif
			timer_completed();
		}
    } else {
        //log_print_string("Cannot add event, queue is full");
        return false;
    }

	#ifdef LOG_FWK_ENABLED
	current_timer = hal_timer_getvalue();
	log_print_string("timer_add_event timer E : %d", current_timer);
	#endif

    return true;
}

void timer_completed()
{
	#ifdef LOG_FWK_ENABLED
	uint16_t current_timer = hal_timer_getvalue();
	log_print_string("timer_completed: %d", current_timer);
	#endif

    timer_event* event = (timer_event*) queue_pop_value(&event_queue, sizeof(timer_event));

    bool directly_fire_new_event = false;

    if (event_queue.length == 0)
    {
        hal_timer_disable_interrupt();
        started = false;
    } else {
        hal_timer_setvalue(((timer_event*) event_queue.front)->next_event);
        uint16_t diff = hal_timer_getvalue() - ((timer_event*) event_queue.front)->next_event;
        if (diff < 1000)
        	directly_fire_new_event = true;
    }

    event->f(&event->argument);

    if (directly_fire_new_event)
    	timer_completed();
}

uint16_t timer_get_counter_value()
{
    return hal_timer_getvalue();
}

void benchmarking_timer_init()
{
	hal_benchmarking_timer_init();
}

uint32_t benchmarking_timer_getvalue()
{
    return hal_benchmarking_timer_getvalue();
}
void benchmarking_timer_start()
{
	hal_benchmarking_timer_start();
}

void benchmarking_timer_stop()
{
	hal_benchmarking_timer_stop();
}
