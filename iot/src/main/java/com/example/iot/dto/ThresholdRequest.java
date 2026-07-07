package com.example.iot.dto;

/**
 * Data Transfer Object utilizzato per la configurazione delle soglie di allarme
 * associate a un dispositivo.
 * <p>
 * Contiene i limiti minimi e massimi per temperatura, umidità e pressione.
 * 
 * Struttura del tipo di dato dell'oggetto di tipo TresholdRequest.
 */
public class ThresholdRequest {

	private Double temperatureMin;
	private Double temperatureMax;
	private Double humidityMin;
	private Double humidityMax;
	private Double pressureMin;
	private Double pressureMax;

	/**
	 * Costruttore di default richiesto dal framework per la deserializzazione JSON.
	 */
	public ThresholdRequest() {
	}

	/**
	 * Restituisce la soglia minima di temperatura.
	 *
	 * @return la soglia minima di temperatura
	 */
	public Double getTemperatureMin() {
		return temperatureMin;
	}

	/**
	 * Imposta la soglia minima di temperatura.
	 *
	 * @param temperatureMin la soglia minima di temperatura
	 */
	public void setTemperatureMin(Double temperatureMin) {
		this.temperatureMin = temperatureMin;
	}

	/**
	 * Restituisce la soglia massima di temperatura.
	 *
	 * @return la soglia massima di temperatura
	 */
	public Double getTemperatureMax() {
		return temperatureMax;
	}

	/**
	 * Imposta la soglia massima di temperatura.
	 *
	 * @param temperatureMax la soglia massima di temperatura
	 */
	public void setTemperatureMax(Double temperatureMax) {
		this.temperatureMax = temperatureMax;
	}

	/**
	 * Restituisce la soglia minima di umidità.
	 *
	 * @return la soglia minima di umidità
	 */
	public Double getHumidityMin() {
		return humidityMin;
	}

	/**
	 * Imposta la soglia minima di umidità.
	 *
	 * @param humidityMin la soglia minima di umidità
	 */
	public void setHumidityMin(Double humidityMin) {
		this.humidityMin = humidityMin;
	}

	/**
	 * Restituisce la soglia massima di umidità.
	 *
	 * @return la soglia massima di umidità
	 */
	public Double getHumidityMax() {
		return humidityMax;
	}

	/**
	 * Imposta la soglia massima di umidità.
	 *
	 * @param humidityMax la soglia massima di umidità
	 */
	public void setHumidityMax(Double humidityMax) {
		this.humidityMax = humidityMax;
	}

	/**
	 * Restituisce la soglia minima di pressione.
	 *
	 * @return la soglia minima di pressione
	 */
	public Double getPressureMin() {
		return pressureMin;
	}

	/**
	 * Imposta la soglia minima di pressione.
	 *
	 * @param pressureMin la soglia minima di pressione
	 */
	public void setPressureMin(Double pressureMin) {
		this.pressureMin = pressureMin;
	}

	/**
	 * Restituisce la soglia massima di pressione.
	 *
	 * @return la soglia massima di pressione
	 */
	public Double getPressureMax() {
		return pressureMax;
	}

	/**
	 * Imposta la soglia massima di pressione.
	 *
	 * @param pressureMax la soglia massima di pressione
	 */
	public void setPressureMax(Double pressureMax) {
		this.pressureMax = pressureMax;
	}
}
