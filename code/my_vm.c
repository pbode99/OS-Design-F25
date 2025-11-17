
#include "my_vm.h"
#include <string.h>   // optional for memcpy if you later implement put/get
#include <sys/mman.h>
#include <errno.h>

// -----------------------------------------------------------------------------
// Global Declarations (optional)
// -----------------------------------------------------------------------------

struct tlb tlb_store; // Placeholder for your TLB structure

// Optional counters for TLB statistics
static unsigned long long tlb_lookups = 0;
static unsigned long long tlb_misses  = 0;

/* Physical memory buffer and bitmaps */
static void *phys_mem = NULL;           /* base of simulated physical memory */
static unsigned char *phys_bitmap = NULL; /* 1 bit per physical page */
static unsigned char *virt_bitmap = NULL; /* 1 bit per virtual page (optional) */
static uint32_t phys_num_pages = 0;
static uint32_t virt_num_pages = 0;

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------
/*
 * set_physical_mem()
 * ------------------
 * Allocates and initializes simulated physical memory and any required
 * data structures (e.g., bitmaps for tracking page use).
 *
 * Return value: None.
 * Errors should be handled internally (e.g., failed allocation).
 */
void set_physical_mem(void) {
    /* Allocate a contiguous region to simulate physical RAM. Prefer mmap.
     * MEMSIZE and PGSIZE are defined in the header. We compute the
     * number of physical pages and allocate a bitmap with one bit per page.
     */
    if (phys_mem != NULL) {
        /* Already initialized */
        return;
    }

    /* compute page counts (use 64-bit math to be safe) */
    unsigned long long phys_bytes = (unsigned long long) MEMSIZE;
    unsigned long long page_size = (unsigned long long) PGSIZE;
    unsigned long long pcount = phys_bytes / page_size;
    if (pcount == 0) {
        fprintf(stderr, "set_physical_mem: invalid MEMSIZE/PGSIZE\n");
        return;
    }

    phys_num_pages = (uint32_t) pcount;

    /* Virtual pages (MAX_MEMSIZE / PGSIZE). MAX_MEMSIZE is 2^32 so fits in 64-bit */
    unsigned long long vcount = (unsigned long long) (MAX_MEMSIZE / page_size);
    virt_num_pages = (uint32_t) vcount;

    /* Try mmap first */
    void *mem = mmap(NULL, (size_t)phys_bytes, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        /* fallback to malloc */
        mem = malloc((size_t) phys_bytes);
        if (!mem) {
            fprintf(stderr, "set_physical_mem: allocation failed: %s\n", strerror(errno));
            return;
        }
    }

    /* zero the physical memory */
    memset(mem, 0, (size_t) phys_bytes);
    phys_mem = mem;

    /* allocate bitmaps */
    size_t phys_bmap_bytes = (size_t) ((phys_num_pages + 7) / 8);
    phys_bitmap = (unsigned char *) malloc(phys_bmap_bytes);
    if (!phys_bitmap) {
        fprintf(stderr, "set_physical_mem: phys_bitmap allocation failed\n");
        /* clean up mem */
        if (mem && mem != MAP_FAILED) munmap(mem, (size_t) phys_bytes);
        else free(mem);
        phys_mem = NULL;
        return;
    }
    memset(phys_bitmap, 0, phys_bmap_bytes);

    /* virtual bitmap (optional but useful) */
    size_t virt_bmap_bytes = (size_t) ((virt_num_pages + 7) / 8);
    virt_bitmap = (unsigned char *) malloc(virt_bmap_bytes);
    if (!virt_bitmap) {
        fprintf(stderr, "set_physical_mem: virt_bitmap allocation failed\n");
        free(phys_bitmap);
        if (mem && mem != MAP_FAILED) munmap(mem, (size_t) phys_bytes);
        else free(mem);
        phys_mem = NULL;
        phys_bitmap = NULL;
        return;
    }
    memset(virt_bitmap, 0, virt_bmap_bytes);

    /* Report basic initialization information for debugging */
    fprintf(stderr, "set_physical_mem: allocated %u phys pages (%llu bytes), %u virt pages\n",
            phys_num_pages, phys_bytes, virt_num_pages);
}

// -----------------------------------------------------------------------------
// TLB
// -----------------------------------------------------------------------------

/*
 * TLB_add()
 * ---------
 * Adds a new virtual-to-physical translation to the TLB.
 * Ensure thread safety when updating shared TLB data.
 *
 * Return:
 *   0  -> Success (translation successfully added)
 *  -1  -> Failure (e.g., TLB full or invalid input)
 */
int TLB_add(void *va, void *pa)
{
    // TODO: Implement TLB insertion logic.
    return -1; // Currently returns failure placeholder.
}

/*
 * TLB_check()
 * -----------
 * Looks up a virtual address in the TLB.
 *
 * Return:
 *   Pointer to the corresponding page table entry (PTE) if found.
 *   NULL if the translation is not found (TLB miss).
 */
pte_t *TLB_check(void *va)
{
    // TODO: Implement TLB lookup.
    return NULL; // Currently returns TLB miss.
}

/*
 * print_TLB_missrate()
 * --------------------
 * Calculates and prints the TLB miss rate.
 *
 * Return value: None.
 */
void print_TLB_missrate(void)
{
    double miss_rate = 0.0;
    // TODO: Calculate miss rate as (tlb_misses / tlb_lookups).
    fprintf(stderr, "TLB miss rate %lf \n", miss_rate);
}

// -----------------------------------------------------------------------------
// Page Table
// -----------------------------------------------------------------------------

/*
 * translate()
 * -----------
 * Translates a virtual address to a physical address.
 * Perform a TLB lookup first; if not found, walk the page directory
 * and page tables using a two-level lookup.
 *
 * Return:
 *   Pointer to the PTE structure if translation succeeds.
 *   NULL if translation fails (e.g., page not mapped).
 */
pte_t *translate(pde_t *pgdir, void *va)
{
    // TODO: Extract the 32-bit virtual address and compute indices
    // for the page directory, page table, and offset.
    // Return the corresponding PTE if found.
    return NULL; // Translation unsuccessful placeholder.
}

/*
 * map_page()
 * -----------
 * Establishes a mapping between a virtual and a physical page.
 * Creates intermediate page tables if necessary.
 *
 * Return:
 *   0  -> Success (mapping created)
 *  -1  -> Failure (e.g., no space or invalid address)
 */
int map_page(pde_t *pgdir, void *va, void *pa)
{
    // TODO: Map virtual address to physical address in the page tables.
    return -1; // Failure placeholder.
}

// -----------------------------------------------------------------------------
// Allocation
// -----------------------------------------------------------------------------

/*
 * get_next_avail()
 * ----------------
 * Finds and returns the base virtual address of the next available
 * block of contiguous free pages.
 *
 * Return:
 *   Pointer to the base virtual address if available.
 *   NULL if there are no sufficient free pages.
 */
void *get_next_avail(int num_pages)
{
    // TODO: Implement virtual bitmap search for free pages.
    return NULL; // No available block placeholder.
}

/*
 * n_malloc()
 * -----------
 * Allocates a given number of bytes in virtual memory.
 * Initializes physical memory and page directories if not already done.
 *
 * Return:
 *   Pointer to the starting virtual address of allocated memory (success).
 *   NULL if allocation fails.
 */
void *n_malloc(unsigned int num_bytes)
{
    // TODO: Determine required pages, allocate them, and map them.
    return NULL; // Allocation failure placeholder.
}

/*
 * n_free()
 * ---------
 * Frees one or more pages of memory starting at the given virtual address.
 * Marks the corresponding virtual and physical pages as free.
 * Removes the translation from the TLB.
 *
 * Return value: None.
 */
void n_free(void *va, int size)
{
    // TODO: Clear page table entries, update bitmaps, and invalidate TLB.
    


}

// -----------------------------------------------------------------------------
// Data Movement
// -----------------------------------------------------------------------------

/*
 * put_data()
 * ----------
 * Copies data from a user buffer into simulated physical memory using
 * the virtual address. Handle page boundaries properly.
 *
 * Return:
 *   0  -> Success (data written successfully)
 *  -1  -> Failure (e.g., translation failure)
 */
int put_data(void *va, void *val, int size)
{
    // TODO: Walk virtual pages, translate to physical addresses,
    // and copy data into simulated memory.
    



    return -1; // Failure placeholder.
}

/*
 * get_data()
 * -----------
 * Copies data from simulated physical memory (accessed via virtual address)
 * into a user buffer.
 *
 * Return value: None.
 */
void get_data(void *va, void *val, int size)
{
    // TODO: Perform reverse operation of put_data().
    //
}

// -----------------------------------------------------------------------------
// Matrix Multiplication
// -----------------------------------------------------------------------------

/*
 * mat_mult()
 * ----------
 * Performs matrix multiplication of two matrices stored in virtual memory.
 * Each element is accessed and stored using get_data() and put_data().
 *
 * Return value: None.
 */
void mat_mult(void *mat1, void *mat2, int size, void *answer)
{
    int i, j, k;
    uint32_t a, b, c;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            c = 0;
            for (k = 0; k < size; k++) {
                // TODO: Compute addresses for mat1[i][k] and mat2[k][j].
                // Retrieve values using get_data() and perform multiplication.
                get_data(NULL, &a, sizeof(int));  // placeholder
                get_data(NULL, &b, sizeof(int));  // placeholder
                c += (a * b);
            }
            // TODO: Store the result in answer[i][j] using put_data().
            put_data(NULL, (void *)&c, sizeof(int)); // placeholder
        }
    }
}

