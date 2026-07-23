import type { MoistureHistoryPoint, RelayHistoryPoint } from '../types/DeviceHistory.type'

export const DEVICE_HISTORY_ACTIONS = {
	REQUEST_DEVICE_HISTORY: '[DEVICE_HISTORY] - REQUEST_DEVICE_HISTORY',
	DEVICE_HISTORY_LOADING: '[DEVICE_HISTORY] - DEVICE_HISTORY_LOADING',
	DEVICE_HISTORY_SUCCESS: '[DEVICE_HISTORY] - DEVICE_HISTORY_SUCCESS',
	DEVICE_HISTORY_ERROR: '[DEVICE_HISTORY] - DEVICE_HISTORY_ERROR',
} as const

export const requestDeviceHistory = (payload: { deviceId: string; sinceMs: number }) => ({
	type: DEVICE_HISTORY_ACTIONS.REQUEST_DEVICE_HISTORY,
	payload,
})

export const deviceHistoryLoading = (payload: boolean) => ({
	type: DEVICE_HISTORY_ACTIONS.DEVICE_HISTORY_LOADING,
	payload,
})

export const deviceHistorySuccess = (payload: {
	deviceId: string
	moisture: MoistureHistoryPoint[]
	relay: RelayHistoryPoint[]
}) => ({
	type: DEVICE_HISTORY_ACTIONS.DEVICE_HISTORY_SUCCESS,
	payload,
})

export const deviceHistoryError = () => ({
	type: DEVICE_HISTORY_ACTIONS.DEVICE_HISTORY_ERROR,
})
