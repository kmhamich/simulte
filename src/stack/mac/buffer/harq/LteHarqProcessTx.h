//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

#ifndef _LTE_LTEHARQPROCESSTX_H_
#define _LTE_LTEHARQPROCESSTX_H_

#include "LteHarqUnitTx.h"

typedef std::vector<LteHarqUnitTx *> UnitVector;
typedef std::pair<unsigned char, TxHarqPduStatus> UnitStatus;
typedef std::vector<std::vector<UnitStatus> > BufferStatus;

/**
 * Container of H-ARQ units.
 * An H-ARQ process contains the units with id (acid + totalNumberOfProcesses * cw[i]), for each i.
 *
 * An H-ARQ buffer is made of H-ARQ processes.
 * An H-ARQ process is atomic for transmission, while H-ARQ units are atomic for
 * H-ARQ feedback.
 */
//TODO Aggiungere output di debug ripreso da old commenti
class LteHarqProcessTx
{
  protected:

    /// reference to mac module, used to handle errors
    LteMacBase *macOwner_;

    /// contained units vector
    UnitVector *units_;

    /// total number of processes in this H-ARQ buffer
    unsigned int numProcesses_;

    /// number of contained H-ARQ units
    unsigned char numHarqUnits_;

    /// H-ARQ process identifier
    unsigned char acid_;

    /// Number of empty units inside this process
    unsigned char numEmptyUnits_;

    /// Number of selected units inside this process
    unsigned int numSelected_;

  public:

    /*
     * Default Constructor
     */
    LteHarqProcessTx() {}

    /**
     * Crates a new H-ARQ process, which is a container of H-ARQ units.
     *
     * @param acid H-ARQ process identifier
     * @param numUnits number of units contained in this process (MAX_CODEWORDS)
     * @param numProcesses number of processes contained in the H-ARQ buffer.
     * @return
     */
    LteHarqProcessTx(unsigned char acid, unsigned int numUnits, unsigned int numProcesses, LteMacBase *macOwner,
        LteMacBase *dstMac);

    /**
     * Insert a pdu into an H-ARQ unit contained in this process.
     *
     * @param pdu pdu to be inserted
     * @param unitId id of destination unit
     */
    void insertPdu(LteMacPdu *pdu, Codeword cw);

    void markSelected(Codeword cw);

    LteMacPdu *extractPdu(Codeword cw);

    bool pduFeedback(HarqAcknowledgment fb, Codeword cw);

    bool selfNack(Codeword cw);

    /**
     *
     *
     * @returns
     */
    std::vector<UnitStatus> getProcessStatus();

    /**
     * Checks if this process has one or more H-ARQ unit ready for retransmission.
     *
     * @return true if there is at least one unit ready for rtx, false if none
     */
    bool hasReadyUnits();

    /**
     * Returns the tx time of the unit which is not retransmitting for
     * the longest period of time, inside this process (the oldest
     * pdu tx time).
     *
     * @return tx time of the oldest unit in this process
     */
    simtime_t getOldestUnitTxTime();

    /**
     * Returns a list of ids of ready for retransmission units of
     * this process.
     *
     * @return list of unit ids which are ready for rtx
     */
    CwList readyUnitsIds();

    /**
     * Returns a list of ids of empty units inside this process.
     *
     * @return empty units ids list.
     */
    CwList emptyUnitsIds();

    /**
     * Returns a list of ids of selected units.
     *
     * @return selected units ids list.
     */
    CwList selectedUnitsIds();

    /**
     * Tells if the process is empty: all of its units are in EMPTY state.
     *
     * @return true if the process is empty, false if it isn't
     */
    bool isEmpty();

    LteMacPdu *getPdu(Codeword cw);

    /**
     * This is necessary because when a pdu is in CORRECT state at the
     * corresponding H-ARQ rx process, it may be extracted and potentially
     * deleted, so pdu_ reference cannot be used to retrieve the pdu id
     * when feedback is received, because feedback is received 1 TTI after
     * pdu extraction at rx process!
     *
     * @param unitId unit id
     * @return pdu id
     */
    long getPduId(Codeword cw);

    void forceDropProcess();

    bool forceDropUnit(Codeword cw);

    unsigned int getNumHarqUnits()
    {
        return numHarqUnits_;
    }

    TxHarqPduStatus getUnitStatus(Codeword cw);

    // 1:1 getters
    void dropPdu(Codeword cw);
    bool isUnitEmpty(Codeword cw);
    bool isUnitReady(Codeword cw);
    unsigned char getTransmissions(Codeword cw);
    int64 getPduLength(Codeword cw);
    simtime_t getTxTime(Codeword cw);
    bool isUnitMarked(Codeword cw);

    virtual ~LteHarqProcessTx();

  protected:
};

#endif
