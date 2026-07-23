import type { PairingErrorKind } from '../types/Pairing.type'

export const PAIRING_ACTIONS = {
	REQUEST_VERIFY_CODE: '[PAIRING] - REQUEST_VERIFY_CODE',
	VERIFY_CODE_LOADING: '[PAIRING] - VERIFY_CODE_LOADING',
	VERIFY_CODE_SUCCESS: '[PAIRING] - VERIFY_CODE_SUCCESS',
	VERIFY_CODE_ERROR: '[PAIRING] - VERIFY_CODE_ERROR',

	REQUEST_CLAIM_DEVICE: '[PAIRING] - REQUEST_CLAIM_DEVICE',
	CLAIM_DEVICE_LOADING: '[PAIRING] - CLAIM_DEVICE_LOADING',
	CLAIM_DEVICE_ERROR: '[PAIRING] - CLAIM_DEVICE_ERROR',

	RESET_PAIRING: '[PAIRING] - RESET_PAIRING',
} as const

export const requestVerifyCode = (payload: { code: string }) => ({
	type: PAIRING_ACTIONS.REQUEST_VERIFY_CODE,
	payload,
})

export const verifyCodeLoading = (payload: boolean) => ({
	type: PAIRING_ACTIONS.VERIFY_CODE_LOADING,
	payload,
})

export const verifyCodeSuccess = (payload: { deviceId: string }) => ({
	type: PAIRING_ACTIONS.VERIFY_CODE_SUCCESS,
	payload,
})

export const verifyCodeError = (payload: PairingErrorKind) => ({
	type: PAIRING_ACTIONS.VERIFY_CODE_ERROR,
	payload,
})

export const requestClaimDevice = (payload: {
	code: string
	deviceId: string
	nickname: string
	onSuccess?: () => void
}) => ({
	type: PAIRING_ACTIONS.REQUEST_CLAIM_DEVICE,
	payload,
})

export const claimDeviceLoading = (payload: boolean) => ({
	type: PAIRING_ACTIONS.CLAIM_DEVICE_LOADING,
	payload,
})

export const claimDeviceError = (payload: PairingErrorKind) => ({
	type: PAIRING_ACTIONS.CLAIM_DEVICE_ERROR,
	payload,
})

export const resetPairing = () => ({
	type: PAIRING_ACTIONS.RESET_PAIRING,
})
