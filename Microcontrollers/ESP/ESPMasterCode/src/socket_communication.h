uint8_t getRequestFromSocket(WiFiClient client, uint8_t state)
{
  Serial.println("Requesting action");
  uint8_t recieved_state[1];
  client.write(state);
  while(!client.available()){
  }
  char c = client.read(recieved_state, 1u);
  client.write(recieved_state[0]);
  Serial.println(recieved_state[0]);
  return recieved_state[0];
}
