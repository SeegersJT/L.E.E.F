import { all } from 'redux-saga/effects'
import { notificationSaga } from './Notification.saga'
import { authSaga } from './Authentication.saga'
import { deviceSaga } from './Device.saga'
import { pairingSaga } from './Pairing.saga'
import { deviceRemovalSaga } from './DeviceRemoval.saga'
import { deviceHistorySaga } from './DeviceHistory.saga'

export function* RootSaga() {
	yield all([
		notificationSaga(),
		authSaga(),
		deviceSaga(),
		pairingSaga(),
		deviceHistorySaga(),
		deviceRemovalSaga(),
	])
}
