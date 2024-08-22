#pragma once

#include "zlibpp.h"

#include <mutex>

class UniqueZlibPP {
  public:
  class ZlibPPAcquire {
public:
    ZlibPPAcquire(UniqueZlibPP& cmp, IZLibPP* compr): m_zlib(compr), m_cmp(cmp) {
      cmp.lock();
      m_zlib->reset();
    }

    ~ZlibPPAcquire() { m_cmp.unlock(); }

    IZLibPP* operator->() const { return m_zlib; }

    operator IZLibPP*() const { return m_zlib; }

private:
    IZLibPP*      m_zlib;
    UniqueZlibPP& m_cmp;
  };

  friend class ZlibPPAcquire;

  UniqueZlibPP(std::unique_ptr<IZLibPP> compr): m_compr(std::move(compr)) {}

  ZlibPPAcquire acquire() { return ZlibPPAcquire(*this, m_compr.get()); }

  protected:
  void lock() { m_mutex.lock(); }

  void unlock() { m_mutex.unlock(); }

  private:
  std::mutex               m_mutex;
  std::unique_ptr<IZLibPP> m_compr;
};
