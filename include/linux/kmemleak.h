/*
 * include/linux/kmemleak.h
 *
 * Copyright (C) 2008 ARM Limited
 * Written by Catalin Marinas <catalin.marinas@arm.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef __KMEMLEAK_H
#define __KMEMLEAK_H

#include <linux/slab.h>
#include <linux/vmalloc.h>

#ifdef CONFIG_DEBUG_KMEMLEAK
extern int kmemleak_skip_disable;
extern unsigned int bsp_test_mode;

extern void kmemleak_init(void) __init;
extern void __kmemleak_alloc(const void *ptr, size_t size, int min_count,
			   gfp_t gfp) __ref;
extern void __kmemleak_alloc_percpu(const void __percpu *ptr, size_t size,
				  gfp_t gfp) __ref;
extern void __kmemleak_vmalloc(const struct vm_struct *area, size_t size,
			     gfp_t gfp) __ref;
extern void __kmemleak_free(const void *ptr) __ref;
extern void __kmemleak_free_part(const void *ptr, size_t size) __ref;
extern void __kmemleak_free_percpu(const void __percpu *ptr) __ref;
extern void __kmemleak_update_trace(const void *ptr) __ref;
extern void __kmemleak_not_leak(const void *ptr) __ref;
extern void __kmemleak_ignore(const void *ptr) __ref;
extern void __kmemleak_scan_area(const void *ptr, size_t size, gfp_t gfp) __ref;
extern void __kmemleak_no_scan(const void *ptr) __ref;
extern void __kmemleak_alloc_phys(phys_addr_t phys, size_t size, int min_count,
				gfp_t gfp) __ref;
extern void __kmemleak_free_part_phys(phys_addr_t phys, size_t size) __ref;
extern void __kmemleak_not_leak_phys(phys_addr_t phys) __ref;
extern void __kmemleak_ignore_phys(phys_addr_t phys) __ref;

static inline void kmemleak_alloc(const void *ptr, size_t size, int min_count,
				  gfp_t gfp)
{
	if (unlikely(bsp_test_mode && kmemleak_skip_disable))
		__kmemleak_alloc(ptr, size, min_count, gfp);
}

static inline void kmemleak_alloc_percpu(const void __percpu *ptr, size_t size,
					 gfp_t gfp)
{
	if (unlikely(bsp_test_mode && kmemleak_skip_disable))
		__kmemleak_alloc_percpu(ptr, size, gfp);
}

static inline void kmemleak_vmalloc(const struct vm_struct *area, size_t size,
				    gfp_t gfp)
{
	if (unlikely(bsp_test_mode && kmemleak_skip_disable))
		__kmemleak_vmalloc(area, size, gfp);
}

static inline void kmemleak_free(const void *ptr)
{
	if (unlikely(bsp_test_mode && kmemleak_skip_disable))
		__kmemleak_free(ptr);
}

static inline void kmemleak_free_part(const void *ptr, size_t size)
{
	if (unlikely(bsp_test_mode && kmemleak_skip_disable))
		__kmemleak_free_part(ptr, size);
}

static inline void kmemleak_free_percpu(const void __percpu *ptr)
{
	if (unlikely(bsp_test_mode && kmemleak_skip_disable))
		__kmemleak_free_percpu(ptr);
}

static inline void kmemleak_update_trace(const void *ptr)
{
	if (unlikely(bsp_test_mode && kmemleak_skip_disable))
		__kmemleak_update_trace(ptr);
}

static inline void kmemleak_not_leak(const void *ptr)
{
	if (unlikely(bsp_test_mode && kmemleak_skip_disable))
		__kmemleak_not_leak(ptr);
}

static inline void kmemleak_ignore(const void *ptr)
{
	if (unlikely(bsp_test_mode && kmemleak_skip_disable))
		__kmemleak_ignore(ptr);
}

static inline void kmemleak_scan_area(const void *ptr, size_t size, gfp_t gfp)
{
	if (unlikely(bsp_test_mode && kmemleak_skip_disable))
		__kmemleak_scan_area(ptr, size, gfp);
}

static inline void kmemleak_no_scan(const void *ptr)
{
	if (unlikely(bsp_test_mode && kmemleak_skip_disable))
		__kmemleak_no_scan(ptr);
}
static inline void kmemleak_alloc_phys(phys_addr_t phys, size_t size,
				       int min_count, gfp_t gfp)
{
	if (unlikely(bsp_test_mode && kmemleak_skip_disable))
		__kmemleak_alloc_phys(phys, size, min_count, gfp);
}

static inline void kmemleak_free_part_phys(phys_addr_t phys, size_t size)
{
	if (unlikely(bsp_test_mode && kmemleak_skip_disable))
		__kmemleak_free_part_phys(phys, size);
}

static inline void kmemleak_not_leak_phys(phys_addr_t phys)
{
	if (unlikely(bsp_test_mode && kmemleak_skip_disable))
		__kmemleak_not_leak_phys(phys);
}

static inline void kmemleak_ignore_phys(phys_addr_t phys)
{
	if (unlikely(bsp_test_mode && kmemleak_skip_disable))
		__kmemleak_not_leak_phys(phys);
}

static inline void kmemleak_alloc_recursive(const void *ptr, size_t size,
					    int min_count, unsigned long flags,
					    gfp_t gfp)
{
	if (!(flags & SLAB_NOLEAKTRACE))
		kmemleak_alloc(ptr, size, min_count, gfp);
}

static inline void kmemleak_free_recursive(const void *ptr, unsigned long flags)
{
	if (!(flags & SLAB_NOLEAKTRACE))
		kmemleak_free(ptr);
}

static inline void kmemleak_erase(void **ptr)
{
	if (unlikely(bsp_test_mode && kmemleak_skip_disable))
		*ptr = NULL;
}

#else

static inline void kmemleak_init(void)
{
}
static inline void kmemleak_alloc(const void *ptr, size_t size, int min_count,
				  gfp_t gfp)
{
}
static inline void kmemleak_alloc_recursive(const void *ptr, size_t size,
					    int min_count, unsigned long flags,
					    gfp_t gfp)
{
}
static inline void kmemleak_alloc_percpu(const void __percpu *ptr, size_t size,
					 gfp_t gfp)
{
}
static inline void kmemleak_vmalloc(const struct vm_struct *area, size_t size,
				    gfp_t gfp)
{
}
static inline void kmemleak_free(const void *ptr)
{
}
static inline void kmemleak_free_part(const void *ptr, size_t size)
{
}
static inline void kmemleak_free_recursive(const void *ptr, unsigned long flags)
{
}
static inline void kmemleak_free_percpu(const void __percpu *ptr)
{
}
static inline void kmemleak_update_trace(const void *ptr)
{
}
static inline void kmemleak_not_leak(const void *ptr)
{
}
static inline void kmemleak_ignore(const void *ptr)
{
}
static inline void kmemleak_scan_area(const void *ptr, size_t size, gfp_t gfp)
{
}
static inline void kmemleak_erase(void **ptr)
{
}
static inline void kmemleak_no_scan(const void *ptr)
{
}
static inline void kmemleak_alloc_phys(phys_addr_t phys, size_t size,
				       int min_count, gfp_t gfp)
{
}
static inline void kmemleak_free_part_phys(phys_addr_t phys, size_t size)
{
}
static inline void kmemleak_not_leak_phys(phys_addr_t phys)
{
}
static inline void kmemleak_ignore_phys(phys_addr_t phys)
{
}

#endif	/* CONFIG_DEBUG_KMEMLEAK */

#endif	/* __KMEMLEAK_H */