/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis <davidsondfgl@hotmail.com>
 *
 * This file is part of Nanvix.
 *
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <nanvix/clock.h>
#include <nanvix/const.h>
#include <nanvix/hal.h>
#include <nanvix/pm.h>
#include <signal.h>
#include <nanvix/klib.h>

#define FIFO 0 // = ROUND ROBIN
#define PRIORITE 1
#define LOTERIE 2
#define MULTIPLE_QUEUES 3

const int ORDONNANCEMENT = LOTERIE;

/**
 * @brief Schedules a process to execution.
 *
 * @param proc Process to be scheduled.
 */
PUBLIC void sched(struct process *proc)
{
	proc->state = PROC_READY;
	proc->counter = 0;
}

/**
 * @brief Stops the current running process.
 */
PUBLIC void stop(void)
{
	curr_proc->state = PROC_STOPPED;
	sndsig(curr_proc->father, SIGCHLD);
	yield();
}

/**
 * @brief Resumes a process.
 *
 * @param proc Process to be resumed.
 *
 * @note The process must stopped to be resumed.
 */
PUBLIC void resume(struct process *proc)
{
	/* Resume only if process has stopped. */
	if (proc->state == PROC_STOPPED)
		sched(proc);
}

/**
 * @brief comutation sur un nouveau processus
 *
 * @param proc Processus qui doit comuter.
 *
 * @note The process must stopped to be resumed.
 */
PRIVATE void switchProcess(struct process *next)
{
	next->priority = PRIO_USER;
	next->state = PROC_RUNNING;
	next->counter = ORDONNANCEMENT == MULTIPLE_QUEUES ? (1 << (next->class - 1)) * PROC_QUANTUM : PROC_QUANTUM;
	if (curr_proc != next)
		switch_to(next);
}

PRIVATE void loterie(struct process *p, struct process *next)
{
	int total = 0;

	int min = INT_MAX;
	int max = INT_MIN;
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		if (p->state != PROC_READY)
			continue;

		if (p->nice < min)
			min = p->nice;

		if (p->nice > max)
			max = p->nice;
	}

	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		if (p->state != PROC_READY)
			continue;

		next = p;

		total += -(min + 1 + p->nice);
	}

	int win = total == 0 ? 0 : (krand() % total);
	int range = 0;
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		if (p->state != PROC_READY)
			continue;

		int prevRange = range;
		range += -(min + 1 + p->nice);

		if ((win <= range) && (win >= prevRange))
			next = p;
	}

	switchProcess(next);
}

PRIVATE int isPriority(struct process *p1, struct process *p2)
{
	return (p1->priority < p2->priority) || (p1->priority == p2->priority && p1->nice < p2->nice)
	|| (p1->priority == p2->priority && p1->nice == p2->nice && p1->utime + p1->ktime < p2->utime + p2->ktime);
}

PRIVATE void priorite(struct process *p, struct process *next)
{
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip non-ready process. */
		if (p->state != PROC_READY)
			continue;

		if (next == IDLE)
			next = p;
		/*
		 * Process with higher
		 * priority found.
		 */
		else if (isPriority(p, next))
			next = p;
	}
	switchProcess(next);
}

PRIVATE void multiple_queues(struct process *p, struct process *next)
{
	if (curr_proc->counter == 0)
		curr_proc->class ++;

	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip non-ready process. */
		if (p->state != PROC_READY)
			continue;

		/*
		 * Process with higher
		 * waiting time found.
		 */
		if (p->counter > next->counter)
		{
			next->counter++;
			next = p;
		}

		/*
		 * Increment waiting
		 * time of process.
		 */
		else
			p->counter++;
	}

	switchProcess(next);
}

PRIVATE void fifo(struct process *p, struct process *next)
{
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip non-ready process. */
		if (p->state != PROC_READY)
			continue;

		/*
		 * Process with higher
		 * waiting time found.
		 */
		if (p->counter > next->counter)
		{
			next->counter++;
			next = p;
		}

		/*
		 * Increment waiting
		 * time of process.
		 */
		else
			p->counter++;
	}
	switchProcess(next);
}

/**
 * @brief Yields the processor.
 */
PUBLIC void yield(void)
{
	struct process *p;	  /* Working process.     */
	struct process *next; /* Next process to run. */

	/* Re-schedule process for execution. */
	if (curr_proc->state == PROC_RUNNING)
		sched(curr_proc);

	/* Remember this process. */
	last_proc = curr_proc;

	/* Check alarm. */
	for (p = FIRST_PROC; p <= LAST_PROC; p++)
	{
		/* Skip invalid processes. */
		if (!IS_VALID(p))
			continue;

		next = p;

		/* Alarm has expired. */
		if ((p->alarm) && (p->alarm < ticks))
			p->alarm = 0, sndsig(p, SIGALRM);
	}

	next = IDLE;
	switch (ORDONNANCEMENT)
	{
	case FIFO:
		fifo(p, next);
		break;
	case PRIORITE:
		priorite(p, next);
		break;
	case LOTERIE:
		loterie(p, next);
		break;
	case MULTIPLE_QUEUES:
		multiple_queues(p, next);
	default:
		break;
	}
}
