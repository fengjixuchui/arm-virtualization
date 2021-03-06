/*
 * AMBA-PV: amba_pv_ace_protocol_checker.h - AMBA-PV ACE protocol checker model.
 *
 * Copyright 2009 ARM Limited.
 * All rights reserved.
 */

#ifndef AMBA_PV_ACE_PROTOCOL_CHECKER__H
#define AMBA_PV_ACE_PROTOCOL_CHECKER__H

/**
 * @file        amba_pv_ace_protocol_checker.h
 *
 * @brief       AMBA-PV protocol checker model.
 */

/* Includes */
#include "bus/amba_pv_extension.h"
#include "sockets/amba_pv_ace_master_socket.h"
#include "sockets/amba_pv_ace_slave_socket.h"
#include "models/amba_pv_protocol_checker_base.h"

/* Namespaces */

/**
 * @brief       AMBA-PV namespace.
 */
namespace amba_pv {

/* Datatypes */

/**
 * @brief       AMBA-PV ACE protocol checker model.
 *
 * The amba_pv_ace_protocol_checker model is used for confirming that your model
 * complies with the AMBA-PV ACE protocol.
 *
 * You can instantiate the protocol checker between any pair of AMBA-PV ACE 
 * master and slave sockets. The transactions that pass through are checked against
 * the AMBA-PV ACE protocol and errors reported using the SystemC reporting
 * mechanism. All errors are reported with a message type of
 * @c "amba_pv_ace_protocol_checker" and with a severity of @c SC_ERROR.
 * Recommendations are reported with a severity of @c SC_WARNING.
 *
 * @note        The AMBA-PV protocol checker model does not perform any OSCI
 *              TLM 2.0 BP checks.
 *
 * @note        The AMBA-PV protocol checker model might have an effect on
 *              performance.
 *
 * @param       BUSWIDTH bus width in bits as one of 8, 16, 32, 64, 128, 256,
 *              512, or 1024. Defaults to 64.
 */
template<unsigned int BUSWIDTH = 64>
class amba_pv_ace_protocol_checker:
    public virtual amba_pv_fw_transport_if,
    public virtual amba_pv_bw_transport_and_snoop_if,
    public amba_pv_protocol_checker_base<BUSWIDTH>
{
public:
        /**
         * @brief Slave socket.
         */
    amba_pv_ace_slave_socket<BUSWIDTH> amba_pv_s;

        /**
         * @brief Master socket.
         */
    amba_pv_ace_master_socket<BUSWIDTH> amba_pv_m;

    /* Construction */
    explicit amba_pv_ace_protocol_checker(const sc_core::sc_module_name &,
                                          bool /* recommend_on */ = true);

protected:
    /* Forward interface */
    virtual void b_transport(int, amba_pv_transaction &, sc_core::sc_time &);

    virtual unsigned int transport_dbg(int, amba_pv_transaction &);

    virtual bool get_direct_mem_ptr(int, amba_pv_transaction &, tlm::tlm_dmi &);

    /* Backward interface */
    virtual void invalidate_direct_mem_ptr(int, sc_dt::uint64, sc_dt::uint64);

    virtual void b_snoop(int, amba_pv_transaction &, sc_core::sc_time &);

    virtual unsigned int snoop_dbg(int, amba_pv_transaction &);
};

/* Functions */

/**
 * @brief       Constructor.
 *
 * Constructs a new amba_pv_ace_protocol_checker with parameter for configuring
 * recommended rules.
 *
 * @param       name protocol checker name.
 * @param       recommend_on @c true to enable reporting of protocol
 *              recommendations, @c false otherwise.
 *
 * @see         recommend_on()
 */
template<unsigned int BUSWIDTH>
inline
amba_pv_ace_protocol_checker<BUSWIDTH>::amba_pv_ace_protocol_checker(const sc_core::sc_module_name & name,
                                                                     bool recommend_on /* = true */):
    amba_pv_protocol_checker_base<BUSWIDTH>(name, recommend_on, AMBA_PV_ACE),
    amba_pv_s("amba_pv_ace_s"),
    amba_pv_m("amba_pv_ace_m")
{
    /* Bindings... */
    amba_pv_s(*this);
    amba_pv_m(*this);
}

/**
 * @brief       Blocking transport.
 *
 * This version of the method completes the transaction and checks it complies
 * with the AMBA buses protocols.
 */
template<unsigned int BUSWIDTH>
inline void
amba_pv_ace_protocol_checker<BUSWIDTH>::b_transport(int socket_id,
                                                    amba_pv_transaction & trans,
                                                    sc_core::sc_time & t) 
{
    amba_pv_extension * ex = NULL;
    
    /* Retrieve AMBA-PV extension */
    trans.get_extension(ex);

    /* Perform extension checks */
    this->extension_checks(ex);

    /* Perform control checks */
    this->control_checks(ex);

    /* Perform addressing checks */
    this->addressing_checks(trans, ex);

    /* Perform data checks */
    this->data_checks(trans, ex);

    /* Perform exclusive access checks */
    this->exclusive_checks(trans, ex);

    /* Perform cacheability checks */
    this->cacheability_checks(trans, ex);

    /* Transaction proceeds */
    amba_pv_m->b_transport(trans, t);

    /* Perform response checks */
    this->response_checks(trans, ex);
}

/**
 * @brief       Debug access to a target.
 *
 * This version of the method forwards this debug access to the slave and
 * checks it complies with the AMBA buses protocols.
 */
template<unsigned int BUSWIDTH>
inline unsigned int
amba_pv_ace_protocol_checker<BUSWIDTH>::transport_dbg(int socket_id,
                                                      amba_pv_transaction & trans)
{
    amba_pv_extension * ex = NULL;

    /* Retrieve AMBA-PV extension */
    trans.get_extension(ex);

    /* Perform control checks */
    this->control_checks(ex);

    /* Debug transaction proceeds */
    return amba_pv_m->transport_dbg(trans);
}

/**
 * @brief       Requests a DMI access based on the specified transaction.
 *
 * This version of the method forwards this DMI access request to the slave and
 * checks it complies with the AMBA buses protocols.
 */
template<unsigned int BUSWIDTH>
inline bool
amba_pv_ace_protocol_checker<BUSWIDTH>::get_direct_mem_ptr(int socket_id,
                                                           amba_pv_transaction & trans,
                                                           tlm::tlm_dmi & dmi_data)
{
    amba_pv_extension * ex = NULL;

    /* Retrieve AMBA-PV extension */
    trans.get_extension(ex);

    /* Perform extension checks */
    this->extension_checks(ex);

    /* Perform control checks */
    this->control_checks(ex);

    /* Perform addressing checks */
    this->addressing_checks(trans, ex);

    /* DMI request proceeds */
    return amba_pv_m->get_direct_mem_ptr(trans, dmi_data);
}

/**
 * @brief       Invalidates DMI pointers previously established for the
 *              specified DMI region.
 *
 * This version of the method forwards this DMI call to the master.
 */
template<unsigned int BUSWIDTH>
inline void
amba_pv_ace_protocol_checker<BUSWIDTH>::invalidate_direct_mem_ptr(int socket_id,
                                                                  sc_dt::uint64 start_range,
                                                                  sc_dt::uint64 end_range)
{
    amba_pv_s->invalidate_direct_mem_ptr(start_range, end_range);
}

/**
 * @brief       Blocking snoop.
 *
 * This version of the method completes the snoop transaction and checks it complies
 * with the AMBA buses protocols.
 */
template<unsigned int BUSWIDTH>
inline void
amba_pv_ace_protocol_checker<BUSWIDTH>::b_snoop(int socket_id,
                                                amba_pv_transaction& trans,
                                                sc_core::sc_time& t)
{
    amba_pv_extension* ex = NULL;
    
    // Retrieve AMBA-PV extension
    trans.get_extension(ex);

    switch(ex->get_snoop())
    {
    case AMBA_PV_READ_ONCE:
    case AMBA_PV_READ_SHARED:
    case AMBA_PV_READ_CLEAN:
    case AMBA_PV_READ_NOT_SHARED_DIRTY:
    case AMBA_PV_READ_UNIQUE:
    case AMBA_PV_CLEAN_SHARED:
    case AMBA_PV_CLEAN_INVALID:
    case AMBA_PV_MAKE_INVALID:
    case AMBA_PV_DVM_COMPLETE:
    case AMBA_PV_DVM_MESSAGE:
        break;

    default:
        SC_REPORT_ERROR(this->kind(),
                        "Snoop transaction type must be "
                        "ReadOnce, ReadShared, ReadClean, "
                        "ReadNotSharedDirty, ReadUnique, "
                        "CleanShared, CleanInvalid, MakeInvalid, "
                        "DVMComplete or DVMMessage" );
        break;
    }

    // Transaction proceeds
    amba_pv_s.b_snoop(trans, t);
}


/**
 * @brief       Debug access to a master.
 *
 * This version of the method forwards this debug access to the master.
 */
template<unsigned int BUSWIDTH>
inline unsigned int
amba_pv_ace_protocol_checker<BUSWIDTH>::snoop_dbg(int socket_id,
                                                  amba_pv_transaction& trans)
{
    // Debug transaction proceeds
    return amba_pv_s.snoop_dbg(trans);
}


}   /* namespace amba_pv */

#endif  /* defined(AMBA_PV_PROTOCOL_CHECKER__H) */
