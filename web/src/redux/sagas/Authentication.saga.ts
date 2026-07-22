import { call, put, takeLatest } from 'redux-saga/effects'
import type { User } from 'firebase/auth'
import {
	AUTH_ACTIONS,
	requestFirebaseEmailLoginLoading,
	requestFirebaseRegisterLoading,
	requestFirebaseGoogleLoginLoading,
	requestPasswordResetLoading,
	setAuthUser,
} from '../actions/Authentication.action'
import { addSystemNotification } from '../actions/Notification.action'
import type { LoginCredentials, RegisterCredentials, AuthUser } from '../types/Authentication.type'
import { authService } from '@/firebase'
import { navigate } from '@/utils/Navigator'

const toAuthUser = (user: User): AuthUser => ({
	uid: user.uid,
	email: user.email,
	displayName: user.displayName,
	photoURL: user.photoURL,
	emailVerified: user.emailVerified,
})

function* handleFirebaseEmailLoginRequest(action: { type: string; payload: LoginCredentials }) {
	yield put(requestFirebaseEmailLoginLoading(true))

	try {
		const user: User = yield call([authService, authService.login], action.payload)
		yield put(setAuthUser(toAuthUser(user)))

		yield put(
			addSystemNotification({
				type: 'success',
				title: 'Login',
				message: 'Successfully logged into L.E.E.F. Companion',
			})
		)
		yield call(() => navigate('/dashboard/account'))
	} catch (err) {
		yield put(
			addSystemNotification({
				type: 'error',
				title: 'Login',
				message:
					err instanceof Error ? err.message : 'Failed to log into L.E.E.F. Companion',
			})
		)
	} finally {
		yield put(requestFirebaseEmailLoginLoading(false))
	}
}

function* handleFirebaseRegisterRequest(action: { type: string; payload: RegisterCredentials }) {
	yield put(requestFirebaseRegisterLoading(true))

	try {
		const { fullName, email, password } = action.payload
		const user: User = yield call([authService, authService.register], {
			email,
			password,
			displayName: fullName,
		})
		yield put(setAuthUser(toAuthUser(user)))

		yield put(
			addSystemNotification({
				type: 'success',
				title: 'Account created',
				message: 'Welcome! Your L.E.E.F. account is ready.',
			})
		)
		yield call(() => navigate('/dashboard/account'))
	} catch (err) {
		yield put(
			addSystemNotification({
				type: 'error',
				title: 'Registration',
				message: err instanceof Error ? err.message : 'Failed to create your account',
			})
		)
	} finally {
		yield put(requestFirebaseRegisterLoading(false))
	}
}

function* handleFirebaseGoogleLoginRequest() {
	yield put(requestFirebaseGoogleLoginLoading(true))

	try {
		const user: User = yield call([authService, authService.loginWithGoogle])
		yield put(setAuthUser(toAuthUser(user)))

		yield put(
			addSystemNotification({
				type: 'success',
				title: 'Login',
				message: 'Successfully logged into L.E.E.F. Companion',
			})
		)
		yield call(() => navigate('/dashboard/account'))
	} catch (err) {
		yield put(
			addSystemNotification({
				type: 'error',
				title: 'Google sign-in',
				message: err instanceof Error ? err.message : 'Failed to sign in with Google',
			})
		)
	} finally {
		yield put(requestFirebaseGoogleLoginLoading(false))
	}
}

function* handlePasswordResetRequest(action: {
	type: string
	payload: { email: string; onSuccess?: () => void }
}) {
	yield put(requestPasswordResetLoading(true))

	try {
		yield call([authService, authService.resetPassword], action.payload.email)

		yield put(
			addSystemNotification({
				type: 'success',
				title: 'Password reset',
				message: 'Check your inbox for a reset link.',
			})
		)
		action.payload.onSuccess?.()
	} catch (err) {
		yield put(
			addSystemNotification({
				type: 'error',
				title: 'Password reset',
				message: err instanceof Error ? err.message : 'Failed to send reset link',
			})
		)
	} finally {
		yield put(requestPasswordResetLoading(false))
	}
}

export function* authSaga() {
	yield takeLatest(AUTH_ACTIONS.REQUEST_FIREBASE_EMAIL_LOGIN, handleFirebaseEmailLoginRequest)
	yield takeLatest(AUTH_ACTIONS.REQUEST_FIREBASE_REGISTER, handleFirebaseRegisterRequest)
	yield takeLatest(AUTH_ACTIONS.REQUEST_FIREBASE_GOOGLE_LOGIN, handleFirebaseGoogleLoginRequest)
	yield takeLatest(AUTH_ACTIONS.REQUEST_PASSWORD_RESET, handlePasswordResetRequest)
}
