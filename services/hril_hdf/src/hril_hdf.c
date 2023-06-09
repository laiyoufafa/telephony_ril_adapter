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

#include "hril_hdf.h"

#include <stdlib.h>
#include <libudev.h>
#include <pthread.h>

#include "dfx_signal_handler.h"
#include "parameter.h"

#include "modem_adapter.h"
#include "telephony_log_c.h"

#define RIL_VENDOR_LIB_PATH "persist.sys.radio.vendorlib.path"
#define BASE_HEX 16

static struct HRilReport g_reportOps = {
    OnCallReport,
    OnDataReport,
    OnModemReport,
    OnNetworkReport,
    OnSimReport,
    OnSmsReport,
    OnTimerCallback
};

static int32_t GetVendorLibPath(char *path)
{
    int32_t code = GetParameter(RIL_VENDOR_LIB_PATH, "", path, PARAMETER_SIZE);
    if (code <= 0) {
        TELEPHONY_LOGE("Failed to get vendor library path through system properties. err:%{public}d", code);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static UsbDeviceInfo *GetPresetInformation(const char *vId, const char *pId)
{
    if (vId == NULL || pId == NULL) {
        return NULL;
    }
    char *out = NULL;
    UsbDeviceInfo *uDevInfo = NULL;
    int32_t idVendor = (int32_t)strtol(vId, &out, BASE_HEX);
    int32_t idProduct = (int32_t)strtol(pId, &out, BASE_HEX);
    for (uint32_t i = 0; i < sizeof(g_usbModemVendorInfo) / sizeof(UsbDeviceInfo); i++) {
        if (g_usbModemVendorInfo[i].idVendor == idVendor && g_usbModemVendorInfo[i].idProduct == idProduct) {
            TELEPHONY_LOGI("list index:%{public}d", i);
            uDevInfo = &g_usbModemVendorInfo[i];
            break;
        }
    }
    return uDevInfo;
}

static UsbDeviceInfo *GetUsbDeviceInfo(void)
{
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;
    UsbDeviceInfo *uDevInfo = NULL;

    udev = udev_new();
    if (udev == NULL) {
        TELEPHONY_LOGE("Can't create udev");
        return uDevInfo;
    }
    enumerate = udev_enumerate_new(udev);
    if (enumerate == NULL) {
        TELEPHONY_LOGE("Can't create enumerate");
        udev_unref(udev);
        return uDevInfo;
    }
    udev_enumerate_add_match_subsystem(enumerate, "tty");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);
    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path = udev_list_entry_get_name(dev_list_entry);
        if (path == NULL) {
            continue;
        }
        dev = udev_device_new_from_syspath(udev, path);
        if (dev == NULL) {
            continue;
        }
        dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
        if (!dev) {
            TELEPHONY_LOGE("Unable to find parent usb device.");
            return uDevInfo;
        }
        const char *cIdVendor = udev_device_get_sysattr_value(dev, "idVendor");
        const char *cIdProduct = udev_device_get_sysattr_value(dev, "idProduct");
        uDevInfo = GetPresetInformation(cIdVendor, cIdProduct);
        udev_device_unref(dev);
        if (uDevInfo != NULL) {
            break;
        }
    }
    udev_enumerate_unref(enumerate);
    udev_unref(udev);
    return uDevInfo;
}

static void LoadVendor(void)
{
    const char *rilLibPath = NULL;
    char vendorLibPath[PARAMETER_SIZE] = {0};
    // Pointer to ril init function in vendor ril
    const HRilOps *(*rilInitOps)(const struct HRilReport *) = NULL;
    // functions returned by ril init function in vendor ril
    const HRilOps *ops = NULL;

    UsbDeviceInfo *uDevInfo = GetUsbDeviceInfo();
    if (GetVendorLibPath(vendorLibPath) == HDF_SUCCESS) {
        rilLibPath = vendorLibPath;
    } else if (uDevInfo != NULL) {
        rilLibPath = uDevInfo->libPath;
    } else {
        TELEPHONY_LOGI("use default vendor lib.");
        rilLibPath = g_usbModemVendorInfo[DEFAULT_MODE_INDEX].libPath;
    }
    if (rilLibPath == NULL) {
        TELEPHONY_LOGE("dynamic library path is empty");
        return;
    }

    TELEPHONY_LOGI("RilInit LoadVendor start with rilLibPath:%{public}s", rilLibPath);
    g_dlHandle = dlopen(rilLibPath, RTLD_NOW);
    if (g_dlHandle == NULL) {
        TELEPHONY_LOGE("dlopen %{public}s is fail. %{public}s", rilLibPath, dlerror());
        return;
    }
    rilInitOps = (const HRilOps *(*)(const struct HRilReport *))dlsym(g_dlHandle, "RilInitOps");
    if (rilInitOps == NULL) {
        dlclose(g_dlHandle);
        TELEPHONY_LOGE("RilInit not defined or exported");
        return;
    }
    ops = rilInitOps(&g_reportOps);
    HRilRegOps(ops);
    TELEPHONY_LOGI("HRilRegOps completed");
}

static int32_t RilAdapterDispatch(
    struct HdfDeviceIoClient *client, int32_t cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    static pthread_mutex_t dispatchMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&dispatchMutex);
    TELEPHONY_LOGI("RilAdapterDispatch cmd:%{public}d", cmd);
    ret = DispatchRequest(cmd, data);
    pthread_mutex_unlock(&dispatchMutex);
    return ret;
}

static struct IDeviceIoService g_rilAdapterService = {
    .Dispatch = RilAdapterDispatch,
    .Open = NULL,
    .Release = NULL,
};

static int32_t RilAdapterBind(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    device->service = &g_rilAdapterService;
    return HDF_SUCCESS;
}

static int32_t RilAdapterInit(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    DFX_InstallSignalHandler();
    struct HdfSBuf *sbuf = HdfSbufTypedObtain(SBUF_IPC);
    if (sbuf == NULL) {
        TELEPHONY_LOGE("HdfSampleDriverBind, failed to obtain ipc sbuf");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (!HdfSbufWriteString(sbuf, "string")) {
        TELEPHONY_LOGE("HdfSampleDriverBind, failed to write string to ipc sbuf");
        HdfSbufRecycle(sbuf);
        return HDF_FAILURE;
    }
    if (sbuf != NULL) {
        HdfSbufRecycle(sbuf);
    }
    TELEPHONY_LOGI("sbuf IPC obtain success!");
    LoadVendor();
    return HDF_SUCCESS;
}

static void RilAdapterRelease(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        return;
    }
    dlclose(g_dlHandle);
}

struct HdfDriverEntry g_rilAdapterDevEntry = {
    .moduleVersion = 1,
    .moduleName = "hril_hdf",
    .Bind = RilAdapterBind,
    .Init = RilAdapterInit,
    .Release = RilAdapterRelease,
};
HDF_INIT(g_rilAdapterDevEntry);