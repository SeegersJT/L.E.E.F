import { call, put, select, takeLatest } from 'redux-saga/effects'
import { rtdbService } from '@/firebase'
import { addSystemNotification } from '../actions/Notification.action'
import type { RootState } from '../types/_Root.type'
import {
	DEVICE_REMOVAL_ACTIONS,
	removeDeviceError,
	removeDeviceLoading,
} from '../actions/DeviceRemoval.action'

function* handleRequestRemoveDevice(action: {
	type: string
	payload: { deviceId: string; onSuccess?: () => void }
}) {
	yield put(removeDeviceLoading(true))

	try {
		const uid: string | undefined = yield select((state: RootState) => state.auth.user?.uid)

		if (!uid) {
			yield put(removeDeviceError('connection'))
			yield put(
				addSystemNotification({
					type: 'error',
					title: 'Not signed in',
					message: 'Your session may have expired - sign in again and retry.',
				})
			)
			return
		}

		yield call([rtdbService, rtdbService.removeDevice], {
			deviceId: action.payload.deviceId,
			uid,
		})

		yield put(
			addSystemNotification({
				type: 'success',
				title: 'Device removed',
				message: 'It can be paired again with a new code from its screen.',
			})
		)
		action.payload.onSuccess?.()
	} catch (err) {
		const errorCode = (err as { code?: string }).code ?? ''
		if (errorCode.toUpperCase().includes('PERMISSION_DENIED')) {
			yield put(removeDeviceError('failed'))
			yield put(
				addSystemNotification({
					type: 'error',
					title: 'Removal failed',
					message: "That device couldn't be removed - it may already be unpaired.",
				})
			)
		} else {
			yield put(removeDeviceError('connection'))
			yield put(
				addSystemNotification({
					type: 'error',
					title: "Couldn't remove device",
					message: 'Check your connection and try again.',
				})
			)
		}
	} finally {
		yield put(removeDeviceLoading(false))
	}
}

export function* deviceRemovalSaga() {
	yield takeLatest(DEVICE_REMOVAL_ACTIONS.REQUEST_REMOVE_DEVICE, handleRequestRemoveDevice)
}
