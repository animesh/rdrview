/*
 * Sandbox initialization for each supported system
 *
 * Copyright (C) 2020 Ernesto A. Fernández <ernesto.mnd.fernandez@gmail.com>
 *
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

#include <errno.h>
#include "rdrview.h"

#if defined(__linux__)

#include <seccomp.h>

static void do_start_sandbox(void)
{
	scmp_filter_ctx ctx;
	bool fail = false;

	ctx = seccomp_init(SCMP_ACT_ERRNO(EPERM));
	if (!ctx)
		fatal_msg("failed to start the sandbox");

	fail |= seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(brk), 0);
	fail |= seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
	fail |= seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(dup), 0);
	fail |= seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(dup2), 0);
	fail |= seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0);
	fail |= seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);
	fail |= seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(close), 0);
	fail |= seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(fstat), 0);
	fail |= seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mmap), 0);
	fail |= seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mremap), 0);
	fail |= seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(munmap), 0);

	fail |= seccomp_load(ctx);
	if (fail)
		fatal_msg("failed to load the seccomp rules");
	seccomp_release(ctx);
}

#else /* __linux__ */

static void do_start_sandbox(void)
{
	fatal_msg("no sandbox for your system - disable it at your own risk");
}

#endif /* __linux__ */

/**
 * Restrict the process to working with its existing temporary files
 */
void start_sandbox(void)
{
	/*
	 * If a different version of a library is conflicting with the sandbox,
	 * the user must be trusted to decide if it's wise to disable it. The man
	 * page includes an admonition against it, for the less tech-savvy.
	 */
	if (!options.disable_sandbox)
		do_start_sandbox();
}
