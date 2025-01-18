#include <string.h>
#include "uart_roomsoc.h"
#include "fdt.h"

volatile unsigned int *uart_roomsoc;

#define UART_REG_ENABLE     0
#define UART_REG_DATA       1
#define UART_REG_STATUS     2

#define UART_SR_TXEMPTY     (1 << 0)
#define UART_SR_TXBUSY      (1 << 2)

void uart_roomsoc_putchar(uint8_t c)
{
    uart_roomsoc[UART_REG_DATA] = c;

    for (;;) {
        uint32_t status = uart_roomsoc[UART_REG_STATUS];
        if (!(status & UART_SR_TXBUSY)) break;
    }
}

int uart_roomsoc_getchar()
{
    return -1;
}

struct uart_roomsoc_scan
{
    int compat;
    uint64_t reg;
};

static void uart_roomsoc_open(const struct fdt_scan_node *node, void *extra)
{
    struct uart_roomsoc_scan *scan = (struct uart_roomsoc_scan *)extra;
    memset(scan, 0, sizeof(*scan));
}

static void uart_roomsoc_prop(const struct fdt_scan_prop *prop, void *extra)
{
    struct uart_roomsoc_scan *scan = (struct uart_roomsoc_scan *)extra;
    if (!strcmp(prop->name, "compatible") &&
        !strcmp((const char *)prop->value, "roomsoc,uart0")) {
        scan->compat = 1;
    } else if (!strcmp(prop->name, "reg")) {
        fdt_get_address(prop->node->parent, prop->value, &scan->reg);
    }
}

static void uart_roomsoc_done(const struct fdt_scan_node *node, void *extra)
{
    struct uart_roomsoc_scan *scan = (struct uart_roomsoc_scan *)extra;
    if (!scan->compat || !scan->reg || uart_roomsoc)
        return;

    // Initialize Roomsoc UART
    uart_roomsoc = (void *)(uintptr_t)scan->reg;
    uart_roomsoc[UART_REG_ENABLE] = 0x1;
}

void query_uart_roomsoc(uintptr_t fdt)
{
    struct fdt_cb cb;
    struct uart_roomsoc_scan scan;

    memset(&cb, 0, sizeof(cb));
    cb.open = uart_roomsoc_open;
    cb.prop = uart_roomsoc_prop;
    cb.done = uart_roomsoc_done;
    cb.extra = &scan;

    fdt_scan(fdt, &cb);
}
