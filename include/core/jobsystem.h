#pragma once
#ifndef JOB_SYSTEM_H
#define JOB_SYSTEM_H

#include <mana/core/memoryallocator.h>
//
#include <cstorage/queue.h>
#include <threads/threads.h>

struct Job {
  void (*job_func)(void*);
  void* job_data;
};

struct JobWorker {
  struct Queue task_queue;
  int thread_num;
  int run_thread;
  mtx_t mutex;
  cnd_t condition;
  thrd_t thread;
};

int job_worker_init(struct JobWorker* job_worker, int thread_num);
int job_worker_process_tasks(void* pass_worker);
void job_worker_start_thread(struct JobWorker* job_worker);
void job_worker_stop_thread(struct JobWorker* job_worker);

struct JobSystem {
  struct JobWorker* workers;
  int num_workers;
  int round_robin;
};

int job_system_init(struct JobSystem* job_system);
int job_system_delete(struct JobSystem* job_system);
void job_system_enqueue(struct JobSystem* job_system, struct Job* job);
void job_system_start_threads(struct JobSystem* job_system);
void job_system_wait(struct JobSystem* job_system);

#endif  // JOB_SYSTEM_H
