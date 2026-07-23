import { DEVICE_ACTIONS } from '../actions/Device.action'
import type { DeviceState, DeviceStatus } from '../types/Device.type'

const initialState: DeviceState = {
	devices: null,
}

type Action = { type: string; payload?: unknown }

export const DeviceReducer = (state = initialState, action: Action): DeviceState => {
	switch (action.type) {
		case DEVICE_ACTIONS.SET_DEVICE_LIST: {
			const entries = action.payload as { id: string; nickname: string | null }[]
			const existingById = new Map((state.devices ?? []).map(d => [d.id, d]))

			return {
				devices: entries.map(e => ({
					id: e.id,
					nickname: e.nickname,
					status: existingById.get(e.id)?.status ?? null,
				})),
			}
		}

		case DEVICE_ACTIONS.SET_DEVICE_STATUS: {
			const { id, status } = action.payload as { id: string; status: DeviceStatus | null }
			if (!state.devices) return state
			return {
				devices: state.devices.map(d => (d.id === id ? { ...d, status } : d)),
			}
		}

		case DEVICE_ACTIONS.CLEAR_DEVICES:
			return { devices: null }

		default:
			return state
	}
}
