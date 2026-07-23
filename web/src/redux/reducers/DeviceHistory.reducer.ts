import { DEVICE_HISTORY_ACTIONS } from '../actions/DeviceHistory.action'
import type {
	DeviceHistoryState,
	MoistureHistoryPoint,
	RelayHistoryPoint,
} from '../types/DeviceHistory.type'

const initialState: DeviceHistoryState = {
	loading: false,
	error: false,
	deviceId: null,
	moisture: [],
	relay: [],
}

type Action = { type: string; payload?: unknown }

export const DeviceHistoryReducer = (state = initialState, action: Action): DeviceHistoryState => {
	switch (action.type) {
		case DEVICE_HISTORY_ACTIONS.DEVICE_HISTORY_LOADING:
			return { ...state, loading: action.payload as boolean, error: false }

		case DEVICE_HISTORY_ACTIONS.DEVICE_HISTORY_SUCCESS: {
			const payload = action.payload as {
				deviceId: string
				moisture: MoistureHistoryPoint[]
				relay: RelayHistoryPoint[]
			}
			return {
				...state,
				deviceId: payload.deviceId,
				moisture: payload.moisture,
				relay: payload.relay,
			}
		}

		case DEVICE_HISTORY_ACTIONS.DEVICE_HISTORY_ERROR:
			return { ...state, error: true }

		default:
			return state
	}
}
