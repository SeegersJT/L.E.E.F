export interface MoistureChannelStatus {
	label: string
	pin: number
	reading: number
	unit: string
	timestamp: string
	intervalMinutes: number
}

export interface RelayChannelStatus {
	label: string
	pin: number
	state: 'ON' | 'OFF'
	timestamp: string
	onDurationMs: number
}

export interface DeviceStatus {
	MOISTURE_SENSOR_PIN_MM01?: MoistureChannelStatus
	RELAY_PIN_R01?: RelayChannelStatus
	appliedFirmwareDate?: string
	lastOtaResult?: string
	lastOtaCheckTime?: string
	lastSeen?: string
}

export interface Device {
	id: string
	nickname: string | null
	status: DeviceStatus | null
}

export interface DeviceState {
	devices: Device[] | null
}
