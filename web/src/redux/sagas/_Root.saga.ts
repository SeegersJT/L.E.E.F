import { all } from 'redux-saga/effects'
import { notificationSaga } from './Notification.saga'
import { authSaga } from './Authentication.saga'

export function* RootSaga() {
	yield all([notificationSaga(), authSaga()])
}
