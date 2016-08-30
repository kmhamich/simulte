//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

#include "D2DModeSelectionBase.h"

void D2DModeSelectionBase::initialize(int stage)
{
    if (stage != 0)
        return;

    switchList_.clear();

    // get reference to mac layer
    mac_ = check_and_cast<LteMacEnbD2D*>(getParentModule()->getSubmodule("mac"));

    // get reference to the binder
    binder_ = getBinder();

    // get mode selection period
    modeSelectionPeriod_ = par("modeSelectionPeriod").doubleValue();
    if (modeSelectionPeriod_ < TTI)
        modeSelectionPeriod_ = TTI;

    // get the reference to the peering map in the binder
    peeringModeMap_ = binder_->getD2DPeeringModeMap();

    // Start mode selection tick
    modeSelectionTick_ = new cMessage("modeSelectionTick");
    scheduleAt(NOW + 0.05, modeSelectionTick_);
}

void D2DModeSelectionBase::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        if (strcmp(msg->getName(),"modeSelectionTick") == 0)
        {
            // run mode selection algorithm
            doModeSelection();

            // send switch notifications to selected flows
            sendModeSwitchNotifications();

            scheduleAt(NOW+modeSelectionPeriod_, msg);
        }
        else
            throw cRuntimeError("D2DModeSelectionBase::handleMessage - Unrecognized self message %s", msg->getName());
    }
    else
    {
         delete msg;
    }
}

void D2DModeSelectionBase::doModeSwitchAtHandover(MacNodeId nodeId)
{
    EV << NOW << " D2DModeSelectionBase::doModeSwitchAtHandover - Force mode switching for UE " << nodeId << " (handover)" << endl;

    switchList_.clear();
    std::map<MacNodeId, std::map<MacNodeId, LteD2DMode> >::iterator it = peeringModeMap_->begin();
    for (; it != peeringModeMap_->end(); ++it)
    {
        MacNodeId srcId = it->first;
        std::map<MacNodeId, LteD2DMode>::iterator jt = it->second.begin();
        for (; jt != it->second.end(); ++jt)
        {
            MacNodeId dstId = jt->first;
            if (srcId != nodeId && dstId != nodeId)
                continue;

            LteD2DMode oldMode = jt->second;
            if (oldMode == IM)
                continue;

            LteD2DMode newMode = IM;

            // add this flow to the list of flows to be switched
            FlowId p(srcId, dstId);
            FlowModeInfo info;
            info.flow = p;
            info.oldMode = oldMode;
            info.newMode = newMode;
            switchList_.push_back(info);

            // update peering map
            jt->second = newMode;

            EV << NOW << " D2DModeSelectionBase::doModeSwitchAtHandover - Flow: " << srcId << " --> " << dstId << " [" << d2dModeToA(newMode) << "]" << endl;
        }
    }

    // send switching command
    sendModeSwitchNotifications();
}


void D2DModeSelectionBase::sendModeSwitchNotifications()
{
    SwitchList::iterator it = switchList_.begin();
    for (; it != switchList_.end(); ++it)
    {
        MacNodeId srcId = it->flow.first;
        MacNodeId dstId = it->flow.second;
        LteD2DMode oldMode = it->oldMode;
        LteD2DMode newMode = it->newMode;

        mac_->sendModeSwitchNotification(srcId, dstId, oldMode, newMode);
    }
}
