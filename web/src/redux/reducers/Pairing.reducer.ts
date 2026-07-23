import { PAIRING_ACTIONS } from '../actions/Pairing.action'
import type { PairingErrorKind, PairingState } from '../types/Pairing.type'

const initialState: PairingState = {
	verifying: false,
	verifyError: null,
	claiming: false,
	claimError: null,
	deviceId: null,
}

type Action = { type: string; payload?: unknown }

export const PairingReducer = (state = initialState, action: Action): PairingState => {
	switch (action.type) {
		case PAIRING_ACTIONS.VERIFY_CODE_LOADING:
			return { ...state, verifying: action.payload as boolean, verifyError: null }

		case PAIRING_ACTIONS.VERIFY_CODE_SUCCESS:
			return {
				...state,
				deviceId: (action.payload as { deviceId: string }).deviceId,
				verifyError: null,
			}

		case PAIRING_ACTIONS.VERIFY_CODE_ERROR:
			return { ...state, verifyError: action.payload as PairingErrorKind, deviceId: null }

		case PAIRING_ACTIONS.CLAIM_DEVICE_LOADING:
			return { ...state, claiming: action.payload as boolean, claimError: null }

		case PAIRING_ACTIONS.CLAIM_DEVICE_ERROR:
			return { ...state, claimError: action.payload as PairingErrorKind }

		case PAIRING_ACTIONS.RESET_PAIRING:
			return initialState

		default:
			return state
	}
}
