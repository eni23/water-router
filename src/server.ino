


void server_setup(){
  server = new ESP8266WebServer(8080);
  server->on("/", server_process_request );
  server->begin();
}



void server_process_request(){

  debug("trigger via http");

  String valve_str = server->arg("valve");
  String amount_str = server->arg("ml");
  int valve = valve_str.toInt();
  int amount = amount_str.toInt();

  if (valve < 1 || amount<=0 || valve > 5 ){
    server->send(500, "text/plain", String("failed to validate input\n\r") );
    return;
  }

  if ( pump_is_running ){
    server->send(500, "text/plain", String("a job is allready running\n\r") );
    return;
  }

  water(valve, amount);
  server->send(200, "text/plain", String("OK\n\r") );
}
