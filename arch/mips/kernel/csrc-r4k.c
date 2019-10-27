/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2007 by Ralf Baechle
 */
#include <linux/clocksource.h>
#include <linux/init.h>

#include <asm/time.h>

static cycle_t c0_hpt_read(struct clocksource *cs)
{
	return read_c0_count();
}

static struct clocksource clocksource_mips = {
	.name		= "MIPS",
	.read		= c0_hpt_read,
	.mask		= CLOCKSOURCE_MASK(32),
	.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
};

static inline unsigned int rdhwr_count(void)
{
	unsigned int count;

	__asm__ __volatile__(
	"	.set push\n"
	"	.set mips32r2\n"
	"	rdhwr	%0, $2\n"
	"	.set pop\n"
	: "=r" (count));

	return count;
}

static bool rdhwr_count_usable(void)
{
	unsigned int prev, curr, i;

	/*
	 * Older QEMUs have a broken implementation of RDHWR for the CP0 count
	 * which always returns a constant value. Try to identify this and don't
	 * use it in the VDSO if it is broken. This workaround can be removed
	 * once the fix has been in QEMU stable for a reasonable amount of time.
	 */
	for (i = 0, prev = rdhwr_count(); i < 100; i++) {
		curr = rdhwr_count();

		if (curr != prev)
			return true;

		prev = curr;
	}

	pr_warn("Not using R4K clocksource in VDSO due to broken RDHWR\n");
	return false;
}

int __init init_r4k_clocksource(void)
{
	if (!cpu_has_counter || !mips_hpt_frequency)
		return -ENXIO;

	/* Calculate a somewhat reasonable rating value */
	clocksource_mips.rating = 200 + mips_hpt_frequency / 10000000;

	/*
	 * R2 onwards makes the count accessible to user mode so it can be used
	 * by the VDSO (HWREna is configured by configure_hwrena()).
	 */
	if (cpu_has_mips_r2_r6 && rdhwr_count_usable())
		clocksource_mips.archdata.vdso_clock_mode = VDSO_CLOCK_R4K;

	clocksource_register_hz(&clocksource_mips, mips_hpt_frequency);

	return 0;
}
