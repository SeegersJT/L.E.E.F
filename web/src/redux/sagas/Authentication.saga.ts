import { call, put, takeLatest } from 'redux-saga/effects'
import { AUTH_ACTIONS, requestFirebaseEmailLoginLoading } from '../actions/Authentication.action'
import type { LoginCredentials } from '../types/Authentication.type'
import type { User } from 'firebase/auth'
import { authService } from '@/firebase'
import { navigate } from '@/utils/Navigator'
import { addSystemNotification } from '../actions/Notification.action'

function* handleFirebaseEmailLoginRequest(action: { type: string; payload: LoginCredentials }) {
	yield put(requestFirebaseEmailLoginLoading(true))

	try {
		const user: User = yield call([authService, authService.login], action.payload)
		// yield put(setAuthUser(convertToAuthUser(user)))

		yield put(
			addSystemNotification({
				type: 'success',
				title: 'Login',
				message: 'Successfully Logged into L.E.E.F. Companion',
			})
		)
		yield call(navigate, '/dashboard/account')
	} catch (err) {
		const message = err instanceof Error ? err.message : 'Login failed'
		yield put(
			addSystemNotification({
				type: 'error',
				title: 'Login',
				message: 'Failed to Log into L.E.E.F. Companion',
			})
		)
	} finally {
		yield put(requestFirebaseEmailLoginLoading(false))
	}
}

export function* authSaga() {
	yield takeLatest(AUTH_ACTIONS.REQUEST_FIREBASE_EMAIL_LOGIN, handleFirebaseEmailLoginRequest)
}
