import type { AuthState } from './Authentication.type'
import type { NotificationState } from './Notification.type'

export interface RootState {
	system: {
		notification: NotificationState
	}
	auth: AuthState
}
