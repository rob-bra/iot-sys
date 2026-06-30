package com.example.iot.dto;

public class CommandRequest {

    private String type;
    private String payload;

    public CommandRequest() {
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    public String getPayload() {
        return payload;
    }

    public void setPayload(String payload) {
        this.payload = payload;
    }
}
