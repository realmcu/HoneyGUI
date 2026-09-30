/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Radio values for the simulator. Each value is a global: a set stores it,
 * a get returns it. A device port drives the BT and Wi-Fi controllers,
 * which usually answer later; see gui_port_sysval_async.c for that shape.
 */

#include <stddef.h>
#include "gui_components_init.h"
#include "gui_sysval_adapter.h"

static bool bt_enabled = true;
static bool bt_connected = true;
static bool wifi_enabled = false;
static bool wifi_connected = false;

static void respond_bool(gui_sysval_request_t *request, bool b)
{
    gui_sysval_value_t value = gui_sysval_value_bool(b);

    gui_sysval_respond(request, GUI_SYSVAL_STATUS_OK, &value);
}

static void port_sysval_bt_enabled_get(gui_sysval_request_t *request)
{
    respond_bool(request, bt_enabled);
}

static void port_sysval_bt_enabled_set(const gui_sysval_value_t *value,
                                       gui_sysval_request_t *request)
{
    bt_enabled = value->data.b;
    if (!bt_enabled)
    {
        bt_connected = false;
    }
    respond_bool(request, bt_enabled);
}

static void port_sysval_bt_connected_get(gui_sysval_request_t *request)
{
    respond_bool(request, bt_connected);
}

static void port_sysval_wifi_enabled_get(gui_sysval_request_t *request)
{
    respond_bool(request, wifi_enabled);
}

static void port_sysval_wifi_enabled_set(const gui_sysval_value_t *value,
                                         gui_sysval_request_t *request)
{
    wifi_enabled = value->data.b;
    if (!wifi_enabled)
    {
        wifi_connected = false;
    }
    respond_bool(request, wifi_enabled);
}

static void port_sysval_wifi_connected_get(gui_sysval_request_t *request)
{
    respond_bool(request, wifi_connected);
}

static gui_sysval_adapter_t bt_enabled_adapter =
{
    .key = GUI_SYSVAL_KEY_CONNECTIVITY_BLUETOOTH_ENABLED,
    .type = GUI_SYSVAL_TYPE_BOOL,
    .get_request = port_sysval_bt_enabled_get,
    .set_request = port_sysval_bt_enabled_set,
};

static gui_sysval_adapter_t bt_connected_adapter =
{
    .key = GUI_SYSVAL_KEY_CONNECTIVITY_BLUETOOTH_CONNECTED,
    .type = GUI_SYSVAL_TYPE_BOOL,
    .get_request = port_sysval_bt_connected_get,
};

static gui_sysval_adapter_t wifi_enabled_adapter =
{
    .key = GUI_SYSVAL_KEY_CONNECTIVITY_WIFI_ENABLED,
    .type = GUI_SYSVAL_TYPE_BOOL,
    .get_request = port_sysval_wifi_enabled_get,
    .set_request = port_sysval_wifi_enabled_set,
};

static gui_sysval_adapter_t wifi_connected_adapter =
{
    .key = GUI_SYSVAL_KEY_CONNECTIVITY_WIFI_CONNECTED,
    .type = GUI_SYSVAL_TYPE_BOOL,
    .get_request = port_sysval_wifi_connected_get,
};

static int port_sysval_connectivity_init(void)
{
    gui_sysval_adapter_register(&bt_enabled_adapter);
    gui_sysval_adapter_register(&bt_connected_adapter);
    gui_sysval_adapter_register(&wifi_enabled_adapter);
    gui_sysval_adapter_register(&wifi_connected_adapter);
    return 0;
}

GUI_INIT_DEVICE_EXPORT(port_sysval_connectivity_init);
