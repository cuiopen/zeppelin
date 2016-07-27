#include "zp_data_dispatch_thread.h"

#include <glog/logging.h>

#include "zp_data_client_conn.h"
#include "zp_data_server.h"

extern ZPDataServer* g_zp_data_server;

ZPDataDispatchThread::ZPDataDispatchThread(int port, int work_num, ZPDataWorkerThread** worker_thread, int cron_interval)
  : DispatchThread::DispatchThread(port, work_num,
                                   reinterpret_cast<pink::WorkerThread<ZPDataClientConn>**>(worker_thread),
                                   cron_interval) {
}

ZPDataDispatchThread::~ZPDataDispatchThread() {
  LOG(INFO) << "Dispatch thread " << thread_id() << " exit!!!";
}

void ZPDataDispatchThread::CronHandle() {
  uint64_t server_querynum = 0;
  uint64_t server_current_qps = 0;
  for (int i = 0; i < work_num(); i++) {
    slash::RWLock(&(((ZPDataWorkerThread**)worker_thread())[i]->rwlock_), false);
    server_querynum += ((ZPDataWorkerThread**)worker_thread())[i]->thread_querynum();
    server_current_qps += ((ZPDataWorkerThread**)worker_thread())[i]->last_sec_thread_querynum();
  }

  LOG(INFO) << "ClientNum: " << ClientNum() << " ServerQueryNum: " << server_querynum << " ServerCurrentQps: " << server_current_qps;
}

int ZPDataDispatchThread::ClientNum() {
  int num = 0;
  for (int i = 0; i < work_num(); i++) {
    num += ((ZPDataWorkerThread**)worker_thread())[i]->ThreadClientNum();
  }
  return num;
}