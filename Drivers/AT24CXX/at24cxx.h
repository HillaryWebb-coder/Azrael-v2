#ifndef _AT24CXX_H
#define _AT24CXX_H

#define DEV_ADDR 0x50

void at_page_write(uint16_t addr, uint8_t * data, uint16_t size);
void at_read_data(uint8_t *data_buf, uint16_t size);

#endif /* _AT24CXX_H */
