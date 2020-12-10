#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "page_table_driver.h"

#define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)

#define PL_SIZE			512UL
#define PL_MASK			(PL_SIZE - 1)
#define PML4_OFFSET		39
#define PML4_MASK		(PL_MASK << PML4_OFFSET)
#define PDPT_OFFSET		30
#define PDPT_MASK		(PL_MASK << PDPT_OFFSET)
#define PD_OFFSET		21
#define PD_MASK			(PL_MASK << PD_OFFSET)
#define PT_OFFSET		12
#define PT_MASK			(PL_MASK << PT_OFFSET)
#define OFFSET_MASK		(4096U - 1)

#define SET_OFFSET(v, off)	(((v) & 0x3FFFFFFFFFFF000UL) | ((off) & 4095UL))

struct pt_walk_result {
	uint64_t cr3;
	uint64_t pml4_addr;
	uint64_t pml4_data;
	uint64_t pdpt_addr;
	uint64_t pdpt_data;
	uint64_t pd_addr;
	uint64_t pd_data;
	uint64_t pt_addr;
	uint64_t pt_data;
	uint64_t phy_addr;
};

int fd;

// Obtain my cr3 value (a.k.a. PML4 table physical address)
uint64_t get_cr3_value()
{
	struct ioctl_arg cmd;
	int ret;
	cmd.request[0] = IO_CR3;
	ret = ioctl(fd, IOCTL_REQUEST, &cmd);
	return cmd.ret;
}

// Given a physical address, return the value
uint64_t read_physical_address(uint64_t physical_address)
{
	struct ioctl_arg cmd;
	int ret;
	cmd.request[0] = IO_READ;
	cmd.request[1] = physical_address;
	ret = ioctl(fd, IOCTL_REQUEST, &cmd);
	return cmd.ret;
}

// Write value to a physical address
void write_physical_address(uint64_t physical_address, uint64_t value)
{
	struct ioctl_arg cmd;
	int ret;
	cmd.request[0] = IO_WRITE;
	cmd.request[1] = physical_address;
	cmd.request[2] = value;
	ret = ioctl(fd, IOCTL_REQUEST, &cmd);
}

static uint64_t analyze_physic_address(const void *p, struct pt_walk_result *result)
{
	if (p == NULL) {
		return 0;
	}
	struct pt_walk_result r;

	eprintf("Virtual address: 0x%p\n", p);
	uint64_t vaddr = (uint64_t)p;
	uint64_t pml4_off = (vaddr & PML4_MASK) >> PML4_OFFSET;
	uint64_t pdpt_off = (vaddr & PDPT_MASK) >> PDPT_OFFSET;
	uint64_t pd_off = (vaddr & PD_MASK) >> PD_OFFSET;
	uint64_t pt_off = (vaddr & PT_MASK) >> PT_OFFSET;
	uint64_t va_off = vaddr & OFFSET_MASK;
	eprintf("PML4 offset: 0x%lx\n", pml4_off);
	eprintf("PDPT offset: 0x%lx\n", pdpt_off);
	eprintf("  PD offset: 0x%lx\n", pd_off);
	eprintf("  PT offset: 0x%lx\n", pt_off);
	eprintf("     offset: 0x%lx\n", va_off);

	r.cr3 = get_cr3_value();

	r.pml4_addr = SET_OFFSET(r.cr3, pml4_off * sizeof(uint64_t));
	r.pml4_data = read_physical_address(r.pml4_addr);

	r.pdpt_addr = SET_OFFSET(r.pml4_data, pdpt_off * sizeof(uint64_t));
	r.pdpt_data = read_physical_address(r.pdpt_addr);

	r.pd_addr = SET_OFFSET(r.pdpt_data, pd_off * sizeof(uint64_t));
	r.pd_data = read_physical_address(r.pd_addr);

	r.pt_addr = SET_OFFSET(r.pd_data, pt_off * sizeof(uint64_t));
	r.pt_data = read_physical_address(r.pt_addr);

	r.phy_addr = SET_OFFSET(r.pt_data, va_off);

	if (result) {
		memcpy(result, &r, sizeof(struct pt_walk_result));
	}
	return r.phy_addr;
}

static void dump_pt_walk_result(const struct pt_walk_result *r)
{
	if (r == NULL) {
		return;
	}

	eprintf("CR3: \t\t0x%16lx\n", r->cr3);
	eprintf("PML4 address:\t0x%16lx\n", r->pml4_addr);
	eprintf("PML4 data:\t0x%16lx\n", r->pml4_data);
	eprintf("PDPT address:\t0x%16lx\n", r->pdpt_addr);
	eprintf("PDPT data:\t0x%16lx\n", r->pdpt_data);
	eprintf("  PD address:\t0x%16lx\n", r->pd_addr);
	eprintf("  PD data:\t0x%16lx\n", r->pd_data);
	eprintf("  PT address:\t0x%16lx\n", r->pt_addr);
	eprintf("  PT data:\t0x%16lx\n", r->pt_data);
	eprintf("Physic address:\t0x%16lx\n", r->phy_addr);
}

int main()
{
	char *x = (char *)aligned_alloc(4096, 4096) + 0x123;
	char *y = (char *)aligned_alloc(4096, 4096) + 0x123;
	strcpy(x, "This is a simple HW.");
	strcpy(y, "You have to modify my page table.");

	fd = open("/dev/os", O_RDONLY);
	if (fd < 0) {
		printf("Cannot open device!\n");
		return 0;
	}

	struct pt_walk_result x_pt, y_pt;
	eprintf("------ Page table walk for x ------\n");
	analyze_physic_address(x, &x_pt);
	dump_pt_walk_result(&x_pt);
	eprintf("\n");
	eprintf("------ Page table walk for y ------\n");
	analyze_physic_address(y, &y_pt);
	dump_pt_walk_result(&y_pt);
	eprintf("\n");

	printf("Before\n");
	printf("x : %s\n", x);
	printf("y : %s\n", y);

	getchar();
	// ------------------------------------------------
	// Modify page table entry of y
	// Let y point to x's physical address
	// ------------------------------------------------
	write_physical_address(y_pt.pt_addr, x_pt.pt_data);

	printf("After modifying page table\n");
	printf("x : %s\n", x);
	printf("y : %s\n", y);

	getchar();

	strcpy(y, "When you modify y, x is modified actually.");
	printf("After modifying string y\n");
	printf("x : %s\n", x);
	printf("y : %s\n", y);

	// ------------------------------------------------
	// Recover page table entry of y
	// Let y point to its original address
	// You may need to store y's original address at previous step
	// ------------------------------------------------
	write_physical_address(y_pt.pt_addr, y_pt.pt_data);

	getchar();

	printf("After recovering page table of y\n");
	printf("x : %s\n", x);
	printf("y : %s\n", y);

	close(fd);
}
