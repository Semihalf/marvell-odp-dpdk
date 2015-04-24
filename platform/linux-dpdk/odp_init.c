/* Copyright (c) 2013, Linaro Limited
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#include <odp/api/cpu.h>
#include <odp/init.h>
#include <odp_internal.h>
#include <odp/debug.h>
#include <odp_packet_dpdk.h>
#include <odp_debug_internal.h>
#include <odp/system_info.h>

int odp_init_dpdk(void)
{
	int test_argc = 5;
	char *test_argv[6];
	int core_count, i, num_cores = 0;
	char core_mask[8];

	core_count  = odp_cpu_count();
	for (i = 0; i < core_count; i++)
		num_cores += (0x1 << i);
	sprintf(core_mask, "%x", num_cores);

	test_argv[0] = malloc(sizeof("odp_dpdk"));
	strcpy(test_argv[0], "odp_dpdk");
	test_argv[1] = malloc(sizeof("-c"));
	strcpy(test_argv[1], "-c");
	test_argv[2] = malloc(sizeof(core_mask));
	strcpy(test_argv[2], core_mask);
	test_argv[3] = malloc(sizeof("-n"));
	strcpy(test_argv[3], "-n");
	test_argv[4] = malloc(sizeof("3"));
	strcpy(test_argv[4], "3");

	if (rte_eal_init(test_argc, (char **)test_argv) < 0) {
		ODP_ERR("Cannot init the Intel DPDK EAL!");
		return -1;
	}

	if (rte_eal_pci_probe() < 0) {
		ODP_ERR("Cannot probe PCI\n");
		return -1;
	}

	return 0;
}

struct odp_global_data_s odp_global_data;

int odp_init_global(odp_init_t *params  ODP_UNUSED,
		    odp_platform_init_t *platform_params ODP_UNUSED)
{
	odp_global_data.log_fn = odp_override_log;
	odp_global_data.abort_fn = odp_override_abort;

	if (params != NULL) {
		if (params->log_fn != NULL)
			odp_global_data.log_fn = params->log_fn;
		if (params->abort_fn != NULL)
			odp_global_data.abort_fn = params->abort_fn;
	}

	odp_system_info_init();

	if (odp_init_dpdk()) {
		ODP_ERR("ODP dpdk init failed.\n");
		return -1;
	}

	if (odp_shm_init_global()) {
		ODP_ERR("ODP shm init failed.\n");
		return -1;
	}

	if (odp_thread_init_global()) {
		ODP_ERR("ODP thread init failed.\n");
		return -1;
	}

	if (odp_pool_init_global()) {
		ODP_ERR("ODP pool init failed.\n");
		return -1;
	}

	if (odp_queue_init_global()) {
		ODP_ERR("ODP queue init failed.\n");
		return -1;
	}

	if (odp_schedule_init_global()) {
		ODP_ERR("ODP schedule init failed.\n");
		return -1;
	}

	if (odp_pktio_init_global()) {
		ODP_ERR("ODP packet io init failed.\n");
		return -1;
	}

#if 0 /* for now timer is disabled */
	if (odp_timer_init_global()) {
		ODP_ERR("ODP timer init failed.\n");
		return -1;
	}
#endif

	if (odp_crypto_init_global()) {
		ODP_ERR("ODP crypto init failed.\n");
		return -1;
	}

	return 0;
}

int odp_term_global(void)
{
	int rc = 0;

	if (odp_crypto_term_global()) {
		ODP_ERR("ODP crypto term failed.\n");
		rc = -1;
	}

	if (odp_schedule_term_global()) {
		ODP_ERR("ODP schedule term failed.\n");
		rc = -1;
	}

	if (odp_queue_term_global()) {
		ODP_ERR("ODP queue term failed.\n");
		rc = -1;
	}

	if (odp_thread_term_global()) {
		ODP_ERR("ODP thread term failed.\n");
		rc = -1;
	}

	if (odp_shm_term_global()) {
		ODP_ERR("ODP shm term failed.\n");
		rc = -1;
	}

	return rc;
}

int odp_init_local(void)
{
	if (odp_thread_init_local()) {
		ODP_ERR("ODP thread local init failed.\n");
		return -1;
	}

	if (odp_pktio_init_local()) {
		ODP_ERR("ODP packet io local init failed.\n");
		return -1;
	}

	if (odp_schedule_init_local()) {
		ODP_ERR("ODP schedule local init failed.\n");
		return -1;
	}

	return 0;
}

int odp_term_local(void)
{
	int rc = 0;
	int rc_thd = 0;

	if (odp_schedule_term_local()) {
		ODP_ERR("ODP schedule local term failed.\n");
		rc = -1;
	}

	rc_thd = odp_thread_term_local();
	if (rc_thd < 0) {
		ODP_ERR("ODP thread local term failed.\n");
		rc = -1;
	} else {
		if (!rc)
			rc = rc_thd;
	}

	return rc;
}
