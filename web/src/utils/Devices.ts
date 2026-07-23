import type {
	DeviceStatus,
	MoistureChannelStatus,
	RelayChannelStatus,
} from '@/redux/types/Device.type'

export type DeviceChannel =
	| { type: 'moisture'; key: string; data: MoistureChannelStatus }
	| { type: 'relay'; key: string; data: RelayChannelStatus }

export const listChannels = (status: DeviceStatus | null | undefined): DeviceChannel[] => {
	if (!status) return []

	const channels: DeviceChannel[] = []

	if (status.MOISTURE_SENSOR_PIN_MM01) {
		channels.push({
			type: 'moisture',
			key: 'MOISTURE_SENSOR_PIN_MM01',
			data: status.MOISTURE_SENSOR_PIN_MM01,
		})
	}

	if (status.RELAY_PIN_R01) {
		channels.push({
			type: 'relay',
			key: 'RELAY_PIN_R01',
			data: status.RELAY_PIN_R01,
		})
	}

	return channels
}

const ONLINE_THRESHOLD_MS = 5 * 60 * 1000

export const isOnline = (lastSeen: string | undefined): boolean => {
	if (!lastSeen) return false
	const seenAt = new Date(lastSeen).getTime()
	if (Number.isNaN(seenAt)) return false
	return Date.now() - seenAt < ONLINE_THRESHOLD_MS
}
