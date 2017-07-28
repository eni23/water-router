/*******
 * Serial functions
 * Provide a simple serial terminal for getting and setting wifi and
 * trigger configuration.
 ****/


char serial_rcv_data[SERIAL_INPUT_BUFFER_SIZE];
uint8_t serial_rcv_data_pos = 0;


void serial_char_event() {
  char current_char = Serial.read();
  // backspace: delete last char in buffer
  if ( current_char == 0x7F && serial_rcv_data_pos > 0 ) {
    serial_rcv_data[serial_rcv_data_pos-1];
    serial_rcv_data_pos--;
    Serial.print("\b");
    Serial.print(" ");
    Serial.print("\b");
    return;
  }
  // carriage return recived = new line
  if (current_char == 13) {
    Serial.println( current_char );
    serial_rcv_data[serial_rcv_data_pos] = '\0';
    serial_process_input();
    return;
  }
  // put every other char than cr into buffer
  Serial.print( current_char );
  serial_rcv_data[serial_rcv_data_pos] = current_char;
  serial_rcv_data_pos++;
}


/**
 * serial debug message
 **/
void debug( const char* message ) {
  if ( eeprom_config.debug_output_enabled ) {
   Serial.println( message );
  }
}


/**
 * this function parses every line recived by serial port and exectues an action
 * if command found.
 * to keep it simple, everything is done by this single function and input is
 * processed as strings, not char-arrays..
 **/
void serial_process_input() {

  // split incoming line by whitespace and parse first chuck as possible command
  String serdata( serial_rcv_data );
  String command = "";
  command = get_str_token( serdata, ' ', 0 );

  // empty buffer and zero counter
  memset(serial_rcv_data, 0, sizeof( serial_rcv_data ) );
  serial_rcv_data_pos = 0;

  // simple pong for recognition
  if (  command ==  "ping" ) {
    Serial.println( "pong" );
    return;
  }


  if ( command == "port" ){
    String port_str = "";
    port_str = get_str_token( serdata, ' ', 1 );
    String state_str = "";
    state_str = get_str_token( serdata, ' ', 2 );
    int port = port_str.toInt();
    boolean state = false;
    int stateint = state_str.toInt();
    if (stateint>0){
      state=true;
    }
    Serial.print("port=");
    Serial.print(port);
    Serial.print(" state=");
    Serial.println(stateint);
    pcf8574_set_port(port, state);
    pcf8574_update();
    return;
  }

  if ( command == "ml" ){
    String valve_str = "";
    valve_str = get_str_token( serdata, ' ', 1 );
    String amount_str = "";
    amount_str = get_str_token( serdata, ' ', 2 );

    int valve = valve_str.toInt();
    int amount = amount_str.toInt();

    if (valve<=0 || amount<=0){
      debug("failed to parse input");
      return;
    }

    water(valve, amount);
    return;
  }

  if ( command == "stop" ){
    stop_water();
    debug("ok");
    led_timeout(led_color_red, 1000);
    return;
  }


  // update wifi settings
  if ( command == "update-wifi" ) {

    String wifi_ssid = "";
    wifi_ssid = get_str_token( serdata, ' ', 1 );
    String wifi_pass = "";
    wifi_pass = get_str_token( serdata, ' ', 2 );

    if ( eeprom_config.debug_output_enabled ) {
      Serial.println( "updating wifi-configuration in eeprom" );
      Serial.print( "new wifi_ssid: " );
      Serial.println( wifi_ssid );
      Serial.print( "new wifi_pass: " );
      Serial.println( wifi_pass );
    }

    wifi_ssid.toCharArray( eeprom_config.wifi_ssid, 64 );
    wifi_pass.toCharArray( eeprom_config.wifi_pass, 64 );
    eeprom_save_config();

    Serial.println("ok");
    return;
  }

  // configure static ip
  if ( command == "ipconfig" ) {

    String ipaddr_str = get_str_token( serdata, ' ', 1 );
    if (ipaddr_str.length()<7){
      serial_print_ipconfig();
      //debug("Usage: ipconfig [ipaddr] [netmask] [gateway] [dns-server]");
      return;
    }
    uint8_t conf_ip[4];
    get_ip_from_string(ipaddr_str, conf_ip);
    ipcpy(conf_ip, eeprom_config.static_ip_addr);
    Serial.print("ipaddr: ");
    serial_print_ip(conf_ip, true);


    String netmask_str = get_str_token( serdata, ' ', 2 );
    if (netmask_str.length()<7){
      eeprom_save_config();
      return;
    }
    uint8_t conf_nm[4];
    get_ip_from_string(netmask_str, conf_nm);
    ipcpy(conf_nm, eeprom_config.static_ip_netmask);
    Serial.print("netmask: ");
    serial_print_ip(conf_nm, true);

    String gw_str = get_str_token( serdata, ' ', 3 );
    if (gw_str.length()<7){
      eeprom_save_config();
      return;
    }
    uint8_t conf_gw[4];
    get_ip_from_string(gw_str, conf_gw);
    ipcpy(conf_gw, eeprom_config.static_ip_gateway);
    Serial.print("gateway: ");
    serial_print_ip(conf_gw, true);

    String dns_str = get_str_token( serdata, ' ', 4 );
    if (dns_str.length()<7){
      eeprom_save_config();
      return;
    }
    uint8_t conf_dns[4];
    get_ip_from_string(dns_str, conf_dns);
    ipcpy(conf_dns, eeprom_config.static_ip_dns);
    Serial.print("dns-server: ");
    serial_print_ip(conf_dns, true);

    eeprom_save_config();
    return;
  }

  if ( command == "enable-static-ip" ) {
    eeprom_config.static_ip = true;
    eeprom_save_config();
    debug("static ip enabled");
    return;
  }
  if ( command == "disable-static-ip" ) {
    eeprom_config.static_ip = false;
    eeprom_save_config();
    debug("static ip disabled");
    return;
  }

  // reconnect wifi
  if ( command == "reconnect-wifi" ) {
    serial_wifi_reconnect();
    return;
  }

  // turn on printing debug messages
  if ( command == "show-debug" ) {
    eeprom_config.debug_output_enabled = true;
    eeprom_save_config();
    debug("debug output enabled");
    return;
  }

  // turn off printing debug messages
  if ( command == "hide-debug" ) {
    eeprom_config.debug_output_enabled = false;
    eeprom_save_config();
    return;
  }

  // reboot device
  if ( command == "reboot" ) {
    esp.restart();
    return;
  }


  if ( command == "_gpio" ) {
    String port_str = "";
    port_str = get_str_token( serdata, ' ', 1 );

    String state_str = "";
    state_str = get_str_token( serdata, ' ', 2 );

    int port = port_str.toInt();
    int state = state_str.toInt();
    if (state > 0){
      digitalWrite(port, HIGH);
      debug("turn on");
    }
    else {
      digitalWrite(port, HIGH);
      debug("turn off");
    }


  }

  // print detailed systeminfos
  if ( command == "info" ) {

    eeprom_load_config();
    delay(10);

    serial_print_ipconfig();

    // eeprom-dump
    Serial.print( "eeprom.wifi_ssid = " );
    Serial.println( eeprom_config.wifi_ssid );
    Serial.print( "eeprom.wifi_pass = " );
    Serial.println( eeprom_config.wifi_pass );


    Serial.print( "eeprom.static_ip = " );
    if (eeprom_config.static_ip){
      Serial.println( "enabled" );
    }
    else {
      Serial.println( "disabled" );
    }
    Serial.print( "eeprom.static_ip_addr = " );
    serial_print_ip( eeprom_config.static_ip_addr, true );
    Serial.print( "eeprom.static_ip_netmask = " );
    serial_print_ip(  eeprom_config.static_ip_netmask, true );
    Serial.print( "eeprom.static_ip_gateway = " );
    serial_print_ip(  eeprom_config.static_ip_gateway, true );
    Serial.print( "eeprom.static_ip_dns = " );
    serial_print_ip(  eeprom_config.static_ip_dns, true );
    Serial.print( "eeprom.debug_output_enabled = " );
    if ( eeprom_config.debug_output_enabled ) Serial.println( "true" );
    else Serial.println( "false" );

    // version & esp-internals
    //Serial.println(  "firmware.version = " AML_SENSORBOX_GIT_TAGID );
    Serial.println( "firmware.configversion = "  CONFIG_VERSION);

    Serial.print( "esp8266.free_heap = " );
    Serial.println( ESP.getFreeHeap() );
    Serial.print( "esp8266.chip_id = " );
    Serial.println( ESP.getChipId() );
    Serial.print( "esp8266.flash_size = " );
    Serial.println( ESP.getFlashChipSize() );
    Serial.print( "esp8266.flash_speed = " );
    Serial.println( ESP.getFlashChipSpeed() );
    Serial.print( "esp8266.cycle_count = " );
    Serial.println( ESP.getCycleCount() );


    return;
  }

  // if nothing matches, command not found
  debug("unknown command");
}


void serial_print_ipconfig(){
    // wifi status
    String wifi_status_human="";
    switch ( WiFi.status() ) {
      case WL_NO_SHIELD:
        wifi_status_human="no shield";
        break;
      case WL_IDLE_STATUS:
        wifi_status_human="idle";
        break;
      case WL_NO_SSID_AVAIL:
        wifi_status_human="requested ssid not aviable";
        break;
      case WL_SCAN_COMPLETED:
        wifi_status_human="ssid scan completed";
        break;
      case WL_CONNECTED:
        wifi_status_human="connected";
        break;
      case WL_CONNECT_FAILED:
        wifi_status_human="connection failed";
        break;
      case WL_CONNECTION_LOST:
        wifi_status_human="connection lost";
        break;
      case WL_DISCONNECTED:
        wifi_status_human="disconnected";
        break;
    }
    // wifi info
    Serial.print( "wifi.status = " );
    Serial.println( wifi_status_human );
    Serial.print( "wifi.mac = " );
    byte mac[6];
    WiFi.macAddress(mac);
    Serial.print(mac[5],HEX);
    Serial.print(":");
    Serial.print(mac[4],HEX);
    Serial.print(":");
    Serial.print(mac[3],HEX);
    Serial.print(":");
    Serial.print(mac[2],HEX);
    Serial.print(":");
    Serial.print(mac[1],HEX);
    Serial.print(":");
    Serial.println(mac[0],HEX);
    if ( WiFi.status() == WL_CONNECTED ) {
      Serial.print( "wifi.ip = " );
      Serial.println( WiFi.localIP() );
      Serial.print( "wifi.netmask = " );
      Serial.println( WiFi.subnetMask() );
      Serial.print( "wifi.gateway = " );
      Serial.println( WiFi.gatewayIP() );
    }
}


// helper for extracting ip addresses
void get_ip_from_string(String ipstr, uint8_t *result ){
  for (int i=0; i<4; i++){
    String num_str = get_str_token(ipstr, '.', i);
    result[i] = num_str.toInt();
  }
}


// helper for extracting ip addresses
void serial_print_ip( uint8_t *addr, boolean nl ){
  for (int i=0; i<4; i++){
    Serial.print(addr[i]);
    if (i<3){
      Serial.print(".");
    }
  }
  if (nl) {
    Serial.println("");
  }
}


void ipcpy( uint8_t* ipaddr, uint8_t* eepc ){
  for (int i=0; i<4; i++){
    eepc[i] = ipaddr[i];
  }
}


/**
 * reconnect wifi from serial command. this func is blocking cpu
 * for rounds*round_delay ms
 **/
void serial_wifi_reconnect( ) {
  debug("do a wifi-reconnect");
  debug( "disconnecting from current network (100ms)" );
  WiFi.disconnect();
  delay( 100 );
  debug( "connecting (10sec)" );
  WiFi.begin( eeprom_config.wifi_ssid, eeprom_config.wifi_pass );
  wifi_set_static_if_enabled();
  int rounds = 0;
  while ( WiFi.status() != WL_CONNECTED ) {
    delay( 50 );
    Serial.print( "." );
    rounds++;
    // delay=50ms, rounds=200, time=10sec
    if (rounds > 200 ){
      if (  WiFi.status() != WL_CONNECTED ) {
        Serial.println( "." );
        Serial.println( "fail" );
        debug( "failed to connect in given timeframe. connection open in background" );
        return;
      }
      break;
    }
    rounds++;
  }
  Serial.println( "." );
  if (  WiFi.status() == WL_CONNECTED ) {
    Serial.println( "ok" );
  }
}
