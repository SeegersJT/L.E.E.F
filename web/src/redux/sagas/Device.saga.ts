import { eventChannel, type EventChannel, type Task } from 'redux-saga'
import { call, put, take, fork, cancel, cancelled, takeLatest } from 'redux-saga/effects'
import { rtdbService, type UserDeviceEntry } from '@/firebase'
import { AUTH_ACTIONS } from '../actions/Authentication.action'
import { setDeviceList, setDeviceStatus, clearDevices } from '../actions/Device.action'
import type { AuthUser } from '../types/Authentication.type'
import type { DeviceStatus } from '../types/Device.type'

function createUserDevicesChannel(uid: string) {
	return eventChannel<UserDeviceEntry[]>(emit => rtdbService.subscribeToUserDevices(uid, emit))
}

function createDeviceStatusChannel(deviceId: string) {
	return eventChannel<DeviceStatus | null>(emit =>
		rtdbService.subscribeToDeviceStatus(deviceId, emit)
	)
}

function* watchDeviceStatus(deviceId: string) {
	const channel: EventChannel<DeviceStatus | null> = yield call(
		createDeviceStatusChannel,
		deviceId
	)
	try {
		while (true) {
			const status: DeviceStatus | null = yield take(channel)
			yield put(setDeviceStatus({ id: deviceId, status }))
		}
	} finally {
		const wasCancelled: boolean = yield cancelled()
		if (wasCancelled) {
			channel.close()
		}
	}
}

function* watchUserDevices(uid: string) {
	const channel: EventChannel<UserDeviceEntry[]> = yield call(createUserDevicesChannel, uid)
	const statusTasks = new Map<string, Task>()

	try {
		while (true) {
			const entries: UserDeviceEntry[] = yield take(channel)
			const currentIds = new Set(entries.map(e => e.id))

			for (const [id, task] of statusTasks) {
				if (!currentIds.has(id)) {
					yield cancel(task)
					statusTasks.delete(id)
				}
			}

			for (const id of currentIds) {
				if (!statusTasks.has(id)) {
					const task: Task = yield fork(watchDeviceStatus, id)
					statusTasks.set(id, task)
				}
			}

			yield put(setDeviceList(entries))
		}
	} finally {
		const wasCancelled: boolean = yield cancelled()
		if (wasCancelled) {
			channel.close()
			for (const task of statusTasks.values()) {
				yield cancel(task)
			}
		}
	}
}

function* handleAuthChange(action: { type: string; payload: AuthUser | null }) {
	if (!action.payload) {
		yield put(clearDevices())
		return
	}
	yield call(watchUserDevices, action.payload.uid)
}

export function* deviceSaga() {
	yield takeLatest(AUTH_ACTIONS.SET_AUTH_USER, handleAuthChange)
}
