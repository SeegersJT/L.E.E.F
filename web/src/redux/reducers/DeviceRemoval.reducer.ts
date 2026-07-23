import { DEVICE_REMOVAL_ACTIONS } from '../actions/DeviceRemoval.action'
import type { DeviceRemovalErrorKind, DeviceRemovalState } from '../types/DeviceRemoval.type'

const initialState: DeviceRemovalState = {
	removing: false,
	error: null,
}

type Action = { type: string; payload?: unknown }

export const DeviceRemovalReducer = (state = initialState, action: Action): DeviceRemovalState => {
	switch (action.type) {
		case DEVICE_REMOVAL_ACTIONS.REMOVE_DEVICE_LOADING:
			return { removing: action.payload as boolean, error: null }

		case DEVICE_REMOVAL_ACTIONS.REMOVE_DEVICE_ERROR:
			return { ...state, error: action.payload as DeviceRemovalErrorKind }

		case DEVICE_REMOVAL_ACTIONS.RESET_DEVICE_REMOVAL:
			return initialState

		default:
			return state
	}
}
