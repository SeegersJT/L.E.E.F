export type PairingErrorKind = 'invalid' | 'connection'

export interface PairingState {
	verifying: boolean
	verifyError: PairingErrorKind | null
	claiming: boolean
	claimError: PairingErrorKind | null
	deviceId: string | null
}
