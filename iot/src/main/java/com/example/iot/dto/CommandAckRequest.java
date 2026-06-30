package com.example.iot.dto;

public class CommandAckRequest {

    private String status;
    private String ackAt;
    private String resultMessage;

    public CommandAckRequest() {
    }

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public String getAckAt() {
        return ackAt;
    }

    public void setAckAt(String ackAt) {
        this.ackAt = ackAt;
    }

    public String getResultMessage() {
        return resultMessage;
    }

    public void setResultMessage(String resultMessage) {
        this.resultMessage = resultMessage;
    }
}