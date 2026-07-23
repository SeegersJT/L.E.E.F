export interface MoistureHistoryPoint {
	timestamp: string
	reading: number
}

export interface RelayHistoryPoint {
	timestamp: string
	state: 'ON' | 'OFF'
}

export interface DeviceHistoryState {
	loading: boolean
	error: boolean
	deviceId: string | null
	moisture: MoistureHistoryPoint[]
	relay: RelayHistoryPoint[]
}
