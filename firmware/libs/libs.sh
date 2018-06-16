declare -a LIBS=(
    app_trace/libapp_trace.a
    app_update/libapp_update.a
    aws_iot/libaws_iot.a
    bootloader/bootloader_support/libbootloader_support.a
    bootloader/log/liblog.a
    bootloader/main/libmain.a
    bootloader/micro-ecc/libmicro-ecc.a
    bootloader/soc/libsoc.a
    bootloader/spi_flash/libspi_flash.a
    bootloader_support/libbootloader_support.a
    bt/libbt.a
    coap/libcoap.a
    console/libconsole.a
    cxx/libcxx.a
    driver/libdriver.a
    esp32/libesp32.a
    esp_adc_cal/libesp_adc_cal.a
    ethernet/libethernet.a
    expat/libexpat.a
    fatfs/libfatfs.a
    freertos/libfreertos.a
    heap/libheap.a
    idf_test/libidf_test.a
    jsmn/libjsmn.a
    json/libjson.a
    libsodium/liblibsodium.a
    log/liblog.a
    lwip/liblwip.a
    main/libmain.a
    mbedtls/libmbedtls.a
    mdns/libmdns.a
    micro-ecc/libmicro-ecc.a
    newlib/libnewlib.a
    nghttp/libnghttp.a
    nvs_flash/libnvs_flash.a
    openssl/libopenssl.a
    pthread/libpthread.a
    sdmmc/libsdmmc.a
    soc/libsoc.a
    spiffs/libspiffs.a
    spi_flash/libspi_flash.a
    tcpip_adapter/libtcpip_adapter.a
    ulp/libulp.a
    vfs/libvfs.a
    wear_levelling/libwear_levelling.a
    wpa_supplicant/libwpa_supplicant.a
    xtensa-debug-module/libxtensa-debug-module.a
)

for lib in "${LIBS[@]}"
do
    cp $lib "../"
done