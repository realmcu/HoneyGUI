/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file main.c
 * @brief Entry point of the browser (WebAssembly) simulator
 *
 * Counterpart of win32_sim/main.c. The desktop simulator starts the GUI
 * server thread and parks the main thread; in the browser the server loop
 * runs on the main thread itself and yields to the page every frame.
 */

#include <stdio.h>
#include "gui_server.h"
#include "web_sim.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("\n\t************** Realtek Semiconductor Corporation **************\n"
           "\t <HoneyGUI Web Simulator> Build Time: %s, %s\n\n", __DATE__, __TIME__);

    gui_server_init();
    web_sim_run_server();
    return 0;
}
