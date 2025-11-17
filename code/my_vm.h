#ifndef MY_VM_H_INCLUDED
#define MY_VM_H_INCLUDED

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

/* ============================================================================
 *  Virtual Memory Simulation Header
 * ============================================================================
 *  This header defines constants, data types, and function prototypes
 *  for implementing a simulated 32-bit virtual memory system.
 *
 *  Students will:
 *   - Fill in missing constants and macros for address translation.
 *   - Define TLB structure and page table entry fields.
 *   - Implement all declared functions in my_vm.c.
 *
 *  Return conventions (used across functions):
 *    0   → Success
 *   -1   → Failure
 *   NULL → Translation or lookup not found
 * ============================================================================
 */

// -----------------------------------------------------------------------------
//  Memory and Paging Configuration
// -----------------------------------------------------------------------------

#define VA_BITS        32u           // Simulated virtual address width
#define PGSIZE         4096u         // Page size = 4 KB

#define MAX_MEMSIZE    (1ULL << 32)  // Max virtual memory = 4 GB
#define MEMSIZE        (1ULL << 30)  // Simulated physical memory = 1 GB


//COMPLETE HERE

// --- Constants for bit shifts and masks ---
#define OFFBITS       (__builtin_ctz((unsigned)PGSIZE))
#define PTX_BITS      (((unsigned)VA_BITS - OFFBITS) / 2u)
#define PDX_BITS      (((unsigned)VA_BITS - OFFBITS) - PTX_BITS)

#define PTXSHIFT      (OFFBITS)                       /* bits to shift to get page-table index */
#define PDXSHIFT      (PTXSHIFT + PTX_BITS)          /* bits to shift to get page-directory index */

#define PXMASK        ((1u << PTX_BITS) - 1u)        /* mask for page-table index */
#define OFFMASK       (PGSIZE - 1u)                  /* mask for offset within page */

// --- Macros to extract address components ---
#define PDX(va)       ((((vaddr32_t)(VA2U(va))) >> PDXSHIFT) & ((1u << PDX_BITS) - 1u))
#define PTX(va)       ((((vaddr32_t)(VA2U(va))) >> PTXSHIFT) & PXMASK)
#define OFF(va)       (((vaddr32_t)(VA2U(va))) & OFFMASK)

// -----------------------------------------------------------------------------
//  Type Definitions
// -----------------------------------------------------------------------------

typedef uint32_t vaddr32_t;   // Simulated 32-bit virtual address
typedef uint32_t paddr32_t;   // Simulated 32-bit physical address
typedef uint32_t pte_t;       // Page table entry
typedef uint32_t pde_t;       // Page directory entry

// -----------------------------------------------------------------------------
//  Page Table Flags (Students fill as needed)
// -----------------------------------------------------------------------------

#define PFN_SHIFT     /** TODO: number of bits to shift**/
/* PFN_SHIFT: position of the physical frame number inside a PTE.
 * Since PTE stores PFN at the top and offset is the low OFFBITS, we
 * place PFN shifted by OFFBITS.
 */
#define PFN_SHIFT     (OFFBITS)

// -----------------------------------------------------------------------------
//  Address Conversion Helpers (Provided)
// -----------------------------------------------------------------------------

static inline vaddr32_t VA2U(void *va)     { return (vaddr32_t)(uintptr_t)va; }
static inline void*     U2VA(vaddr32_t u)  { return (void*)(uintptr_t)u; }

// -----------------------------------------------------------------------------
//  TLB Configuration
// -----------------------------------------------------------------------------

#define TLB_ENTRIES   512   // Default number of TLB entries

struct tlb {
    /* Direct-mapped TLB storage. Each entry maps a VPN -> PFN. */
    struct {
        uint32_t vpn;   /* virtual page number */
        uint32_t pfn;   /* physical frame number */
        uint8_t  valid; /* valid bit (0 = invalid, 1 = valid) */
    } entries[TLB_ENTRIES];
    /* simple counters for statistics (optional) */
    unsigned long long lookups;
    unsigned long long misses;
};

extern struct tlb tlb_store;

// -----------------------------------------------------------------------------
//  Function Prototypes
// -----------------------------------------------------------------------------

/*
 * Initializes physical memory and supporting data structures.
 * Return: None.
 */
void set_physical_mem(void);

/*
 * Adds a new virtual-to-physical translation to the TLB.
 * Return: 0 on success, -1 on failure.
 */
int TLB_add(void *va, void *pa);

/*
 * Checks if a virtual address translation exists in the TLB.
 * Return: pointer to PTE on hit; NULL on miss.
 */
pte_t *TLB_check(void *va);

/*
 * Calculates and prints the TLB miss rate.
 * Return: None.
 */
void print_TLB_missrate(void);

/*
 * Translates a virtual address to a physical address.
 * Return: pointer to PTE if successful; NULL otherwise.
 */
pte_t *translate(pde_t *pgdir, void *va);

/*
 * Creates a mapping between a virtual and a physical page.
 * Return: 0 on success, -1 on failure.
 */
int map_page(pde_t *pgdir, void *va, void *pa);

/*
 * Finds the next available block of contiguous virtual pages.
 * Return: pointer to base virtual address on success; NULL if unavailable.
 */
void *get_next_avail(int num_pages);

/*
 * Allocates memory in the simulated virtual address space.
 * Return: pointer to base virtual address on success; NULL on failure.
 */
void *n_malloc(unsigned int num_bytes);

/*
 * Frees one or more pages of memory starting from the given virtual address.
 * Return: None.
 */
void n_free(void *va, int size);

/*
 * Copies data from a user buffer into simulated physical memory
 * through a virtual address.
 * Return: 0 on success, -1 on failure.
 */
int put_data(void *va, void *val, int size);

/*
 * Copies data from simulated physical memory into a user buffer.
 * Return: None.
 */
void get_data(void *va, void *val, int size);

/*
 * Performs matrix multiplication using data stored in simulated memory.
 * Each element should be accessed via get_data() and stored via put_data().
 * Return: None.
 */
void mat_mult(void *mat1, void *mat2, int size, void *answer);

#endif // MY_VM_H_INCLUDED

