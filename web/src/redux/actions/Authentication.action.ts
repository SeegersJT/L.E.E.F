import type { LoginCredentials } from '../types/Authentication.type'

export const AUTH_ACTIONS = {
	REQUEST_FIREBASE_EMAIL_LOGIN: '[AUTH] - FIREBASE EMAIL LOGIN - REQUEST',
	REQUEST_FIREBASE_EMAIL_LOGIN_LOADING: '[AUTH] - FIREBASE EMAIL LOGIN - REQUEST - LOADING',
} as const

export const requestFirebaseEmailLogin = (payload: LoginCredentials) => ({
	type: AUTH_ACTIONS.REQUEST_FIREBASE_EMAIL_LOGIN,
	payload,
})

export const requestFirebaseEmailLoginLoading = (loading: boolean) => ({
	type: AUTH_ACTIONS.REQUEST_FIREBASE_EMAIL_LOGIN_LOADING,
	payload: loading,
})
