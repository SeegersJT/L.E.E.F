import type { DeviceStatus } from '../types/Device.type'

export const DEVICE_ACTIONS = {
	SET_DEVICE_LIST: '[DEVICE] - SET_DEVICE_LIST',
	SET_DEVICE_STATUS: '[DEVICE] - SET_DEVICE_STATUS',
	CLEAR_DEVICES: '[DEVICE] - CLEAR_DEVICES',
} as const

export const setDeviceList = (payload: { id: string; nickname: string | null }[]) => ({
	type: DEVICE_ACTIONS.SET_DEVICE_LIST,
	payload,
})

export const setDeviceStatus = (payload: { id: string; status: DeviceStatus | null }) => ({
	type: DEVICE_ACTIONS.SET_DEVICE_STATUS,
	payload,
})

export const clearDevices = () => ({
	type: DEVICE_ACTIONS.CLEAR_DEVICES,
})
