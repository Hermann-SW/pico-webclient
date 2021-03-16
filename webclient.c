#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/watchdog.h"
#include "hardware/structs/watchdog.h"

#include "tusb_lwip_glue.h"

#include "lwip/tcp.h"

#define LED_PIN     25

uint32_t last;

err_t error;

struct tcp_pcb *testpcb;

err_t tcpRecvCallback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    gpio_put(LED_PIN, 0);

    printf("Data received: %dus\n",time_us_32()-last);
    last = time_us_32();

    if (p == NULL) {
        printf("The remote host closed the connection.\n");
        printf("Now I'm closing the connection.\n");
        //tcp_close_con();
        tcp_close(testpcb);
        return ERR_ABRT;
    } else {
        printf("Number of pbufs %d\n", pbuf_clen(p));
        printf("Contents of pbuf %.*s\n", p->len, (char *)p->payload);
    }

    return 0;
}

uint32_t tcp_send_packet(void)
{
    char *string =
#if    SCENARIO == 1
    "GET /index.html HTTP/1.0\r\nHost: foo.bar\r\n\r\n";
#elif  SCENARIO == 2
    "GET /cgi-bin/sol.English.pl?60000 HTTP/1.0\r\nHost: stamm-wilbrandt.de\r\n\r\n";
#else
    "GET /index.html HTTP/1.0\r\nHost: neverssl.com\r\n\r\n";
#endif
    uint32_t len = strlen(string);

    gpio_put(LED_PIN, 1);

    printf("tcp_send_packet(%s): %dus\n", string, time_us_32()-last);
    last = time_us_32();

    /* push to buffer */
    err_t    error = tcp_write(testpcb, string, strlen(string), TCP_WRITE_FLAG_COPY);

    if (error) {
        printf("ERROR: Code: %d (tcp_send_packet :: tcp_write)\n", error);
        return 1;
    }

    /* now send */
    error = tcp_output(testpcb);
    if (error) {
        printf("ERROR: Code: %d (tcp_send_packet :: tcp_output)\n", error);
        return 1;
    }
    return 0;
}

err_t tcpSendCallback(void* arg, struct tcp_pcb *pcb, u16_t len)
{
    gpio_put(LED_PIN, 0);
    printf("tcpSendCallback(): %dus\n", time_us_32()-last);
    last = time_us_32();
    return 0;
}

void tcpErrorHandler(void* arg, err_t err)
{
    printf("tcpErrorHandler()=%d: %dus\n", err, time_us_32()-last);

    for(;;)  { gpio_xor_mask(1 << LED_PIN);  sleep_ms(250); }
}

/* connection established callback, err is unused and only return 0 */
err_t connectCallback(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    printf("Connection Established: %dus\n", time_us_32()-last);
    last = time_us_32();
    printf("Now sending a packet\n");
    tcp_send_packet();
    return 0;
}

void tcp_setup(void)
{
    uint32_t data = 0xdeadbeef;

    printf("tcp_setup(): %dus\n",time_us_32()-last);
    last = time_us_32();

    gpio_put(LED_PIN, 1);

    /* create an ip */
    struct ip4_addr ip;
#if  SCENARIO == 3
    IP4_ADDR(&ip, 13, 225, 84, 121);  //IP of neverssl.com
#else
    IP4_ADDR(&ip, 192, 168, 7, 2);    //IP of Pi400 the Pico is connected to
#endif

    /* create the control block */
    testpcb = tcp_new();    //testpcb is a global struct tcp_pcb
                            // as defined by lwIP


    /* dummy data to pass to callbacks*/

    tcp_arg(testpcb, &data);

    /* register callbacks with the pcb */

    tcp_err(testpcb, tcpErrorHandler);
    tcp_recv(testpcb, tcpRecvCallback);
    tcp_sent(testpcb, tcpSendCallback);

    /* now connect */
#if  SCENARIO == 2
    tcp_connect(testpcb, &ip, 4433, connectCallback);
#else
    tcp_connect(testpcb, &ip, 80, connectCallback);
#endif
}

int main()
{
    stdio_uart_init();

    last = time_us_32();

    // Initialize tinyusb, lwip, dhcpd and httpd
    init_lwip();
    printf("init_lwip(): %dus\n",time_us_32()-last);
    last = time_us_32();
    wait_for_netif_is_up();
    printf("wait_for_netif_is_up(): %dus\n",time_us_32()-last);
    last = time_us_32();
    dhcpd_init();
    printf("dhcpd_init(): %dus\n",time_us_32()-last);
    last = time_us_32();
    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_put(LED_PIN, 1);
    tcp_setup();

    while (true)
    {
        tud_task();
        service_traffic();
    }

    return 0;
}
