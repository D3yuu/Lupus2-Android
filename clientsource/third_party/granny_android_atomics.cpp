#include "rrAtomics.h"

void rrAtomicMemoryBarrierFull(void) { __atomic_thread_fence(__ATOMIC_SEQ_CST); }

U32 rrAtomicLoadAcquire32(U32 const volatile *p) { return __atomic_load_n(p, __ATOMIC_ACQUIRE); }
U64 rrAtomicLoadAcquire64(U64 const volatile *p) { return __atomic_load_n(p, __ATOMIC_ACQUIRE); }
void rrAtomicStoreRelease32(U32 volatile *p, U32 v) { __atomic_store_n(p, v, __ATOMIC_RELEASE); }
void rrAtomicStoreRelease64(U64 volatile *p, U64 v) { __atomic_store_n(p, v, __ATOMIC_RELEASE); }

U32 rrAtomicCmpXchg32(U32 volatile *p, U32 v, U32 compare)
{
    __atomic_compare_exchange_n(p, &compare, v, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return compare;
}

U64 rrAtomicCmpXchg64(U64 volatile *p, U64 v, U64 compare)
{
    __atomic_compare_exchange_n(p, &compare, v, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return compare;
}

rrbool rrAtomicCAS32(U32 volatile *p, U32 *oldValue, U32 newValue)
{
    return __atomic_compare_exchange_n(p, oldValue, newValue, false,
                                       __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

rrbool rrAtomicCAS64(U64 volatile *p, U64 *oldValue, U64 newValue)
{
    return __atomic_compare_exchange_n(p, oldValue, newValue, false,
                                       __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

U32 rrAtomicExchange32(U32 volatile *p, U32 v) { return __atomic_exchange_n(p, v, __ATOMIC_SEQ_CST); }
U64 rrAtomicExchange64(U64 volatile *p, U64 v) { return __atomic_exchange_n(p, v, __ATOMIC_SEQ_CST); }
U32 rrAtomicAddExchange32(U32 volatile *p, S32 v) { return __atomic_fetch_add(p, (U32)v, __ATOMIC_SEQ_CST); }
U64 rrAtomicAddExchange64(U64 volatile *p, S64 v) { return __atomic_fetch_add(p, (U64)v, __ATOMIC_SEQ_CST); }
