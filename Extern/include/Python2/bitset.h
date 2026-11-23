
#ifndef Py_BITSET_H
#define Py_BITSET_H
#ifdef __cplusplus
extern "C" {
#endif

/* Bitset interface */

/* Keep Python's byte type private: a BYTE macro breaks C++ functional casts. */
typedef unsigned char Py_BITSET_BYTE;
/* Avoid colliding with std::bitset when the Python 2 C API is included by C++. */
typedef Py_BITSET_BYTE *Py_bitset;

Py_bitset newbitset(int nbits);
void delbitset(Py_bitset bs);
#define testbit(ss, ibit) (((ss)[BIT2BYTE(ibit)] & BIT2MASK(ibit)) != 0)
int addbit(Py_bitset bs, int ibit); /* Returns 0 if already set */
int samebitset(Py_bitset bs1, Py_bitset bs2, int nbits);
void mergebitset(Py_bitset bs1, Py_bitset bs2, int nbits);

#define BITSPERBYTE	(8*sizeof(Py_BITSET_BYTE))
#define NBYTES(nbits)	(((nbits) + BITSPERBYTE - 1) / BITSPERBYTE)

#define BIT2BYTE(ibit)	((ibit) / BITSPERBYTE)
#define BIT2SHIFT(ibit)	((ibit) % BITSPERBYTE)
#define BIT2MASK(ibit)	(1 << BIT2SHIFT(ibit))
#define BYTE2BIT(ibyte)	((ibyte) * BITSPERBYTE)

#ifdef __cplusplus
}
#endif
#endif /* !Py_BITSET_H */
