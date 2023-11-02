var serviceId        = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
var characteristicId = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

var myCharacteristic = null;
var dec = new TextDecoder();
var enc = new TextEncoder();

document.querySelector("#connect").onclick = function () {
  navigator.bluetooth.requestDevice({
    filters: [{ services: [serviceId] }]
  }).then(device => {
    return device.gatt.connect();
  }).then(server => {
    return server.getPrimaryService(serviceId);
  }).then(service => {
    return service.getCharacteristic(characteristicId);
  }).then(characteristic => {
    myCharacteristic = characteristic;
  });
};

document.querySelector("#send").onclick = function () {
  if (!myCharacteristic) {
    return;
  }

  var message = document.querySelector("#send-message").value;
  myCharacteristic.writeValue(enc.encode(message));
};

document.querySelector("#receive").onclick = function () {
  if (!myCharacteristic) {
    return;
  }

  myCharacteristic.readValue().then(function (v) {
    var li = document.createElement("li");
    li.innerHTML = dec.decode(v);
    document.querySelector("#received-messages").appendChild(li);
  });
};
