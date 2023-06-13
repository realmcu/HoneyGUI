/*
 * File      : mptool.c
 * This file for download
 * demo
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
#include "rtconfig.h"
#include <rthw.h>
#include "dfs_fs.h"
#include <dfs_posix.h>
#include "crc16btx.h"
#include "mptool.h"



static rt_device_t serial;
static struct rt_semaphore rx_sem;


static void hci_write(uint32_t len, uint8_t *buf)
{
    uint8_t head[4] = {0x01, 0x20, 0xFC, 0x00};
    head[3] = len;

    rt_device_write(serial, 0, head, 4);
    rt_device_write(serial, 0, buf, len);
}
static rt_bool_t hci_read(uint32_t len, uint8_t *buf)
{
    struct rt_serial_device *serial_dev = (struct rt_serial_device *)serial;
    struct rt_serial_rx_fifo *rx_fifo;
    rx_fifo = (struct rt_serial_rx_fifo *)serial_dev->serial_rx;

    uint32_t rx_length = 0;

    while (rx_length != (len + 3))
    {
        rt_err_t result = rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        if (result != RT_EOK)
        {
            return RT_FALSE;
        }
        rx_length = (rx_fifo->put_index >= rx_fifo->get_index) ? (rx_fifo->put_index - rx_fifo->get_index) :
                    \
                    (serial_dev->config.bufsz - (rx_fifo->get_index - rx_fifo->put_index));
    }
    rt_device_read(serial, -1, buf, 3);
    if ((buf[0] != 0x04) || (buf[1] != 0x0E) || (buf[2] != len))
    {
        rt_kprintf("HCI EVENT ERROR 0x%x - 0x%x - 0x%x - 0x%x - 0x%x\n", buf[0], buf[1], buf[2], buf[3],
                   buf[4]);
    }
    rt_device_read(serial, -1, buf, len);
    return RT_TRUE;
}

static rt_bool_t flash_reponse(uint16_t op_code)
{
    uint8_t reponse[12];
    struct rt_serial_device *serial_dev = (struct rt_serial_device *)serial;
    struct rt_serial_rx_fifo *rx_fifo;
    rx_fifo = (struct rt_serial_rx_fifo *)serial_dev->serial_rx;

    uint32_t rx_length = 0;

    while (rx_length != 10)
    {
        rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        rx_length = (rx_fifo->put_index >= rx_fifo->get_index) ? (rx_fifo->put_index - rx_fifo->get_index) :
                    \
                    (serial_dev->config.bufsz - (rx_fifo->get_index - rx_fifo->put_index));
    }
    rt_device_read(serial, -1, reponse, 10);
    if (reponse[3] !=  0x00)
    {
        rt_kprintf("flash op fail!! \n");
        return RT_FALSE;
    }
    return RT_TRUE;
}

static void flash_sector_erase(uint32_t addr, uint8_t cnt)
{
    uint8_t request[16];
    request[0] = 0x87;
    uint16_t op = 0x1030;

    UINT32_TO_ARRAY(request + 1, op);
    UINT32_TO_ARRAY(request + 3, addr);
    uint32_t len = cnt * 4096;
    UINT32_TO_ARRAY(request + 7, len);
    uint16_t crc = btxfcs(BTXFCS_INIT, request, 11);
    UINT16_TO_ARRAY(request + 11, crc);
    rt_device_write(serial, 0, request, 13);
    if (flash_reponse(op) == RT_FALSE)
    {
        rt_kprintf("sector erase fail \n");
        while (1);
    }
}
void flash_chip_erase(void)
{
    uint8_t request[16];
    request[0] = 0x87;
    uint16_t op = 0x1031;

    UINT32_TO_ARRAY(request + 1, op);
    uint16_t crc = btxfcs(BTXFCS_INIT, request, 3);
    UINT16_TO_ARRAY(request + 3, crc);
    rt_device_write(serial, 0, request, 5);
    if (flash_reponse(op) == RT_FALSE)
    {
        rt_kprintf("chip erase fail \n");
        while (1);
    }
}
static void flash_program(uint32_t addr, uint32_t len, uint8_t *payload)
{
    uint8_t request[16];
    request[0] = 0x87;
    uint16_t op = 0x1032;

    UINT32_TO_ARRAY(request + 1, op);
    UINT32_TO_ARRAY(request + 3, addr);
    UINT32_TO_ARRAY(request + 7, len);
    rt_device_write(serial, 0, request, 11);
    rt_device_write(serial, 0, payload, len);
    uint16_t crc0 = btxfcs(BTXFCS_INIT, request, 11);
    uint16_t crc = btxfcs(crc0, payload, len);

    UINT16_TO_ARRAY(request, crc);
    rt_device_write(serial, 0, request, 2);

    if (flash_reponse(op) == RT_FALSE)
    {
        rt_kprintf("flash program fail \n");
        while (1);
    }
}
static void flash_verify(uint32_t addr, uint32_t len, uint16_t image_crc)
{
    uint8_t request[16];
    request[0] = 0x87;
    uint16_t op = 0x1050;

    UINT32_TO_ARRAY(request + 1, op);
    UINT32_TO_ARRAY(request + 3, addr);
    UINT32_TO_ARRAY(request + 7, len);
    UINT32_TO_ARRAY(request + 11, image_crc);
    uint16_t crc = btxfcs(BTXFCS_INIT, request, 13);
    UINT16_TO_ARRAY(request + 13, crc);
    rt_device_write(serial, 0, request, 15);
    if (flash_reponse(op) == RT_FALSE)
    {
        rt_kprintf("flash verify fail \n");
        while (1);
    }
}

#if 0
int read_version(void)
{
    uint8_t receive[16];
    uint8_t head[4] = {0x01, 0x01, 0x10, 0x00};
    rt_device_write(serial, 0, head, 4);
    hci_read(12, receive);
    return 0;
}
int read_rom_version(void)
{
    uint8_t receive[16];
    uint8_t head[4] = {0x01, 0x6D, 0xFC, 0x00};
    rt_device_write(serial, 0, head, 4);
    hci_read(5, receive);
    return 0;
}
#endif


int fw_download(uint32_t index, uint8_t *data, uint32_t len)
{
    uint8_t send[256];
    uint8_t receive[16];
    send[0] = index;
    memcpy(send + 1, data, len);
    hci_write(len + 1, send);

    if (hci_read(5, receive) == RT_FALSE)
    {
        //fw_download(index, data, len);
    }
    if ((receive[0] != 0x02) || (receive[3] != 0x00) || (receive[4] != index))
    {
        rt_kprintf("FW DOWNLOAD ERROR index = %d\n", index);
        rt_kprintf("FW DOWNLOAD ERROR 0x%x - 0x%x - 0x%x - 0x%x - 0x%x\n", receive[0], receive[1],
                   receive[2], receive[3], receive[4]);
        while (1);
    }
    else
    {
        //rt_kprintf("FW DOWNLOAD SUCCESS! index = %d, len = %d \n", index, len);
    }
    return 0;
}

/*
    RTL8762C B-CUT
            {0x201234, 0x203831, 0},
    RTL8762D A-CUT
            {0x2011c0, 0x203431, 0},
*/
void fw_bring_up(void)
{
    uint32_t parameter1 = 0x2011c0;
    uint32_t parameter2 = 0x203431;
    uint8_t head[4] = {0x01, 0x62, 0xFC, 0x00};
    head[3] = 9;
    rt_device_write(serial, 0, head, 4);
    uint8_t buf[9];
    buf[0] = 0x20;
    UINT32_TO_ARRAY(buf + 1, parameter1);
    UINT32_TO_ARRAY(buf + 5, parameter2);
    rt_device_write(serial, 0, buf, 9);

    rt_thread_mdelay(1000);
    rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
    char ch;
    //uint8_t cnt = 0;
    /* clear rx fifo*/
    while (rt_device_read(serial, -1, &ch, 1) == 1)
    {
        //cnt++;
        //rt_kprintf("0x%02x  ", ch);
    }
    //rt_kprintf("cnt = %d\n", cnt);
}

static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&rx_sem);
    //rt_kprintf("R = %d \n", size);
    return RT_EOK;
}


static void mptool_app_image_download_task_entry(void *parameter)
{
    rt_pin_mode(MP_RST_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(MP_LOG_PIN, PIN_MODE_OUTPUT);

    rt_pin_write(MP_RST_PIN, 1);
    rt_pin_write(MP_LOG_PIN, 1);

    rt_pin_write(MP_LOG_PIN, 0);
    Pad_Config(MP_LOG_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    rt_thread_mdelay(1000);
    rt_pin_write(MP_RST_PIN, 0);
    rt_thread_mdelay(500);
    rt_pin_write(MP_RST_PIN, 1);
    rt_thread_mdelay(1000);
    //rt_pin_write(MP_LOG_PIN, 1);

    serial = rt_device_find("uart1");
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    rt_device_open(serial, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(serial, uart_input);


    int fd;
    long file_len;
    char *image = "/firmware/app_MP_sdk#####_1.0.1.3_6a1a81b5-472c65f020304888c776ce6a97bc714a.bin";
    char *fw = "/firmware/RTL8762D_FW_A.BIN";
    uint8_t *packet = RT_NULL;
    fd = open(fw, O_RDONLY | O_BINARY, 0);
    file_len = lseek(fd, 0, SEEK_END);
    rt_kprintf("fw len %ld\n", file_len);
    /* move to the begining of file */
    lseek(fd, 0, SEEK_SET);


    packet = malloc(FW_PACKET_SIZE);
    for (uint32_t i = 0; i < file_len / FW_PACKET_SIZE; i++)
    {
        read(fd, packet, FW_PACKET_SIZE);
        rt_kprintf("fw packet cnt = %d\n", i);
        fw_download(i, packet, FW_PACKET_SIZE);
    }
    read(fd, packet, file_len % FW_PACKET_SIZE);
    fw_download(file_len / FW_PACKET_SIZE, packet, file_len % FW_PACKET_SIZE);
    free(packet);
    packet = RT_NULL;
    close(fd);
    fw_bring_up();



    fd = open(image, O_RDONLY | O_BINARY, 0);
    file_len = lseek(fd, 0, SEEK_END);
    rt_kprintf("image len %ld\n", file_len);
    /* move to the begining of file */
    lseek(fd, 512, SEEK_SET);
    file_len = file_len - 512;

    packet = malloc(IMAGE_PACKET_SIZE);
    for (uint32_t i = 0; i < file_len / IMAGE_PACKET_SIZE; i++)
    {
        read(fd, packet, IMAGE_PACKET_SIZE);
        flash_sector_erase(IMAGE_START_ADDR + i * IMAGE_PACKET_SIZE, 1 * (IMAGE_PACKET_SIZE / 4096));
        flash_program(IMAGE_START_ADDR + i * IMAGE_PACKET_SIZE, IMAGE_PACKET_SIZE, packet);
        uint16_t image_crc = btxfcs(BTXFCS_INIT, packet, IMAGE_PACKET_SIZE);;
        flash_verify(IMAGE_START_ADDR + i * IMAGE_PACKET_SIZE, IMAGE_PACKET_SIZE, image_crc);
        rt_kprintf("mp packet cnt = %d\n", i);
    }
    read(fd, packet, file_len % IMAGE_PACKET_SIZE);
    flash_sector_erase(IMAGE_START_ADDR + (file_len / IMAGE_PACKET_SIZE) * IMAGE_PACKET_SIZE,
                       1 * (IMAGE_PACKET_SIZE / 4096));

    flash_program(IMAGE_START_ADDR + (file_len / IMAGE_PACKET_SIZE) * IMAGE_PACKET_SIZE,
                  file_len % IMAGE_PACKET_SIZE, packet);
    uint16_t image_crc = btxfcs(BTXFCS_INIT, packet, file_len % IMAGE_PACKET_SIZE);
    flash_verify(IMAGE_START_ADDR + (file_len / IMAGE_PACKET_SIZE) * IMAGE_PACKET_SIZE,
                 file_len % IMAGE_PACKET_SIZE, image_crc);
    free(packet);
    packet = RT_NULL;
    close(fd);

    rt_kprintf("=====DONE======\n");

    while (1)
    {
        rt_thread_mdelay(5000);
    }
}

static void mptool_pack_image_download_task_entry(void *parameter)
{
    rt_pin_mode(MP_RST_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(MP_LOG_PIN, PIN_MODE_OUTPUT);

    rt_pin_write(MP_RST_PIN, 1);
    rt_pin_write(MP_LOG_PIN, 1);

    rt_pin_write(MP_LOG_PIN, 0);
    Pad_Config(MP_LOG_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    rt_thread_mdelay(1000);
    rt_pin_write(MP_RST_PIN, 0);
    rt_thread_mdelay(500);
    rt_pin_write(MP_RST_PIN, 1);
    rt_thread_mdelay(1000);
    //rt_pin_write(MP_LOG_PIN, 1);

    serial = rt_device_find("uart1");
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    rt_device_open(serial, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(serial, uart_input);


    int fd;
    long file_len;
    long sub_file_len;
    rt_uint32_t sub_file_start_addr;
    char *image = "/firmware/ImgPacketFile-2cd2e2b848e812dd11ea07fae7af2f5b.bin";
    char *fw = "/firmware/RTL8762D_FW_A.BIN";
    uint8_t *packet = RT_NULL;
    fd = open(fw, O_RDONLY | O_BINARY, 0);
    file_len = lseek(fd, 0, SEEK_END);
    rt_kprintf("fw len %ld\n", file_len);
    /* move to the begining of file */
    lseek(fd, 0, SEEK_SET);

    packet = malloc(FW_PACKET_SIZE);
    for (uint32_t i = 0; i < file_len / FW_PACKET_SIZE; i++)
    {
        read(fd, packet, FW_PACKET_SIZE);
        rt_kprintf("fw packet cnt = %d\n", i);
        fw_download(i, packet, FW_PACKET_SIZE);
    }
    read(fd, packet, file_len % FW_PACKET_SIZE);
    fw_download(file_len / FW_PACKET_SIZE, packet, file_len % FW_PACKET_SIZE);
    free(packet);
    packet = RT_NULL;
    close(fd);
    fw_bring_up();

    fd = open(image, O_RDONLY | O_BINARY, 0);
    file_len = lseek(fd, 0, SEEK_END);
    rt_kprintf("pack image len %ld\n", file_len);
    /* move to the begining of file */

    rt_uint8_t header_signature[2];
    lseek(fd, 0, SEEK_SET);
    read(fd, header_signature, 2);

    if ((0x47 != header_signature[0]) || (0x4D != header_signature[1]))
    {
        rt_kprintf("downloading file type error\n");
        return;
    }

    union sub_file_ind header_sub_file_indicator;
    lseek(fd, 40, SEEK_SET);
    read(fd, header_sub_file_indicator.byte, 4);

    rt_uint8_t sub_file_sum = 0;
    for (rt_uint8_t i = 0; i < 32; i++)
    {
        if ((header_sub_file_indicator.value >> i) & 0x00000001)
        {
            sub_file_sum++;
        }
    }
    rt_kprintf("sub_file_sum %d\n", sub_file_sum);

    uint16_t sub_file_header_len = sub_file_sum * 12;
    rt_uint8_t *header_sub_file = malloc(sub_file_header_len);
    if (header_sub_file == NULL)
    {
        return;
    }

    lseek(fd, 44, SEEK_SET);
    read(fd, header_sub_file, sub_file_header_len);

    rt_uint32_t base_addr = 44 + sub_file_header_len;
    packet = malloc(IMAGE_PACKET_SIZE);
    for (rt_uint8_t file_num = 0; file_num < sub_file_sum; file_num++)
    {
        sub_file_len = LE_EXTRN2DWORD(&header_sub_file[file_num * 12 + 4]) - SUB_FILE_DATA_OFFSET;
        sub_file_start_addr = LE_EXTRN2DWORD(&header_sub_file[file_num * 12]);
        rt_kprintf("file cnt %d sub_file_len = %d, sub_file_start_addr 0x%x\n", file_num, sub_file_len,
                   sub_file_start_addr);

        lseek(fd, base_addr + SUB_FILE_DATA_OFFSET, SEEK_SET);
        for (uint32_t i = 0; i < sub_file_len / IMAGE_PACKET_SIZE; i++)
        {
            read(fd, packet, IMAGE_PACKET_SIZE);
            flash_sector_erase(sub_file_start_addr + i * IMAGE_PACKET_SIZE, 1 * (IMAGE_PACKET_SIZE / 4096));
            flash_program(sub_file_start_addr + i * IMAGE_PACKET_SIZE, IMAGE_PACKET_SIZE, packet);
            uint16_t image_crc = btxfcs(BTXFCS_INIT, packet, IMAGE_PACKET_SIZE);;
            flash_verify(sub_file_start_addr + i * IMAGE_PACKET_SIZE, IMAGE_PACKET_SIZE, image_crc);
            rt_kprintf("mp packet cnt = %d\n", i);
        }
        read(fd, packet, sub_file_len % IMAGE_PACKET_SIZE);
        flash_sector_erase(sub_file_start_addr + (sub_file_len / IMAGE_PACKET_SIZE) * IMAGE_PACKET_SIZE,
                           1 * (IMAGE_PACKET_SIZE / 4096));

        flash_program(sub_file_start_addr + (sub_file_len / IMAGE_PACKET_SIZE) * IMAGE_PACKET_SIZE,
                      sub_file_len % IMAGE_PACKET_SIZE, packet);
        uint16_t image_crc = btxfcs(BTXFCS_INIT, packet, sub_file_len % IMAGE_PACKET_SIZE);
        flash_verify(sub_file_start_addr + (sub_file_len / IMAGE_PACKET_SIZE) * IMAGE_PACKET_SIZE,
                     sub_file_len % IMAGE_PACKET_SIZE, image_crc);

        base_addr += sub_file_len + SUB_FILE_DATA_OFFSET;
    }

    free(packet);
    packet = RT_NULL;
    free(header_sub_file);
    header_sub_file = RT_NULL;
    close(fd);

    rt_kprintf("=====DONE======\n");

    while (1)
    {
        rt_thread_mdelay(5000);
    }
}

static int mptool_demo(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("mptool",
                           mptool_pack_image_download_task_entry, RT_NULL,
                           512 * 4, 25, 10);

    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }


    return 0;
}

INIT_APP_EXPORT(mptool_demo);





