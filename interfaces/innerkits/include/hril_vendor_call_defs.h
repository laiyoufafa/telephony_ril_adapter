/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_RIL_VENDOR_CALL_DEFS_H
#define OHOS_RIL_VENDOR_CALL_DEFS_H

#include <stdint.h>
#include <stddef.h>

#include "hril_enum.h"
#include "hril_public_struct.h"

/* From 3GPP TS 27.007 V4.3.0 (2001-12)  AT+CGDCONT */
typedef struct {
    int index; /* Call identification number as described in 3GPP TS 22.030 [19] sub-clause 6.5.5.1.
                * This number can be used in +CHLD command operations */
    int dir; /* 0 mobile originated (MO) call
              * 1 mobile terminated (MT) call */
    HRilCallState state; /* call state */
    HRilCallMode mode; /* call mode */
    int mpty; /* 0 call is not one of multiparty (conference) call parties
               * 1 call is one of multiparty (conference) call parties */
    int voiceDomain; /* Identifies the service domain.
                      * 0: CS domain phone
                      * 1: IMS domain phone */
    int callType; /* 0: voice call
                   * 1: Video call: send one-way video, two-way voice
                   * 2: Video call: one-way receiving video, two-way voice
                   * 3: Video call: two-way video, two-way voice */
    char *number; /* phone number in format specified by <type> */

    int type; /* Type of address octet in integer format (refer TS 24.008 [8] subclauses 10.5.4.7);
               * default 145 when dialling string includes international access code character "+",
               * otherwise 129. see 3GPP TS 27.007 V4.3.0 (2001-12) 6.1 */
    char *alpha; /* alphanumeric representation of <number> corresponding to the entry found in phonebook;
                  * used character set should be the one selected with command select TE character set +CSCS */
} HRilCallInfo;

typedef struct {
    int smsSrvStatus; /* Service status of IMS SMS.
                       * 0: IMS SMS service is not available.
                       * 1: IMS SMS is in limited service.
                       * 2: IMS SMS is in full service. */
    int smsSrvRat; /* IMS SMS service domain.
                    * 0: IMS service is registered on LTE.
                    * 1: IMS service is registered on WIFI.
                    * 2: Reserve. */
    int voipSrvStatus; /* Service status of IMS VoIP.
                        * 0: IMS VoIP service is not available.
                        * 1: IMS VoIP is in limited service.
                        * 2: IMS VoIP is in full service. */
    int voipSrvRat; /* IMS VoIP service domain.
                     * 0: IMS service is registered on LTE.
                     * 1: IMS service is registered on WIFI.
                     * 2: Reserve. */
    int vtSrvStatus; /* Service status of IMS VT.
                      * 0: IMS VT service is not available.
                      * 1: IMS VT is in limited service.
                      * 2: IMS VT is in full service. */
    int vtSrvRat; /* IMS VT service domain.
                   * 0: IMS service is registered on LTE.
                   * 1: IMS service is registered on WIFI.
                   * 2: Reserve. */
    int vsSrvStatus; /* Service status of IMS VS.
                      * 0: IMS VS service is not available.
                      * 1: IMS VS is in limited service.
                      * 2: IMS VS is in full service. */
    int vsSrvRat; /* IMS VS service domain.
                   * 0: IMS service is registered on LTE.
                   * 1: IMS service is registered on WIFI.
                   * 2: Reserve. */
} HRilImsServiceStatus;

typedef struct {
    int callId; /* Call ID */
    int status; /* Indicates the call status of the corresponding callid.
                 * 0: Call originate(MO).
                 * 1: Call is Proceeding.
                 * 2: Alerting
                 * 3: Connected
                 * 4: Released
                 * 5: InComing
                 * 6: Waiting
                 * 7: Hold
                 * 8: Retrieve */
    int voiceDomain; /* Call domain for voice call.
                      * 0: CS domain voice call.
                      * 1: IMS domain voice call. */
} HRilCallStatusInfo;

typedef struct {
    int callId; /* Call ID, Value range: 1 ~ 7 */
    int duration; /* Call duration, unit: s. */
    int noCliCause; /* No caller number reason value.
                     * 0: The number is not available.
                     * 1: The user refused to provide a number.
                     * 2: Affected by other services.
                     * 3: Pay phone. */
    int ccCause; /* The value of the reason for the call error.
                  * Corresponds to the CS domain error code. */
} HRilCallEndInfo;

typedef struct {
    int callId; /* Call ID, Value range: 1 ~ 7 */
    int type; /* Call type.
               * 0: Voice call.
               * 9: Emergency call. */
} HRilCallConnectInfo;

typedef struct {
    char *type; /* business type..
                 * VOICE: Normal voice call.
                 * SYNC: Circuit domain 64kbit/s data service call (synchronous transparent mode).
                 * GPRS: PDP activation indication initiated by the network. */
    char *pdpType; /* Packet data protocol type.
                    * IP: Internet Protocol (IPV4).
                    * IPV6: IPV6 protocol.
                    * IPV4V6: IPV4 & IPV6 */
    char *pdpAddress; /* PDP address of IPV4 type. */
    char *l2p; /* Identifies the layer 2 protocol used between TE and MT. This parameter is ignored. */
    char *apn; /* Access point name, used to select GGSN or external packet data network;
                * if the value is empty, the contract value is used. */
} HRilCallCringInfo;

typedef struct {
    char *number; /* Phone number, format determined by < type >. */
    int type; /* Number address type.
               * 129: Normal number.
               * 145: International number (starting with "+"). */
    int businessClass; /* Business category, the value is a combination of the following business categories,
                        * and the default value is 255.
                        * 1: voice (telephony);
                        * 2: data;
                        * 4: fax (facsimile services);
                        * 8: short message service;
                        * 16: data circuit sync;
                        * 32: data circuit async;
                        * 64: dedicated packet access;
                        * 128: dedicated PAD access. */
} HRilCallWaitInfo;

/* From 3GPP TS 27.007 V4.3.0 (2001-12) ATD%s%s */
typedef struct {
    char *address; /* Type of address octet in integer format (refer TS 24.008 [8] subclauses 10.5.4.7);
                    * default 145 when dialling string includes international access code character "+", otherwise
                    * 129. */
    int clir; /* This command refers to CLIR service according to 3GPP TS 22.081 [3] that allows a calling
               * subscriber to enable or disable the presentation of the CLI to the called party when originating a
               * call. <n> (parameter sets the adjustment for outgoing calls): 0	presentation indicator is used
               * according to the subscription of the CLIR service 1	CLIR invocation 2	CLIR suppression <m>
               * (parameter shows the subscriber CLIR service status in the network): 0	CLIR not provisioned 1
               * CLIR provisioned in permanent mode 2	unknown (e.g. no network, etc.) 3	CLIR temporary mode
               * presentation restricted 4	CLIR temporary mode presentation allowed */
} HRilDial;

typedef struct {
    int reason; /* call forwarding type <0-5> */
    int mode; /* call forwarding operation mode */
    char *number; /* phone number */
    int classx; /* is a sum of integers each representing a class of information default 255
                 *  1	voice (telephony)
                 *  2	data (refers to all bearer services; with <mode>=2 this may refer only
                 *      to some bearer service if TA does not support values 16, 32, 64 and 128)
                 *  4	fax (facsimile services)
                 *  8	short message service
                 *  16	data circuit sync
                 *  32	data circuit async
                 *  64	dedicated packet access
                 *  128	dedicated PAD access
                 */
    int time; /* when "no reply" is enabled or queried, this gives the time in seconds to wait
               * before call is forwarded, default value 20
               */
    int status; /* 0: not active, 1: active */
} HRilCFInfo;

typedef struct {
    int callId;
    const char *dtmfKey;
    int onLength;
    int offLength;
    int stringLength;
} CallDtmfInfo;

typedef struct {
    const char *fac;
    int mode;
    const char *password;
} CallRestrictionInfo;

typedef struct {
    int status;
    int classx;
    char *number;
    int type; /* default 145 when dialling string includes international access code
               * character "+", otherwise 129,
               * other value refer TS 24.008 [8] sub-clause 10.5.4.7
               */
} HRilCFQueryInfo;

typedef struct {
    int action; /* parameter sets/shows the result code presentation status in the TA */
    int clipStat; /* parameter shows the subscriber CLIP service status in the network, <0-4> */
} HRilGetClipResult;

typedef struct {
    int action; /* parameter sets/shows the result code presentation status in the TA */
    int clirStat; /* parameter shows the subscriber CLIP service status in the network, <0-4> */
} HRilGetCallClirResult;

typedef struct {
    int status; /* 0 or 1 */
    /**
     * 1: voice (telephony);
     * 2: data;
     * 4: fax (facsimile services);
     * 8: short message service;
     * 16: data circuit sync;
     * 32: data circuit async;
     * 64: dedicated packet access;
     * 128: dedicated PAD access.
     */
    int classCw;
} HRilCallWaitResult;

typedef struct {
    int status; /* 0 or 1 */
    /**
     * 1: voice (telephony);
     * 2: data;
     * 4: fax (facsimile services);
     * 8: short message service;
     * 16: data circuit sync;
     * 32: data circuit async;
     * 64: dedicated packet access;
     * 128: dedicated PAD access.
     */
    int classCw;
} HRilCallRestrictionResult;

typedef struct {
    int32_t m; /* Integer value.
                            0: The network does not require a TE reply (USSD-Notify initiated by the network or TE
                            The network does not need further information after starting operation);
                            1: The network needs a TE reply (USSD-Request initiated by the network, or TE sent
                            After starting the operation, the network needs further information);
                            2: The USSD session is released by the network;
                            3: Other local clients have responded;
                            4: The operation is not supported;
                            5: The network timed out. */
    char *str; /* USSD string, the maximum length is 160 characters. */
    int32_t dcs; /* Integer value, encoding method.
                                15: Not specified (default 7bit encoding);
                                68: 8bit encoding (not supported temporarily);
                                72: UCS2 encoding (not supported temporarily). */
} HRilUssdCusdNoticeInfo;

typedef struct {
    int32_t index; /* Number index */
    int32_t total; /*  Total number of numbers */
    char* eccNum; /*  Emergency call number in string format */
    int32_t category; /*  Emergency call type
                                            0: interface message of initiating an emergency call
                                            1: Bandit police
                                            2: Rescue
                                            4: Fire alarm
                                            8: Marine Police
                                            16: Alpine rescue */
    int32_t simpresent; /*  the number is valid with or without a card
                                                0: valid without card
                                                1: Valid with card */
    char* mcc; /* Country code */
    int32_t abnormalService; /* distinguishes CS domain abnormal service status
                                                                0: all States are valid
                                                                1: the CS domain is not in normal service */
} HRilEmergencyinfo;

/*
 * Active reporting of SRVCC status is controlled by the +CIREP command.
 * This command complies with the 3GPP TS 27.007 protocol.
 */
typedef struct {
    /*
     * SRVCC status.
     *  1: SRVCC starts;
     *   2: SRVCC is successful;
     *   3: SRVCC is cancelled;
     *   4: SRVCC failed.
    */
    int32_t status;
} HRilCallSrvccStatus;

/*
 * Query the bandwidth information of the voice channel during the call.
 * Note: Modem private commands, not a reported field specified by the 3gpp protocol.
 */
typedef struct {
    /**
     * Integer value, voice channel type:
     *   0: No in-band audio information;
     *   1: In-band voice is available, narrowband voice;
     *   2: In-band voice is available, broadband voice.
     */
    int32_t status;
    /**
     * Integer value, call domain of voice call:
     *   0: 3GPP CS domain voice call;
     *   1: IMS domain voice call;
     *   2: 3GPP2 CS domain voice call.
     */
    int32_t voiceDomain;
} HRilCallCsChannelInfo;

typedef struct {
    void (*GetCallList)(const ReqDataInfo *requestInfo);
    void (*Dial)(const ReqDataInfo *requestInfo, const HRilDial *data, size_t dataLen);
    void (*Hangup)(const ReqDataInfo *requestInfo, const uint32_t *data, size_t dataLen);
    void (*Reject)(const ReqDataInfo *requestInfo);
    void (*Answer)(const ReqDataInfo *requestInfo);
    void (*GetClip)(const ReqDataInfo *requestInfo);
    void (*SetClip)(const ReqDataInfo *requestInfo, int action);
    void (*HoldCall)(const ReqDataInfo *requestInfo);
    void (*UnHoldCall)(const ReqDataInfo *requestInfo);
    void (*SwitchCall)(const ReqDataInfo *requestInfo);
    void (*CombineConference)(const ReqDataInfo *requestInfo, int callType);
    void (*SeparateConference)(const ReqDataInfo *requestInfo, int callIndex, int callType);
    void (*CallSupplement)(const ReqDataInfo *requestInfo, int type);
    void (*GetCallWaiting)(const ReqDataInfo *requestInfo);
    void (*SetCallWaiting)(const ReqDataInfo *requestInfo, int active);
    void (*SetCallTransferInfo)(const ReqDataInfo *requestInfo, HRilCFInfo info);
    void (*GetCallTransferInfo)(const ReqDataInfo *requestInfo, int reason);
    void (*GetCallRestriction)(const ReqDataInfo *requestInfo, const char *fac);
    void (*SetCallRestriction)(const ReqDataInfo *requestInfo, CallRestrictionInfo info);
    void (*GetClir)(const ReqDataInfo *requestInfo);
    void (*SetClir)(const ReqDataInfo *requestInfo, int action);
    void (*StartDtmf)(const ReqDataInfo *requestInfo, CallDtmfInfo info);
    void (*SendDtmf)(const ReqDataInfo *requestInfo, CallDtmfInfo info);
    void (*StopDtmf)(const ReqDataInfo *requestInfo, CallDtmfInfo info);
    void (*GetImsCallList)(const ReqDataInfo *requestInfo);
    void (*GetCallPreferenceMode)(const ReqDataInfo *requestInfo);
    void (*SetCallPreferenceMode)(const ReqDataInfo *requestInfo, int mode);
    void (*GetLteImsSwitchStatus)(const ReqDataInfo *requestInfo);
    void (*SetLteImsSwitchStatus)(const ReqDataInfo *requestInfo, int active);
    void (*SetUssdCusd)(const ReqDataInfo *requestInfo, const char *str);
    void (*GetUssdCusd)(const ReqDataInfo *requestInfo);
    void (*SetMute)(const ReqDataInfo *requestInfo, int mute);
    void (*GetMute)(const ReqDataInfo *requestInfo);
    void (*GetEmergencyCallList)(const ReqDataInfo *requestInfo);
    void (*GetCallFailReason)(const ReqDataInfo *requestInfo);
} HRilCallReq;
#endif // OHOS_RIL_VENDOR_CALL_DEFS_H
