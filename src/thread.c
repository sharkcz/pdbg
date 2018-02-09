/* Copyright 2017 IBM Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <inttypes.h>
#include <stdio.h>

#include <bitutils.h>

#include <target.h>
#include <operations.h>

#include "main.h"

static int print_thread_status(struct pdbg_target *target, uint32_t index, uint64_t *status, uint64_t *unused1)
{
	*status = SETFIELD(0xf << (index * 4), *status, thread_status(target) & 0xf);
	return 1;
}

static int print_core_thread_status(struct pdbg_target *core_target, uint32_t index, uint64_t *unused, uint64_t *unused1)
{
	uint64_t status = -1UL;
	int i, rc;

	printf("c%02d:", index);
	rc = for_each_child_target("thread", core_target, print_thread_status, &status, NULL);
	for (i = 0; i < 8; i++)
		switch ((status >> (i * 4)) & 0xf) {
		case THREAD_STATUS_ACTIVE:
			printf(" A");
			break;

		case THREAD_STATUS_DOZE:
		case THREAD_STATUS_QUIESCE | THREAD_STATUS_DOZE:
			printf(" D");
			break;

		case THREAD_STATUS_NAP:
		case THREAD_STATUS_QUIESCE | THREAD_STATUS_NAP:
			printf(" N");
			break;

		case THREAD_STATUS_SLEEP:
		case THREAD_STATUS_QUIESCE | THREAD_STATUS_SLEEP:
			printf(" S");
			break;

		case THREAD_STATUS_ACTIVE | THREAD_STATUS_QUIESCE:
			printf(" Q");
			break;

		case 0xf:
			printf("  ");
			break;

		default:
			printf(" U");
			break;
		}
	printf("\n");

	return rc;
}

int print_proc_thread_status(struct pdbg_target *pib_target, uint32_t index, uint64_t *unused, uint64_t *unused1)
{
	printf("\np%01dt: 0 1 2 3 4 5 6 7\n", index);
	return for_each_child_target("core", pib_target, print_core_thread_status, NULL, NULL);
};

int start_thread(struct pdbg_target *thread_target, uint32_t index, uint64_t *unused, uint64_t *unused1)
{
	return ram_start_thread(thread_target) ? 0 : 1;
}

int step_thread(struct pdbg_target *thread_target, uint32_t index, uint64_t *count, uint64_t *unused1)
{
	return ram_step_thread(thread_target, *count) ? 0 : 1;
}

int stop_thread(struct pdbg_target *thread_target, uint32_t index, uint64_t *unused, uint64_t *unused1)
{
	return ram_stop_thread(thread_target) ? 0 : 1;
}

int sreset_thread(struct pdbg_target *thread_target, uint32_t index, uint64_t *unused, uint64_t *unused1)
{
	return ram_sreset_thread(thread_target) ? 0 : 1;
}


