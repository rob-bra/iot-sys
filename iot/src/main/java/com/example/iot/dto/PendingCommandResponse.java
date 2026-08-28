package com.example.iot.dto;

/**
 * Data Transfer Object che rappresenta un comando remoto pendente restituito
 * dal backend al dispositivo embedded durante il polling dei comandi.
 * <p>
 * Questo DTO viene utilizzato come forma di risposta dell'endpoint REST
 * dedicato al recupero dei comandi con stato {@code PENDING} associati a uno
 * specifico dispositivo.
 * </p>
 * <p>
 * Rispetto alla rappresentazione interna del database, questo oggetto espone
 * una struttura JSON più pulita e orientata all'API, utilizzando convenzioni
 * in camelCase invece dei nomi delle colonne SQL con underscore.
 * </p>
 */
public class PendingCommandResponse {

    private Long id;
    private String deviceId;
    private String type;
    private String payload;
    private String status;
    private String createdAt;
    private String ackAt;
    private String resultMessage;

    /**
     * Costruttore di default richiesto dal framework per la serializzazione e
     * deserializzazione JSON.
     */
    public PendingCommandResponse() {
    }

    /**
     * Costruisce un DTO completo rappresentante un comando remoto.
     * 
     * Constructs a new PendingCommandResponse with the specified details.
     *
     * @param id The unique identifier of the pending command.
     * @param deviceId The identifier of the device associated with the command.
     * @param type The type of the command.
     * @param payload The payload of the command.
     * @param status The current status of the command.
     * @param createdAt The timestamp when the command was created.
     * @param ackAt The timestamp when the command was acknowledged.
     * @param resultMessage Any result message associated with the command.
     * */
    public PendingCommandResponse(Long id, String deviceId, String type, String payload,
                                  String status, String createdAt, String ackAt, String resultMessage) {
        this.id = id;
        this.deviceId = deviceId;
        this.type = type;
        this.payload = payload;
        this.status = status;
        this.createdAt = createdAt;
        this.ackAt = ackAt;
        this.resultMessage = resultMessage;
    }

    /**
     * Returns the unique identifier of the pending command.
     *
     * @return The ID of the pending command.
     * */
    public Long getId() {
        return id;
    }

    /**
     * Returns the identifier of the device associated with the command.
     *
     * @return The device ID.
     * */
    public String getDeviceId() {
        return deviceId;
    }

    /**
     * Returns the type of the command.
     *
     * @return The command type.
     * */
    public String getType() {
        return type;
    }

    /**
     * Returns the payload of the command.
     *
     * @return The command payload.
     * */
    public String getPayload() {
        return payload;
    }

    /**
     * Returns the current status of the command.
     *
     * @return The command status.
     * */
    public String getStatus() {
        return status;
    }

    public void setId(Long id) {
		this.id = id;
	}

	public void setType(String type) {
		this.type = type;
	}

	public void setPayload(String payload) {
		this.payload = payload;
	}

	/**
     * Returns the timestamp when the command was created.
     *
     * @return The creation timestamp.
     * */
    public String getCreatedAt() {
        return createdAt;
    }

    /**
     * Returns the timestamp when the command was acknowledged.
     *
     * @return The acknowledgment timestamp.
     * */
    public String getAckAt() {
        return ackAt;
    }

    /**
     * Returns any result message associated with the command.
     *
     * @return The result message.
     * */
    public String getResultMessage() {
        return resultMessage;
    }
}