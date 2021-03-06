/*!
 * \file    VirtualDVMNode.h
 * \brief   DVM node for creating DVM bridges in LISA models
 * \date    Copyright 2011 ARM Limited. All rights reserved.
 *
 * Virtual DVM node to route DVM messages into, and out of, the PVBus network.
 */

#ifndef VIRTUALDVMNODE_H
#define VIRTUALDVMNODE_H

#include <string>

#include <pv/DVM.h>
#include <pv/DVMNodeSimple.h>

#include "protocol_DVMMessage.h"     // Autogenerated from LISA protocol


template <bool PVBUS_MASTER>
class VirtualDVMNode : public DVMNodeSimpleImpl
{
protected:
    protocol_DVMMessage*  lisa_bridge;

public:
    VirtualDVMNode(protocol_DVMMessage* lisa_bridge_, std::string const& name):
        lisa_bridge(lisa_bridge_)
    {
        setInstanceName(name);
    }

    // Broadcast upstream DVM message from PVBus network
    virtual DVM::error_response_t handleUpstreamDVMMessage(DVMNode*       from_,
                                                           DVM::id_t      id_,
                                                           DVM::Message*  message_)
    {
        if (PVBUS_MASTER)
            return lisa_bridge->send(message_);
        else
            return DVMNodeSimpleImpl::handleUpstreamDVMMessage(from_, id_, message_);
    }

    // Broadcast downstream DVM message from PVBus network
    virtual DVM::error_response_t handleDownstreamDVMMessage(DVMNode*       from_,
                                                             DVM::id_t      id_,
                                                             DVM::Message*  message_)
    {
        if (PVBUS_MASTER)
            return DVMNodeSimpleImpl::handleDownstreamDVMMessage(from_, id_, message_);
        else
            return lisa_bridge->send(message_);
    }

    // Insert DVM message into PVBus network
    DVM::error_response_t insertDVMMessage(DVM::id_t id,
                                           uint64_t addr,
                                           uint64_t additional_addr)
    {
        DVM::error_response_t  response;
        DVM::Message*          message_ = DVM::newMessageFromAxADDR(addr, additional_addr);

        if (message_ == 0)
            return DVM::error;

        if (PVBUS_MASTER)
            response = handleDownstreamDVMMessage(this, id, message_);
        else
            response = handleUpstreamDVMMessage(this, id, message_);

        delete message_;

        return response;
    }
};

#endif  // VIRTUALDVMNODE_H
