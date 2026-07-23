export type DeviceRemovalErrorKind = 'failed' | 'connection'

export interface DeviceRemovalState {
	removing: boolean
	error: DeviceRemovalErrorKind | null
}
