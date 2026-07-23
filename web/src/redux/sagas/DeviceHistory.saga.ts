import { call, put, takeLatest } from 'redux-saga/effects'
import { rtdbService, type DeviceHistoryResult } from '@/firebase'
import {
	DEVICE_HISTORY_ACTIONS,
	deviceHistoryLoading,
	deviceHistorySuccess,
	deviceHistoryError,
} from '../actions/DeviceHistory.action'

function* handleRequestDeviceHistory(action: {
	type: string
	payload: { deviceId: string; sinceMs: number }
}) {
	yield put(deviceHistoryLoading(true))

	try {
		const result: DeviceHistoryResult = yield call(
			[rtdbService, rtdbService.fetchDeviceHistory],
			action.payload.deviceId,
			action.payload.sinceMs
		)

		yield put(
			deviceHistorySuccess({
				deviceId: action.payload.deviceId,
				moisture: result.moisture,
				relay: result.relay,
			})
		)
	} catch {
		yield put(deviceHistoryError())
	} finally {
		yield put(deviceHistoryLoading(false))
	}
}

export function* deviceHistorySaga() {
	yield takeLatest(DEVICE_HISTORY_ACTIONS.REQUEST_DEVICE_HISTORY, handleRequestDeviceHistory)
}
