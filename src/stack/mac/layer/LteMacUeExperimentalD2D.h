//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

#ifndef _LTE_LTEMACUEEXPERIMENTALD2D_H_
#define _LTE_LTEMACUEEXPERIMENTALD2D_H_

#include "LteMacUeExperimental.h"
#include "LteMacEnbExperimentalD2D.h"
#include "LteHarqBufferTxD2D.h"

class LteSchedulingGrant;
class LteSchedulerUeUl;
class LteBinder;

class LteMacUeExperimentalD2D : public LteMacUeExperimental
{
    // reference to the eNB
    LteMacEnbExperimentalD2D* enb_;

    // RAC Handling variables
    bool racD2DMulticastRequested_;
    // Multicast D2D BSR handling
    bool bsrD2DMulticastTriggered_;

    // if true, use the preconfigured TX params for transmission, else use that signaled by the eNB
    bool usePreconfiguredTxParams_;
    UserTxParams* preconfiguredTxParams_;
    UserTxParams* getPreconfiguredTxParams();  // build and return new user tx params

  protected:

    virtual int numInitStages() const { return 11; }
    /**
     * Reads MAC parameters for ue and performs initialization.
     */
    virtual void initialize(int stage);

    /**
     * Analyze gate of incoming packet
     * and call proper handler
     */
    virtual void handleMessage(cMessage *msg);

    /**
     * Main loop
     */
    virtual void handleSelfMessage();

    virtual void macHandleGrant(cPacket* pkt);

    void macHandleD2DModeSwitch(cPacket* pkt);

    virtual LteMacPdu* makeBsr(int size);

    /**
     * macPduMake() creates MAC PDUs (one for each CID)
     * by extracting SDUs from Real Mac Buffers according
     * to the Schedule List.
     * It sends them to H-ARQ (at the moment lower layer)
     *
     * On UE it also adds a BSR control element to the MAC PDU
     * containing the size of its buffer (for that CID)
     */
    virtual void macPduMake();

    virtual void updateUserTxParam(cPacket* pkt);

  public:
    LteMacUeExperimentalD2D();
    virtual ~LteMacUeExperimentalD2D();

    virtual bool isD2DCapable()
    {
        return true;
    }

    virtual void triggerBsr(MacCid cid)
    {
        if (connDesc_[cid].getDirection() == D2D_MULTI)
            bsrD2DMulticastTriggered_ = true;
        else
            bsrTriggered_ = true;
    }
};

#endif
