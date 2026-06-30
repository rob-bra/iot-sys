CREATE TABLE IF NOT EXISTS devices (
    device_id TEXT PRIMARY KEY,
    status TEXT NOT NULL,
    battery INTEGER,
    last_seen TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS telemetry (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id TEXT NOT NULL,
    timestamp TEXT NOT NULL,
    temperature REAL NOT NULL,
    humidity REAL NOT NULL,
    pressure REAL NOT NULL,
	battery INTEGER,
    orientation TEXT,
    FOREIGN KEY (device_id) REFERENCES devices(device_id)
);