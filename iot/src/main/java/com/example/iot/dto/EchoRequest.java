package com.example.iot.dto;

// DTO = Data Transfer Object
/* Oggetto Java che rappresenta il JSON in ingresso.
 * 
 * esempio: dal client mando un file JSON:
 * {
 * 	 "message" : "ciao",
 *   "deviceId": "STEVAL V1"
 * }
 *
 * spring lo andrà a mappare in un oggetto EchoRequest.
 **/
public class EchoRequest {
    private String message;
    private String deviceId;

    public EchoRequest() {
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    public String getDeviceId() {
        return deviceId;
    }

    public void setDeviceId(String deviceId) {
        this.deviceId = deviceId;
    }
}
