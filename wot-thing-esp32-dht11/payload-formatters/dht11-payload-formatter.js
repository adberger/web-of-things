// decodes payload (temperature and humidity) from dht11 sensor
function decodeUplink(input) {
  var data = {};
  
  data.temperature = (input.bytes[0] << 8) + input.bytes[1];
  data.humidity = (input.bytes[2] << 8) + input.bytes[3];
  
  return {
    data: data,
    warnings: [],
    errors: []
  };
}