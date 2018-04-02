/**
 * @file vfp.c
 * @brief Floting point extension for CortexA9
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 27.03.2018
 */

#include <stdint.h>

#include <util/log.h>
#include <util/math.h>

#include <embox/unit.h>
#include <framework/mod/options.h>
#include <kernel/printk.h>

EMBOX_UNIT_INIT(vfp_init);

static int vfp_init(void) {
	uint32_t val;
	uint32_t sid;
	/* Allow access to c10 & c11 coprocessors */
	asm volatile ("mrc p15, 0, %0, c1, c0, 2" : "=r" (val) :);
	val |= 0xf << 20;
	asm volatile ("mcr p15, 0, %0, c1, c0, 2" : : "r" (val));

	val = 1 << 30;
	/* Enable FPU extensions */
	asm volatile ("VMSR FPEXC, %0" : : "r" (val));

	asm volatile ("VMRS %0, FPEXC" : "=r" (sid));

	log_info("VPF info:\n"
			"\t\t\t %s\n"
			"\t\t\t Implementer =        0x%02x (%s)\n"
			"\t\t\t Subarch:             VFPv%d\n"
			"\t\t\t Part number =        0x%02x\n"
			"\t\t\t Variant     =        0x%02x\n"
			"\t\t\t Revision    =        0x%02x",
			(sid >> 23) & 1 ? "Software FP emulation" : "Hardware FP support",
			sid >> 24, ((sid >> 24) == 0x41) ? "ARM" : "Unknown",
			max((sid >> 16) & 0x7F, 3),
			(sid >> 8) & 0xFF,
			(sid >> 4) & 0xF,
			sid & 0xF);

	/* Disable access to c10 & c11 coprocessors */
	asm volatile ("mrc p15, 0, %0, c1, c0, 2" : "=r" (val) :);
	val &= ~(0xf << 20);
	asm volatile ("mcr p15, 0, %0, c1, c0, 2" : : "r" (val));

	return 0;
}
