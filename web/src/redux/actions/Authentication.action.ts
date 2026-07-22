import type { LoginCredentials, RegisterCredentials, AuthUser } from '../types/Authentication.type'

export const AUTH_ACTIONS = {
	REQUEST_FIREBASE_EMAIL_LOGIN: '[AUTH] - REQUEST_FIREBASE_EMAIL_LOGIN',
	REQUEST_FIREBASE_EMAIL_LOGIN_LOADING: '[AUTH] - REQUEST_FIREBASE_EMAIL_LOGIN_LOADING',

	REQUEST_FIREBASE_REGISTER: '[AUTH] - REQUEST_FIREBASE_REGISTER',
	REQUEST_FIREBASE_REGISTER_LOADING: '[AUTH] - REQUEST_FIREBASE_REGISTER_LOADING',

	REQUEST_FIREBASE_GOOGLE_LOGIN: '[AUTH] - REQUEST_FIREBASE_GOOGLE_LOGIN',
	REQUEST_FIREBASE_GOOGLE_LOGIN_LOADING: '[AUTH] - REQUEST_FIREBASE_GOOGLE_LOGIN_LOADING',

	REQUEST_PASSWORD_RESET: '[AUTH] - REQUEST_PASSWORD_RESET',
	REQUEST_PASSWORD_RESET_LOADING: '[AUTH] - REQUEST_PASSWORD_RESET_LOADING',

	SET_AUTH_USER: '[AUTH] - SET_AUTH_USER',
} as const

export const requestFirebaseEmailLogin = (payload: LoginCredentials) => ({
	type: AUTH_ACTIONS.REQUEST_FIREBASE_EMAIL_LOGIN,
	payload,
})

export const requestFirebaseEmailLoginLoading = (payload: boolean) => ({
	type: AUTH_ACTIONS.REQUEST_FIREBASE_EMAIL_LOGIN_LOADING,
	payload,
})

export const requestFirebaseRegister = (payload: RegisterCredentials) => ({
	type: AUTH_ACTIONS.REQUEST_FIREBASE_REGISTER,
	payload,
})

export const requestFirebaseRegisterLoading = (payload: boolean) => ({
	type: AUTH_ACTIONS.REQUEST_FIREBASE_REGISTER_LOADING,
	payload,
})

export const requestFirebaseGoogleLogin = () => ({
	type: AUTH_ACTIONS.REQUEST_FIREBASE_GOOGLE_LOGIN,
})

export const requestFirebaseGoogleLoginLoading = (payload: boolean) => ({
	type: AUTH_ACTIONS.REQUEST_FIREBASE_GOOGLE_LOGIN_LOADING,
	payload,
})

export const requestPasswordReset = (payload: { email: string; onSuccess?: () => void }) => ({
	type: AUTH_ACTIONS.REQUEST_PASSWORD_RESET,
	payload,
})

export const requestPasswordResetLoading = (payload: boolean) => ({
	type: AUTH_ACTIONS.REQUEST_PASSWORD_RESET_LOADING,
	payload,
})

export const setAuthUser = (payload: AuthUser | null) => ({
	type: AUTH_ACTIONS.SET_AUTH_USER,
	payload,
})
