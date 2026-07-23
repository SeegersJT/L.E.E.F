import type { DeviceRemovalErrorKind } from '../types/DeviceRemoval.type'

export const DEVICE_REMOVAL_ACTIONS = {
	REQUEST_REMOVE_DEVICE: '[DEVICE_REMOVAL] - REQUEST_REMOVE_DEVICE',
	REMOVE_DEVICE_LOADING: '[DEVICE_REMOVAL] - REMOVE_DEVICE_LOADING',
	REMOVE_DEVICE_ERROR: '[DEVICE_REMOVAL] - REMOVE_DEVICE_ERROR',
	RESET_DEVICE_REMOVAL: '[DEVICE_REMOVAL] - RESET_DEVICE_REMOVAL',
} as const

export const requestRemoveDevice = (payload: { deviceId: string; onSuccess?: () => void }) => ({
	type: DEVICE_REMOVAL_ACTIONS.REQUEST_REMOVE_DEVICE,
	payload,
})

export const removeDeviceLoading = (payload: boolean) => ({
	type: DEVICE_REMOVAL_ACTIONS.REMOVE_DEVICE_LOADING,
	payload,
})

export const removeDeviceError = (payload: DeviceRemovalErrorKind) => ({
	type: DEVICE_REMOVAL_ACTIONS.REMOVE_DEVICE_ERROR,
	payload,
})

export const resetDeviceRemoval = () => ({
	type: DEVICE_REMOVAL_ACTIONS.RESET_DEVICE_REMOVAL,
})
