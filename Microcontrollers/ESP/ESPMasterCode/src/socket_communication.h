uint8_t getRequestFromSocket(WiFiClient client, uint8_t state)
{
  WebSerial.println("Requesting action");
  uint8_t recieved_state[1];
  client.write(state);
  WebSerial.println(state);
  while(!client.available()){
  }
  delay(5);
  char c = client.read(recieved_state, 1u);
  client.write(0x01u);
  WebSerial.println(recieved_state[0]);
  delay(5000);
  return recieved_state[0];
}
