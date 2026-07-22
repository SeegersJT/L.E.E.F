import { delay, fork, put, takeEvery } from 'redux-saga/effects'
import type { Notification } from '../types/Notification.type'
import { NOTIFICATION_ACTIONS, removeSystemNotification } from '../actions/Notification.action'

function* autoRemoveSystemNotification(notification: Notification) {
	yield delay(notification.duration ?? 5000)
	yield put(removeSystemNotification(notification.id))
}

function* handleSystemNotificationAdd(action: { type: string; payload: Notification }) {
	yield fork(autoRemoveSystemNotification, action.payload)
}

export function* notificationSaga() {
	yield takeEvery(NOTIFICATION_ACTIONS.ADD_SYSTEM_NOTIFICATION, handleSystemNotificationAdd)
}
